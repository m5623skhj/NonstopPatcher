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
	constexpr int sleepTime{ 5000 };
	while (isRunning)
	{
		system("cls");
		std::cout << "-----------------------------" << std::endl;
		std::cout << "| PatchOperatorSender Start |" << std::endl;
		std::cout << "-----------------------------" << std::endl << std::endl;
		std::cout << "q : Stop this program" << std::endl;
		std::cout << "1 : Operation dll swap to patch target" << std::endl;
		std::cout << "2 : Print patch target dll list" << std::endl;

		std::cin >> userInput;
		system("cls");
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
			continue;
		}
		break;
		}

		if (userInput != 'q')
		{
			std::cout << std::endl << std::endl << "Press any key" << std::endl;
			std::cin.get();
		}
	}

	CloseHandle(pipeHandle);
	std::cout << "PatchOperatorSender Stop" << std::endl;
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
		std::cout << "SendMessageToReceiver() failed, pipeHandle is invalid" << std::endl;
		return;
	}

	std::string message{};
	DWORD sendBytes{};
	if (not WriteFile(pipeHandle, message.c_str(), static_cast<DWORD>(message.length()), &sendBytes, NULL))
	{
		std::cout << "WriteFile() failed in SendMessageToReceiver() with " << GetLastError() << std::endl;
		return;
	}
}

void PatchOperatorSender::PrintReceiverDLLState()
{
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "PrintReceiverDLLState() failed, pipeHandle is invalid" << std::endl;
		return;
	}

	constexpr const char* sendMessage = "Print";
	DWORD sendBytes{};
	if (not WriteFile(pipeHandle, sendMessage, static_cast<DWORD>(strlen(sendMessage)), &sendBytes, NULL))
	{
		std::cout << "WriteFile() failed in PrintReceiverDLLState() with " << GetLastError() << std::endl;
		return;
	}

	constexpr int bufferSize{ 4096 };
	char buffer[bufferSize];
	DWORD recvBytes;
	if (ReadFile(pipeHandle, buffer, bufferSize, &recvBytes, NULL))
	{
		std::cout << "ReadFile() failed in PrintReceiverDLLState() with " << GetLastError() << std::endl;
		return;
	}

	std::cout << "----------------------------------------------------" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "----------------------------------------------------" << std::endl;
}