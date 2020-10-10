#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include "CoreTempPlugin.h"
#include "OpenRGBPluginClass.h"

OpenRGBPluginClass *openRGBPluginClass = NULL;

int Start()
{
	return openRGBPluginClass->Start();
}

void Update(const LPCoreTempSharedData data)
{
	openRGBPluginClass->Update(data);
}

void Stop()
{
	openRGBPluginClass->Stop();
}

int Configure()
{
	return openRGBPluginClass->Configure();
}

void Remove(const wchar_t *path)
{
	openRGBPluginClass->Remove(path);
}

LPCoreTempPlugin WINAPI GetPlugin(HMODULE hModule)
{
	openRGBPluginClass = new OpenRGBPluginClass();
	LPCoreTempPlugin plugin = openRGBPluginClass->GetPluginInstance(hModule);
	plugin->Start = Start;
	plugin->Update = Update;
	plugin->Stop = Stop;
	plugin->Configure = Configure;
	plugin->Remove = Remove;
	return plugin;
}

void WINAPI ReleasePlugin()
{
	if (openRGBPluginClass)
	{
		delete openRGBPluginClass;
		openRGBPluginClass = NULL;
	}
}
