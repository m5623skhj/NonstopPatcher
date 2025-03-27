#pragma once

#include <string>
#include <unordered_map>

enum class DLLType : short
{
    Unknwon,
    Func_1,
    Func_2,
};

const std::unordered_map<DLLType, std::string> dllPath = 
{
    {DLLType::Unknwon, ""},
    {DLLType::Func_1, "../../DLL/TestDll_1.dll"},
    {DLLType::Func_2, "../../DLL/TestDll_2_1.dll"},
};

const std::unordered_map<std::string, DLLType> dllNameToType = 
{
    {"Unknwon", DLLType::Unknwon},
    {"Func_1", DLLType::Func_1},
    {"Func_2", DLLType::Func_2},
};

const std::unordered_map<DLLType, std::string> typeToDLLName = 
{
    {DLLType::Unknwon, "Unknwon"},
    {DLLType::Func_1, "Func_1"},
    {DLLType::Func_2, "Func_2"},
};

