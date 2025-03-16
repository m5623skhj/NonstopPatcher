#pragma once
#include <thread>
#include <map>
#include <Windows.h>
#include <string>
#include <shared_mutex>
#include "DLLType.h"
#include <list>

struct DLLInfo
{
	std::string dllPath{};
	HMODULE dllHandle{};
	std::unique_ptr<std::shared_mutex> mutex{};
};

class DLLLoader
{
public:
	DLLLoader& GetInst();

private:
	DLLLoader() = default;
	~DLLLoader() = default;

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