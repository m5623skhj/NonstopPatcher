#include "PatchOperatorReceiver.h"
#include <iostream>
#include "../Common/StringCommon.h"
#include "../Common/DLLType.h"
#include "DLLLoader.h"

PatchOperatorReceiver& PatchOperatorReceiver::GetInst()
{
	static PatchOperatorReceiver instance;
	return instance;
}

bool PatchOperatorReceiver::StartReceive(const std::wstring& inPipeName)
{
	pipeName = inPipeName;
	if (not CreatePipe())
	{
		return false;
	}

	receiverThread = std::thread([this]() { this->RunOperatorThread(); });
	return true;
}

void PatchOperatorReceiver::StopOperator()
{
	isRunning = false;
	CloseHandle(pipeHandle);
}

bool PatchOperatorReceiver::CreatePipe()
{
	pipeHandle = CreateNamedPipe(pipeName.c_str()
		, PIPE_ACCESS_DUPLEX
		, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT
		, PIPE_UNLIMITED_INSTANCES
		, 0
		, 0
		, 0
		, nullptr);
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreatePipe() failed with error code " << GetLastError() << '\n';
		return false;
	}

	return true;
}

void PatchOperatorReceiver::RunOperatorThread() const
{
	if (not ConnectNamedPipe(pipeHandle, nullptr))
	{
		return;
	}

	constexpr int bufferSize{ 1024 };
	char buffer[bufferSize];
	DWORD recvBytes;

	while (isRunning)
	{
		if (ReadFile(pipeHandle, buffer, bufferSize, &recvBytes, nullptr))
		{
			if (recvBytes >= bufferSize)
			{
				std::cout << "Invalid recv size " << recvBytes << '\n';
				continue;
			}

			buffer[recvBytes] = '\0';
			ConvertBufferToOperation(buffer);
		}
		else
		{
			std::cout << "ReadFile() failed with " << GetLastError() << '\n';
		}
	}
}

void PatchOperatorReceiver::ConvertBufferToOperation(const char* buffer) const
{
	const std::string recvString = buffer;

	const auto inputOpt = SplitByCharacter(recvString, ';');
	if (not inputOpt.has_value())
	{
		return;
	}

	if (const auto order = inputOpt.value().first; order == "DLLPathChange")
	{
		AsyncDllChange(inputOpt.value().second);
	}
	else if (order == "Print")
	{
		SendDllList();
	}
}

void PatchOperatorReceiver::AsyncDllChange(const std::string& recvString)
{
	const auto inputOpt = SplitByCharacter(recvString, ',');
	if (not inputOpt.has_value())
	{
		return;
	}

	DLLType dllType{};
	std::string newDllPath{};
	try
	{
		dllType = static_cast<DLLType>(std::stoi(inputOpt.value().first));
		newDllPath = inputOpt.value().second;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << '\n';
		return;
	}
	
	DLLManager::GetInst().LoadDllAsync(dllType, newDllPath);
}

void PatchOperatorReceiver::SendDllList() const
{
	std::string dllList{};

	for (const auto dllPaths = DLLManager::GetInst().GetDllPaths(); const auto& [dllType, path] : dllPaths)
	{
		auto itor = typeToDLLName.find(dllType);
		if (itor == typeToDLLName.end())
		{
			continue;
		}

		dllList += itor->second + ", " + path + '\n';
	}

	DWORD sendBytes{};
	if (not WriteFile(pipeHandle, dllList.c_str(), static_cast<DWORD>(dllList.length()), &sendBytes, nullptr))
	{
		std::cout << "WriteFile() failed in SendDLLList() with " << GetLastError() << '\n';
		return;
	}
}
