#include "pch.h"
#include "TestFunc_2.h"
#include <iostream>

std::string TestFunc_2::ExecuteSomething(char& c)
{
	std::cout << "Test case 1" << std::endl;
	std::cout << "TestFunc_2 : " << c << std::endl;
	c = 'e';
	
	return "Return string";
}