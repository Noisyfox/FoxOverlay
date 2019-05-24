#pragma once

#include "framework.h"

class GlobalConfiguration
{
public:
	const UINT32 ipcPort;

	explicit GlobalConfiguration(UINT32 ipcPort);

	static std::shared_ptr<GlobalConfiguration> load();
};
