#pragma once
#include <thread>
#include <map>
#include <Windows.h>
#include <string>
#include <shared_mutex>
#include "../Common/DLLType.h"
#include <list>
#include <functional>
#include <optional>
#include <any>

class DLLInfo
{
public:
	DLLInfo() = delete;
	explicit DLLInfo(const std::string& inDllPath)
		: dllPath(inDllPath)
	{
	}

	~DLLInfo()
	{
		if (dllHandle != nullptr)
		{
			FreeLibrary(dllHandle);
		}
	}

public:
	bool TryLoadLibrary()
	{
		{
			std::unique_lock lock(*mutex);

			if (dllHandle != nullptr)
			{
				return false;
			}

			dllHandle = LoadLibraryA(dllPath.c_str());
			if (dllHandle == nullptr)
			{
				return false;
			}
		}
		
		return true;
	}

	void FreeLoadedLibrary()
	{
		{
			std::unique_lock lock(*mutex);

			if (dllHandle != nullptr)
			{

				FreeLibrary(dllHandle);
				dllHandle = nullptr;
			}
		}
	}

	template<typename ReturnParamType, typename... InputParamTypes>
	void AddFunction(const std::string& functionName, std::function<ReturnParamType(InputParamTypes...)> function)
	{
		std::unique_lock lock(*mutex);

		functions[functionName] = [function](InputParamTypes... inputParams)
		{
			function(inputParams...);
		};
	}

	template<typename ReturnParamType, typename... InputParamTypes>
	std::optional<ReturnParamType> CallFunction(const std::string& functionName, InputParamTypes... inputParams)
	{
		std::shared_lock lock(*mutex);

		auto itor = functions.find(functionName);
		if (itor == functions.end())
		{
			return std::nullopt;
		}

		const std::function<ReturnParamType(InputParamTypes...)>& func = std::any_cast<std::function<ReturnParamType(InputParamTypes...)>>(itor->second);
		return func(inputParams...);
	}

private:
	std::string dllPath{};
	HMODULE dllHandle{};
	std::shared_ptr<std::shared_mutex> mutex;
	std::map<std::string, std::function<void()>> functions{};
};

class DLLManager
{
public:
	DLLManager& GetInst();

private:
	DLLManager() = default;
	~DLLManager() = default;

public:
	void StopThread();

	bool LoadDLL(const DLLType dllType, const std::string& dllPath);
	void LoadDLLAsync(const DLLType dllType, const std::string& dllPath);
	void UnloadDLL(const DLLType dllType);

private:
	void RunDLLLoaderThread();

private:
	bool threadStop{};

	std::thread dllLoadThread;
	HANDLE threadEvent{};
	std::list<std::pair<DLLType, std::string>> dllLoadList;
	std::mutex dllLoadListLock;

	std::map<DLLType, DLLInfo> dllHandles;
	std::shared_mutex dllHandlesMutex;
};