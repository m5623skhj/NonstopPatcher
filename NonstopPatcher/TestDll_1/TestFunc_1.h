#pragma once
#include <string>
#include <iostream>

class TestFunc_1
{
public:
	static void ExecuteSomething(int& i, std::string& s, float& f);
};

extern "C" __declspec(dllexport) void ExecuteSomethingWrapper(int& i, std::string& s, float& f)
{
	TestFunc_1::ExecuteSomething(i, s, f);
}
