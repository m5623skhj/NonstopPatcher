#include "PatchOperatorReceiver.h"
#include <iostream>

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

	constexpr int bufferSize{ 256 };
	char buffer[bufferSize];
	DWORD recvBytes;

	while (isRunning)
	{
		if (ReadFile(pipeHandle, buffer, bufferSize, &recvBytes, NULL))
		{
			if (recvBytes >= bufferSize)
			{
				// logging
				continue;
			}

			buffer[recvBytes] = '\0';
			ConvertBufferToOperation(buffer);
		}
		else
		{
			// logging
		}
	}
}

void PatchOperatorReceiver::ConvertBufferToOperation(const char* buffer)
{

}
