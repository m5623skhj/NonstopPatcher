#include "PatchOperatorReceiver.h"
#include <iostream>
#include "../Common/StringCommon.h"
#include "DLLLoader.h"

PatchOperatorReceiver& PatchOperatorReceiver::GetInst()
{
	static PatchOperatorReceiver instance;
	return instance;
}

bool PatchOperatorReceiver::StartReceive(std::wstring&& inPipeName)
{
	pipeName = std::move(inPipeName);
	if (CreatePipe())
	{
		return false;
	}

	receiverThread = std::thread([this]() { this->RunOperatorThread(); });
	return true;
}

void PatchOperatorReceiver::StopOperator()
{
	isRunning = true;
	CloseHandle(pipeHandle);
}

bool PatchOperatorReceiver::CreatePipe()
{
	pipeHandle = CreateFile(pipeName.c_str()
		, PIPE_ACCESS_DUPLEX
		, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT
		, NULL
		, OPEN_EXISTING
		, 0
		, NULL);
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreatePipe() failed with error code " << GetLastError() << std::endl;
		return false;
	}

	return true;
}

void PatchOperatorReceiver::RunOperatorThread()
{
	if (not ConnectNamedPipe(pipeHandle, NULL))
	{
		return;
	}

	constexpr int bufferSize{ 1024 };
	char buffer[bufferSize];
	DWORD recvBytes;

	while (isRunning)
	{
		if (ReadFile(pipeHandle, buffer, bufferSize, &recvBytes, NULL))
		{
			if (recvBytes >= bufferSize)
			{
				std::cout << "Invalid recv size " << recvBytes << std::endl;
				continue;
			}

			buffer[recvBytes] = '\0';
			ConvertBufferToOperation(buffer);
		}
		else
		{
			std::cout << "ReadFile() failed with " << GetLastError() << std::endl;
		}
	}
}

void PatchOperatorReceiver::ConvertBufferToOperation(const char* buffer)
{
	std::string recvString = buffer;
	
	auto inputOpt = SplitByCharacter(recvString, ';');
	if (not inputOpt.has_value())
	{
		return;
	}
	auto order = inputOpt.value().first;

	if (order == "DLLPathChange")
	{
		AsyncDLLChange(inputOpt.value().second);
	}
	else if (order == "Print")
	{
		SendDLLList();
	}
}

void PatchOperatorReceiver::AsyncDLLChange(const std::string& recvString)
{
	auto inputOpt = SplitByCharacter(recvString, ',');
	if (not inputOpt.has_value())
	{
		return;
	}

	DLLType dllType{};
	std::string newDLLPath{};
	try
	{
		dllType = static_cast<DLLType>(std::stoi(inputOpt.value().first));
		newDLLPath = inputOpt.value().second;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return;
	}
	
	DLLManager::GetInst().LoadDLLAsync(dllType, newDLLPath);
}

void PatchOperatorReceiver::SendDLLList()
{

}