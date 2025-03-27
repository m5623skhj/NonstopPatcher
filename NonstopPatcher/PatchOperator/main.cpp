#include "PatchOperatorSender.h"

int main()
{
	Sleep(1500);
	PatchOperatorSender::GetInst().StartOperator(L"\\\\.\\pipe\\mypipe");

	return 0;
}