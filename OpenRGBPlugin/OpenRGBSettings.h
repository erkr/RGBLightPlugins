#pragma once
#include "Settings.h"
#include "NetworkProtocol.h"
class OpenRGBSettings : public PluginSettings
{
public:
	virtual void SaveSettings();
	virtual bool ReadSettings();

	char OpenRGBIP[17] = "127.0.0.1";
	int  OpenRGBPort = OPENRGB_SDK_PORT;
	int  OpenRGBMode = 0;
};

