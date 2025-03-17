#pragma once
#include <thread>

class PatchOperatorReceiver
{
public:
	static PatchOperatorReceiver& GetInst();

private:
	PatchOperatorReceiver() = default;
	~PatchOperatorReceiver() = default;

public:
	void StartOperator();
	void StopOperator();

private:
	void RunOperatorThread();

private:
	std::thread receiverThread{};
};