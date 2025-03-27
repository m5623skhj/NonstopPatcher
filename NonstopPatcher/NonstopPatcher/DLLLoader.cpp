#include "DLLLoader.h"
#include <iostream>

DLLManager& DLLManager::GetInst()
{
	static DLLManager instance;
	return instance;
}

void DLLManager::StartThread()
{
	threadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	dllLoadThread = std::thread([this]() { RunDLLLoaderThread(); });
}

void DLLManager::StopThread()
{
	threadStop = true;
	SetEvent(threadEvent);
	dllLoadThread.join();
}

bool DLLManager::FirstLoadDLL(const DLLType dllType, const std::string& dllPath)
{
	{
		std::shared_lock lock(dllHandlesMutex);
		if (dllHandles.find(dllType) != dllHandles.end())
		{
			return false;
		}
	}

	return LoadDLL(dllType, dllPath);
}

bool DLLManager::LoadDLL(const DLLType dllType, const std::string& dllPath)
{
	{
		std::unique_lock lock(dllHandlesMutex);
		auto itor = dllHandles.try_emplace(dllType, DLLInfo{ dllPath }).first;
		if (itor == dllHandles.end())
		{
			return false;
		}

		return itor->second.TryLoadLibrary();
	}
}

void DLLManager::LoadDLLAsync(const DLLType dllType, const std::string& dllPath)
{
	{
		std::unique_lock lock(dllLoadListLock);
		dllLoadList.push_back(std::make_pair(dllType, dllPath));
	}

	SetEvent(threadEvent);
}

void DLLManager::UnloadDLL(const DLLType dllType)
{
	auto itor = dllHandles.find(dllType);
	if (itor == dllHandles.end())
	{
		return;
	}

	itor->second.FreeLoadedLibrary();

	{
		std::unique_lock lock(dllHandlesMutex);
		dllHandles.erase(itor);
	}
}

void DLLManager::RunDLLLoaderThread()
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

			for (const auto& [dllType, dllPath] : dllLoadListCopy)
			{
				UnloadDLL(dllType);
				if (not LoadDLL(dllType, dllPath))
				{
					std::cout << "RunDLLLoaderThread() failed to load DLL with type " << static_cast<int>(dllType) << " and path " << dllPath << std::endl;
					continue;
				}
			}
		}
		else
		{
			std::cout << "RunDLLLoaderThread() failed to wait for thread event with " << GetLastError() << std::endl;
			break;
		}
	}
}
