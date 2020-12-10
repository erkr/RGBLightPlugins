#pragma once
#include "CoreTempPlugin.h"
#include "OpenRGBSettings.h"
#include "EventLog.h"


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
	bool wasConnected = false;
	EventLog EL;
	OpenRGBSettings Settings;
	CoreTempPlugin m_CoreTempPlugin;
	void configurePlugin();
};

