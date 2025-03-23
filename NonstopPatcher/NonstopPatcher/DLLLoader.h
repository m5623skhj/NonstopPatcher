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

template<typename ReturnType, typename... InputType>
concept VoidReturnType = std::is_void_v<ReturnType>;

class DLLInfo
{
public:
	DLLInfo() = delete;
	explicit DLLInfo(const std::string& inDllPath)
		: dllPath(inDllPath)
		, mutex(std::make_shared<std::shared_mutex>())
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
	bool AddFunction(const std::string& functionName) 
	{
		std::unique_lock lock(*mutex);

		auto dllFunc = (ReturnParamType(*)(InputParamTypes&...))GetProcAddress(dllHandle, functionName.c_str());
		if (dllFunc == nullptr)
		{
			return false;
		}

		std::function<ReturnParamType(InputParamTypes&...)> func = [dllFunc](InputParamTypes&... inputParams) -> ReturnParamType
		{
			return dllFunc(inputParams...);
		};
		functions[functionName] = std::any(func);

		return true;
	}

	template<typename ReturnParamType, typename... InputParamTypes>
	requires(not VoidReturnType<ReturnParamType>)
	std::optional<ReturnParamType> CallFunction(const std::string& functionName, InputParamTypes&... inputParams)
	{
		std::shared_lock lock(*mutex);

		auto itor = functions.find(functionName);
		if (itor == functions.end())
		{
			return std::nullopt;
		}

		std::function<ReturnParamType(InputParamTypes&...)> func = std::any_cast<std::function<ReturnParamType(InputParamTypes&...)>>(itor->second);
		return func(inputParams...);
	}

	template<typename ReturnParamType, typename... InputParamTypes>
	requires(VoidReturnType<ReturnParamType>)
	void CallFunction(const std::string& functionName, InputParamTypes&... inputParams)
	{
		auto it = functions.find(functionName);
		if (it == functions.end())
		{
			return;
		}

		std::function<void(InputParamTypes&...)> func = std::any_cast<std::function<void(InputParamTypes&...)>>(it->second);
		func(inputParams...);
	}

private:
	std::string dllPath{};
	HMODULE dllHandle{};
	std::shared_ptr<std::shared_mutex> mutex;
	std::map<std::string, std::any> functions{};
};

class DLLManager
{
public:
	static DLLManager& GetInst();

private:
	DLLManager() = default;
	~DLLManager() = default;

public:
	void StartThread();
	void StopThread();

	bool LoadDLL(const DLLType dllType, const std::string& dllPath);
	void LoadDLLAsync(const DLLType dllType, const std::string& dllPath);
	void UnloadDLL(const DLLType dllType);

public:
	template<typename ReturnParamType, typename... InputParamTypes>
	bool AddFunction(const DLLType dllType, const std::string& functionName)
	{
		std::shared_lock lock(dllHandlesMutex);

		auto itor = dllHandles.find(dllType);
		if (itor == dllHandles.end())
		{
			return false;
		}

		return itor->second.AddFunction<ReturnParamType, InputParamTypes&...>(functionName);
	}

	template<typename ReturnParamType, typename... InputParamTypes>
	requires(not VoidReturnType<ReturnParamType>)
	std::optional<ReturnParamType> CallFunction(const DLLType dllType, const std::string& functionName, InputParamTypes&... inputParams)
	{
		std::shared_lock lock(dllHandlesMutex);

		auto itor = dllHandles.find(dllType);
		if (itor == dllHandles.end())
		{
			return std::nullopt;
		}

		return itor->second.CallFunction<ReturnParamType, InputParamTypes&...>(functionName, inputParams...);
	}

	template<typename ReturnParamType, typename... InputParamTypes>
	requires(VoidReturnType<ReturnParamType>)
	void CallFunction(DLLType dllType, const std::string& functionName, InputParamTypes&... inputParams)
	{
		std::shared_lock lock(dllHandlesMutex);

		auto itor = dllHandles.find(dllType);
		if (itor == dllHandles.end())
		{
			return;
		}

		itor->second.CallFunction<void, InputParamTypes&...>(functionName, inputParams...);
	}

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