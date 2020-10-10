#pragma once
#include "CoreTempPlugin.h"
#include "CMysticLightCtrl.h"
#include "Settings.h"

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
	CMysticLightCtrl ML;
	PluginSettings Settings;
	CoreTempPlugin m_CoreTempPlugin;
	void configurePlugin();
};

