#pragma once
#include <thread>
#include <Windows.h>
#include <string>

class PatchOperatorSender
{
public:
	static PatchOperatorSender& GetInst();

private:
	PatchOperatorSender() = default;
	~PatchOperatorSender() = default;

public:
	void StartOperator(const std::wstring& inPipeName);

private:
	bool CreatePipe();
	void SendMessageToReceiver() const;
	void PrintReceiverDllState() const;

private:
	std::thread senderThread{};
	HANDLE pipeHandle{};

	std::wstring pipeName{};
};