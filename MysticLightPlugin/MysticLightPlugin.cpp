#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include "CoreTempPlugin.h"
#include "MysticLightPluginClass.h"

MysticLightPluginClass *mysticLightPluginClass = NULL;

int Start()
{
	return mysticLightPluginClass->Start();
}

void Update(const LPCoreTempSharedData data)
{
	mysticLightPluginClass->Update(data);
}

void Stop()
{
	mysticLightPluginClass->Stop();
}

int Configure()
{
	return mysticLightPluginClass->Configure();
}

void Remove(const wchar_t *path)
{
	mysticLightPluginClass->Remove(path);
}

LPCoreTempPlugin WINAPI GetPlugin(HMODULE hModule)
{
	mysticLightPluginClass = new MysticLightPluginClass();
	LPCoreTempPlugin plugin = mysticLightPluginClass->GetPluginInstance(hModule);
	plugin->Start = Start;
	plugin->Update = Update;
	plugin->Stop = Stop;
	plugin->Configure = Configure;
	plugin->Remove = Remove;
	return plugin;
}

void WINAPI ReleasePlugin()
{
	if (mysticLightPluginClass)
	{
		delete mysticLightPluginClass;
		mysticLightPluginClass = NULL;
	}


}

