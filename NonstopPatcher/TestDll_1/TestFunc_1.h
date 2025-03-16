#pragma once
#include "../NonstopPatcher/PlugIn.h"

class TestFunc_1 : public IPlugIn
{
public:
	void ExecuteSomething(Parameters& params) override;
};