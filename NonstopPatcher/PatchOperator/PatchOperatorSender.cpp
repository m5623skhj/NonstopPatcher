#include "PatchOperatorSender.h"
#include <iostream>

PatchOperatorSender& PatchOperatorSender::GetInst()
{
	static PatchOperatorSender instance;
	return instance;
}

void PatchOperatorSender::StartOperator(std::wstring&& inPipeName)
{
	pipeName = std::move(inPipeName);
	if (CreatePipe())
	{
		return;
	}

	bool isRunning{ true };
	char userInput{};
	while (isRunning)
	{
		system("cls");
		std::cout << "-----------------------------" << std::endl;
		std::cout << "| PatchOperatorSender Start |" << std::endl;
		std::cout << "-----------------------------" << std::endl << std::endl;

		std::cin >> userInput;
		switch (userInput)
		{
		case 'q':
		{
			isRunning = false;
		}
		break;
		case 1:
		{
			SendMessageToReceiver();
		}
		break;
		case 2:
		{
			PrintReceiverDLLState();
		}
		break;
		default:
		{
		}
		break;
		}
	}

	std::cout << std::endl << std::endl << "PatchOperatorSender Stop" << std::endl;
}

bool PatchOperatorSender::CreatePipe()
{
	pipeHandle = CreateFile(pipeName.c_str(), PIPE_ACCESS_DUPLEX, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreatePipe() failed with error code " << GetLastError() << std::endl;
		return false;
	}

	return true;
}

void PatchOperatorSender::SendMessageToReceiver()
{
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	std::string message{};
	DWORD bytesWritten{};
	WriteFile(pipeHandle, message.c_str(), static_cast<DWORD>(message.length()), &bytesWritten, NULL);
}

void PatchOperatorSender::PrintReceiverDLLState()
{

}