#include <iostream>
#include "DLLLoader.h"
#include "PatchOperatorReceiver.h"

int main() 
{
	DLLManager::GetInst().StartThread();
	if (not PatchOperatorReceiver::GetInst().StartReceive(L"\\\\.\\pipe\\mypipe"))
	{
		std::cout << "StartReceive() failed" << '\n';
		return 0;
	}


	if (DLLManager::GetInst().FirstLoadDll(DLLType::Func_1, "../../DLL/TestDll_1.dll") == false||
		DLLManager::GetInst().FirstLoadDll(DLLType::Func_2, "../../DLL/TestDll_2.dll") == false)
	{
		std::cout << "FirstLoadDLL() failed" << '\n';
		return 0;
	}

	const std::string funcName = "ExecuteSomethingWrapper";
	if (DLLManager::GetInst().AddFunction<void, int&, std::string&, float&>(DLLType::Func_1, funcName) == false ||
		DLLManager::GetInst().AddFunction<std::string, char&>(DLLType::Func_2, funcName) == false)
	{
		std::cout << "AddFunction() failed" << '\n';
		return 0;
	}

	while (true)
	{
		int i = 1;
		std::string s = "param test";
		float f = 0.7f;
		DLLManager::GetInst().CallFunction<void, int&, std::string&, float&>(DLLType::Func_1, funcName, i, s, f);

		char c = 't';
		auto returnOpt = DLLManager::GetInst().CallFunction<std::string, char&>(DLLType::Func_2, funcName, c);
		if (not returnOpt.has_value())
		{
			std::cout << "Return is not has value" << '\n' << '\n';
			return 0;
		}
		std::cout << "After call with " << c << " / " << *returnOpt << '\n' << '\n';

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			break;
		}
		Sleep(1000);
		system("cls");
	}

	DLLManager::GetInst().StopThread();

	std::cout << '\n';
	return 0;
}