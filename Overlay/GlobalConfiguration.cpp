#include "GlobalConfiguration.h"
#include "WinMutex.h"
#include <mutex>
#include "scope_guard.hpp"
#include "MemoryBinaryReader.h"
#include <vector>
#include "json11.hpp"

using namespace json11;

// Make sure to be consistent with constants in GlobalConfiguration.cs file
static const LPCTSTR SHARED_MEM_NAME = _T("Global\\FoxOverlay_Configuration");
static const LPCTSTR SHARED_MEM_MUTEX_NAME = _T("Global\\FoxOverlay_Configuration_Mutex");

static const ULONGLONG VERSION = 1ULL;

GlobalConfiguration::GlobalConfiguration(const UINT32 ipcPort)
	: ipcPort(ipcPort)
{
}

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
	if (ullVersion != VERSION)
	{
		OutputDebugString(_T("Wrong global configuration version\n"));

		return nullptr;
	}

	// Read the size
	reader.resize(16);
	const auto ullContentSize = reader.read_le<ULONGLONG>();

	// Read the configuration content
	reader.resize(16 + ullContentSize);
	std::vector<CHAR> vTmpBuffer(ullContentSize);
	reader.read(vTmpBuffer.data(), ullContentSize);

	// Load the content as std::string
	std::string strInUtf8(vTmpBuffer.data(), ullContentSize);

	// Parse Json
	std::string err;
	const auto json = Json::parse(strInUtf8, err);
	if (!err.empty() || json.is_null())
	{
		OutputDebugString(_T("Unable to parse the global configuration: "));
		OutputDebugStringA(err.c_str());
		OutputDebugString(_T("\n"));

		return nullptr;
	}

	// TODO: handle error such as property missing
	UINT32 ipcPort = json["IpcPort"].int_value();

	return std::make_shared<GlobalConfiguration>(ipcPort);
}
