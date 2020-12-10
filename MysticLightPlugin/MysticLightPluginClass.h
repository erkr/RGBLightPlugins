#pragma once
#include "CoreTempPlugin.h"
#include "CMysticLightCtrl.h"
#include "Settings.h"
#include "EventLog.h"

class MysticLightPluginClass
{
public:
	MysticLightPluginClass(void);
	virtual ~MysticLightPluginClass(void);

	int Start();
	void Update(const LPCoreTempSharedData data);
	void Stop();
	int Configure();
	void Remove(const wchar_t *path);

	LPCoreTempPlugin GetPluginInstance(HMODULE hModule);

protected:
	EventLog EL;
	CMysticLightCtrl ML;
	PluginSettings Settings;
	CoreTempPlugin m_CoreTempPlugin;
	void configurePlugin();
};

