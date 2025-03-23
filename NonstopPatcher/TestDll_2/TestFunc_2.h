#pragma once
#include <string>

class TestFunc_2
{
public:
	static std::string ExecuteSomething(char& params);
};

extern "C" __declspec(dllexport) std::string ExecuteSomethingWrapper(TestFunc_2* obj, char& params)
{
	return TestFunc_2::ExecuteSomething(params);
}
