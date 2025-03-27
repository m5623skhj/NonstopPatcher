#include "pch.h"
#include "TestFunc_1.h"
#include <iostream>

void TestFunc_1::ExecuteSomething(int& i, std::string& s, float& f)
{
	std::cout << "TestFunc_1" << std::endl;
	std::cout << i << std::endl;
	std::cout << s << std::endl;
	std::cout << f << std::endl << std::endl;
}