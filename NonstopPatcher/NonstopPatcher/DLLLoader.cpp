#include "DLLLoader.h"
#include <iostream>

DLLLoader& DLLLoader::GetInst()
{
	static DLLLoader instance;
	return instance;
}

void DLLLoader::StopThread()
{
	threadStop = true;
	dllLoadThread.join();
}

bool DLLLoader::LoadDLL(const DLLType dllType, const std::string& dllPath)
{
	{
		std::shared_lock lock(dllHandlesMutex);
		if (dllHandles.find(dllType) != dllHandles.end())
		{
			return false;
		}
	}

	{
		std::unique_lock lock(dllHandlesMutex);
		return (dllHandles.try_emplace(dllType, DLLInfo{ dllPath })).second;
	}
}

void DLLLoader::LoadDLLAsync(const DLLType dllType, const std::string& dllPath)
{
	{
		std::unique_lock lock(dllLoadListLock);
		dllLoadList.push_back(std::make_pair(dllType, dllPath));
	}

	SetEvent(threadEvent);
}

void DLLLoader::UnloadDLL(const DLLType dllType)
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

void DLLLoader::RunDLLLoaderThread()
{
	std::list<std::pair<DLLType, std::string>> dllLoadListCopy;
	while (not threadStop)
	{
		{
			std::unique_lock lock(dllLoadListLock);
			dllLoadListCopy = std::move(dllLoadList);
		}

		if (WaitForSingleObject(threadEvent, INFINITE) == WAIT_OBJECT_0)
		{
			for (const auto& [dllType, dllPath] : dllLoadListCopy)
			{
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
