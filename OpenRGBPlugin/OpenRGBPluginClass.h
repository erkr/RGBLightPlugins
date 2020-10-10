#pragma once
#include "CoreTempPlugin.h"
#include "OpenRGBSettings.h"


class OpenRGBPluginClass
{
public:
	OpenRGBPluginClass(void);
	virtual ~OpenRGBPluginClass(void);

	int Start();
	void Update(const LPCoreTempSharedData data);
	void Stop();
	int Configure();
	void Remove(const wchar_t *path);

	LPCoreTempPlugin GetPluginInstance(HMODULE hModule);

protected:

	OpenRGBSettings Settings;
	CoreTempPlugin m_CoreTempPlugin;
	void configurePlugin();
};

