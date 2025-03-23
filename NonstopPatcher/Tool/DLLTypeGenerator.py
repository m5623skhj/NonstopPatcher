import yaml

def LoadYamlFile(fildPath):
    with open(fildPath, "r", encoding="utf-8") as file:
        data = yaml.safe_load(file)
    return data.get("DLLItems", [])

def GenerateDLLHeaderFile(dllItems, outFilePath):
    with open(outFilePath, "w", encoding="utf-8") as file:
        file.write("#pragma once\n\n")
        file.write("#include <string>\n")
        file.write("#include <unordered_map>\n\n")

        file.write("enum class DLLType : short\n{\n")
        for item in dllItems:
            file.write(f"    {item['Type']},\n")

        file.write("};\n\n")

        file.write("const std::unordered_map<DLLType, std::string> dllPath = \n{\n")
        for item in dllItems:
            file.write(f"    {{DLLType::{item['Type']}, \"{item['DLLPath']}\"}},\n")

        file.write("};\n\n")
        
        file.write("const std::unordered_map<std::string, DLLType> dllNameToType = \n{\n")
        for item in dllItems:
            file.write(f"    {{\"{item['Type']}\", DLLType::{item['Type']}}},\n")
            
        file.write("};\n\n")

    print("File create completed")

def main():
    yamlFilePath = "DLLType.yml"
    outFilePath = "../Common/DLLType.h"

    dllItems = LoadYamlFile(yamlFilePath)
    if not dllItems:
        print("Can't find dll items")
        return

    GenerateDLLHeaderFile(dllItems, outFilePath)

if __name__ == "__main__":
    main()
