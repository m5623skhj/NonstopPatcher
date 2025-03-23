#pragma once
#include <thread>
#include <Windows.h>
#include <string>

class PatchOperatorReceiver
{
public:
	static PatchOperatorReceiver& GetInst();

private:
	PatchOperatorReceiver() = default;
	~PatchOperatorReceiver() = default;

public:
	bool StartReceive(std::wstring&& inPipeName);
	void StopOperator();

private:
	bool CreatePipe();
	void RunOperatorThread();
	void ConvertBufferToOperation(const char* buffer);

private:
	std::thread receiverThread{};
	HANDLE pipeHandle{};

	std::wstring pipeName{};

	bool isRunning{};
	const DWORD sleepTime{1000};
};