#include "pch.h"
#include "TestFunc_2.h"
#include <iostream>

void TestFunc_2::ExecuteSomething(Parameters& params)
{
	char c;

	if (Plugin::GetParameters<char>(c, params))
	{
		std::cout << "TestFunc_2::ExecuteSomething() called successfully!" << std::endl;
	}
	else
	{
		std::cout << "TestFunc_2::ExecuteSomething() failed to call!" << std::endl;
	}
}