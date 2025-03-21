#pragma once

#include <string>
#include <unordered_map>

enum class DLLType {
    Unknwon,
    Func_1,
    Func_2,
};

const std::unordered_map<DLLType, std::string> DLLPath = {
    {DLLType::Unknwon, ""},
    {DLLType::Func_1, "../DLL/Func_1.dll"},
    {DLLType::Func_2, "../DLL/Func_2.dll"},
};
