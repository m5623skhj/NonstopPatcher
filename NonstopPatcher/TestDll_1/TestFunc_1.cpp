#include "pch.h"
#include "TestFunc_1.h"
#include <iostream>

void TestFunc_1::ExecuteSomething(Parameters& params)
{
	int i;
	std::string s;
	float f;

	if (Plugin::GetParameters<int, std::string, float>(i, s, f, params))
	{
		std::cout << "TestFunc_1::ExecuteSomething() called successfully!" << std::endl;
	}
	else
	{
		std::cout << "TestFunc_1::ExecuteSomething() failed to call!" << std::endl;
	}
}