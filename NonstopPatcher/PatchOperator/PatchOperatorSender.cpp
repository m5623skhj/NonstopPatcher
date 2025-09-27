#include "PatchOperatorSender.h"
#include <iostream>
#include "../Common/DLLType.h"
#include "../Common/StringCommon.h"
#include <conio.h>

PatchOperatorSender& PatchOperatorSender::GetInst()
{
	static PatchOperatorSender instance;
	return instance;
}

void PatchOperatorSender::StartOperator(const std::wstring& inPipeName)
{
	pipeName = inPipeName;
	if (not CreatePipe())
	{
		return;
	}

	bool isRunning{ true };
	char userInput{};
	while (isRunning)
	{
		system("cls");
		std::cout << "-----------------------------" << '\n';
		std::cout << "| PatchOperatorSender Start |" << '\n';
		std::cout << "-----------------------------" << '\n' << '\n';
		std::cout << "q : Stop this program" << '\n';
		std::cout << "1 : Operation dll swap to patch target" << '\n';
		std::cout << "2 : Print patch target dll list" << '\n';

		std::cin >> userInput;
		system("cls");
		switch (userInput)
		{
		case 'q':
		{
			isRunning = false;
		}
		break;
		case '1':
		{
			SendMessageToReceiver();
		}
		break;
		case '2':
		{
			PrintReceiverDllState();
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
			Sleep(1000);
			std::cout << '\n' << '\n' << "Press any key" << '\n';
			std::ignore = _getch();
		}
	}

	CloseHandle(pipeHandle);
	std::cout << "PatchOperatorSender Stop" << '\n';
}

bool PatchOperatorSender::CreatePipe()
{
	pipeHandle = CreateFile(pipeName.c_str(), PIPE_ACCESS_DUPLEX, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreatePipe() failed with error code " << GetLastError() << '\n';
		return false;
	}

	return true;
}

void PatchOperatorSender::SendMessageToReceiver() const
{
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "SendMessageToReceiver() failed, pipeHandle is invalid" << '\n';
		return;
	}

	std::string input{};
	std::cin >> input;

	const auto dllTypeAndPathOpt = SplitByCharacter(input, ';');
	if (not dllTypeAndPathOpt.has_value())
	{
		return;
	}
	auto dllTypeAndPath = dllTypeAndPathOpt.value();

	const auto itor = dllNameToType.find(dllTypeAndPath.first);
	if (itor == dllNameToType.end())
	{
		std::cout << "Invalid DLL type " << dllTypeAndPath.first << '\n';
		return;
	}

	std::string message{"DLLPathChange;"};
	message += std::to_string(static_cast<short>(itor->second));
	message += "," + dllTypeAndPath.second;
	DWORD sendBytes{};
	if (not WriteFile(pipeHandle, message.c_str(), static_cast<DWORD>(message.length()), &sendBytes, nullptr))
	{
		std::cout << "WriteFile() failed in SendMessageToReceiver() with " << GetLastError() << '\n';
		return;
	}
}

void PatchOperatorSender::PrintReceiverDllState() const
{
	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "PrintReceiverDLLState() failed, pipeHandle is invalid" << '\n';
		return;
	}

	DWORD sendBytes{};
	if (constexpr auto sendMessage = "Print;"; not WriteFile(pipeHandle, sendMessage, static_cast<DWORD>(strlen(sendMessage)), &sendBytes, nullptr))
	{
		std::cout << "WriteFile() failed in PrintReceiverDLLState() with " << GetLastError() << '\n';
		return;
	}

	constexpr int bufferSize{ 8192 };
	char buffer[bufferSize];
	DWORD recvBytes;
	if (not ReadFile(pipeHandle, buffer, bufferSize, &recvBytes, nullptr))
	{
		std::cout << "ReadFile() failed in PrintReceiverDLLState() with " << GetLastError() << '\n';
		return;
	}

	if (recvBytes >= bufferSize)
	{
		std::cout << "PrintReceiverDLLState() recvBytes is bigger than bufferSize" << '\n';
		return;
	}
	buffer[recvBytes] = '\0';

	std::cout << "----------------------------------------------------" << '\n';
	std::cout << buffer << '\n';
	std::cout << "----------------------------------------------------" << '\n';
}
