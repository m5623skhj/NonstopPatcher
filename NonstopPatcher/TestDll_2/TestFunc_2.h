#pragma once
#include "../NonstopPatcher/PlugIn.h"

class TestFunc_2 : public IPlugIn
{
public:
	void ExecuteSomething(Parameters& params) override;
};