#include <iostream>
#include "DLLLoader.h"

int main() 
{
	DLLManager::GetInst().StartThread();

	if (DLLManager::GetInst().LoadDLL(DLLType::Func_1, "../../DLL/TestDll_1.dll") == false||
		DLLManager::GetInst().LoadDLL(DLLType::Func_2, "../../DLL/TestDll_2.dll") == false)
	{
		std::cout << "LoadDLL() failed" << std::endl;
		return 0;
	}

	const std::string funcName = "ExecuteSomethingWrapper";
	if (DLLManager::GetInst().AddFunction<void, int&, std::string&, float&>(DLLType::Func_1, funcName) == false ||
		DLLManager::GetInst().AddFunction<std::string, char&>(DLLType::Func_2, funcName) == false)
	{
		std::cout << "AddFunction() failed" << std::endl;
		return 0;
	}

	int i = 1;
	std::string s = "param test";
	float f = 0.7f;
	DLLManager::GetInst().CallFunction<void, int&, std::string&, float&>(DLLType::Func_1, funcName, i, s, f);

	char c = 't';
	auto returnOpt = DLLManager::GetInst().CallFunction<std::string, char&>(DLLType::Func_2, funcName, c);
	if (not returnOpt.has_value())
	{
		std::cout << "Return is not has value" << std::endl << std::endl;
		return 0;
	}
	std::cout << "After call with " << c << " / " << *returnOpt << std::endl;

	DLLManager::GetInst().StopThread();

	std::cout << std::endl;
	return 0;
}