#include "DLLLoader.h"
#include <iostream>

DLLInfo::DLLInfo(const std::string& inDllPath)
	: dllPath(inDllPath)
	, mutex(std::make_shared<std::shared_mutex>())
{
}

DLLInfo::~DLLInfo()
{
	if (dllHandle != nullptr)
	{
		FreeLibrary(dllHandle);
	}
}

DLLManager& DLLManager::GetInst()
{
	static DLLManager instance;
	return instance;
}

void DLLManager::StartThread()
{
	threadEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	dllLoadThread = std::thread([this]() { RunDllLoaderThread(); });
}

void DLLManager::StopThread()
{
	threadStop = true;
	SetEvent(threadEvent);
	dllLoadThread.join();
}

bool DLLManager::FirstLoadDll(const DLLType dllType, const std::string& inDllPath)
{
	{
		std::shared_lock lock(dllHandlesMutex);
		if (dllHandles.contains(dllType))
		{
			return false;
		}
	}

	return LoadDll(dllType, inDllPath);
}

bool DLLManager::LoadDll(const DLLType dllType, const std::string& inDllPath)
{
	{
		std::unique_lock lock(dllHandlesMutex);
		const auto itor = dllHandles.try_emplace(dllType, DLLInfo{ inDllPath }).first;
		if (itor == dllHandles.end())
		{
			return false;
		}

		return itor->second.TryLoadLibrary();
	}
}

void DLLManager::LoadDllAsync(const DLLType dllType, const std::string& inDllPath)
{
	{
		std::unique_lock lock(dllLoadListLock);
		dllLoadList.emplace_back(dllType, inDllPath);
	}

	SetEvent(threadEvent);
}

void DLLManager::UnloadDll(const DLLType dllType)
{
	DLLInfo unloadTarget{};
	{
		std::unique_lock lock(dllHandlesMutex);
		const auto itor = dllHandles.find(dllType);
		if (itor == dllHandles.end())
		{
			return;
		}

		unloadTarget = itor->second;
		dllHandles.erase(itor);
	}

	unloadTarget.FreeLoadedLibrary();
}

void DLLManager::RunDllLoaderThread()
{
	std::list<std::pair<DLLType, std::string>> dllLoadListCopy;
	while (not threadStop)
	{
		if (WaitForSingleObject(threadEvent, INFINITE) == WAIT_OBJECT_0)
		{
			{
				std::unique_lock lock(dllLoadListLock);
				for (auto& dll : dllLoadList)
				{
					dllLoadListCopy.emplace_back(std::move(dll));
				}
				dllLoadList.clear();
			}

			for (const auto& [dllType, inDllPath] : dllLoadListCopy)
			{
				UnloadDll(dllType);
				if (not LoadDll(dllType, inDllPath))
				{
					std::cout << "RunDLLLoaderThread() failed to load DLL with type " << static_cast<int>(dllType) << " and path " << inDllPath << '\n';
				}
			}
		}
		else
		{
			std::cout << "RunDLLLoaderThread() failed to wait for thread event with " << GetLastError() << '\n';
			break;
		}
	}
}
