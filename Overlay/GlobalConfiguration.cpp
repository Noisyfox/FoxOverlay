#include "GlobalConfiguration.h"
#include "WinMutex.h"
#include <mutex>
#include "scope_guard.hpp"
#include "MemoryBinaryReader.h"
#include <vector>


// Make sure to be consistent with constants in GlobalConfiguration.cs file
static const LPCTSTR SHARED_MEM_NAME = _T("Global\\FoxOverlay_Configuration");
static const LPCTSTR SHARED_MEM_MUTEX_NAME = _T("Global\\FoxOverlay_Configuration_Mutex");

static const ULONGLONG VERSION = 1ULL;

std::shared_ptr<GlobalConfiguration> GlobalConfiguration::load()
{
	OutputDebugString(_T("Loading global configuration...\n"));

	// Acquire the mutex before open the shared memory
	WinMutex mutex(false, SHARED_MEM_MUTEX_NAME);
	std::lock_guard<WinMutex> lock(mutex);

	const auto hMap = OpenFileMapping(FILE_MAP_READ, FALSE, SHARED_MEM_NAME);
	if (!hMap)
	{
		return nullptr;
	}

	const auto pBuffer = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (!pBuffer)
	{
		CloseHandle(hMap);
		return nullptr;
	}

	// Cleanup when function returns
	auto cleanupGuard = sg::make_scope_guard([=]
	{
		UnmapViewOfFile(pBuffer);
		CloseHandle(hMap);
	});

	// Read the config
	MemoryBinaryReader reader(pBuffer, 8);

	// Check the version first
	const auto ullVersion = reader.read_le<ULONGLONG>();
	if(ullVersion != VERSION)
	{
		OutputDebugString(_T("Wrong global configuration version\n"));

		return nullptr;
	}

	// Read the size
	reader.resize(16);
	const auto ullContentSize = reader.read_le<ULONGLONG>();

	// Read the configuration content
	reader.resize(16 + ullContentSize);
	std::vector<CHAR> strContentInUtf8(ullContentSize);
	reader.read(strContentInUtf8.data(), ullContentSize);

	// TODO: utf-8 to unicode

	return nullptr;
}
