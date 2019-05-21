#include "GlobalConfiguration.h"


std::shared_ptr<GlobalConfiguration> GlobalConfiguration::load()
{
	OutputDebugString(_T("Loading global configuration...\n"));
	auto hMap = OpenFileMapping(FILE_MAP_READ, FALSE, _T("Global\\FoxOverlay_Configuration"));
	if (!hMap)
	{
		return nullptr;
	}

	CloseHandle(hMap);

	return nullptr;
}
