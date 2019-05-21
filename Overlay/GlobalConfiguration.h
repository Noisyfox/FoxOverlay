#pragma once

#include "framework.h"

class GlobalConfiguration
{
private:
	GlobalConfiguration();
public:
	static std::shared_ptr<GlobalConfiguration> load();
};
