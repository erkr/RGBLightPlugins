//#include <iostream>
#include <Windows.h>
#include "MysticLightPluginClass.h"
#include "mapping.h"
#include "LaunchApp.h"
#include "Settings.h"

using namespace std;

MysticLightPluginClass::MysticLightPluginClass(void) 
{
	this->configurePlugin();
}

MysticLightPluginClass::~MysticLightPluginClass(void)
{
	delete m_CoreTempPlugin.pluginInfo;
}

int MysticLightPluginClass::Start()
{
	// Return 0 for success, non-zero for failure.
	Settings.ReadSettings();
	EL.Initialize(L"MysticLightPlugin");
	EL.WriteInfo(L"MysticLightPlugin is Started");
	return 0;
}

void MysticLightPluginClass::Update(const LPCoreTempSharedData data)
{
	try {
		if (ML.IsConnected() == false)
		{
			if (ML.Connect(Settings.ControlIdx)) { // give it a try
			    // switch to Steady style that allow to change colors!
				for (int l = 0; l < ML.NrOfLEDS(); l++) {
					ML.SetLedStyle(L"Steady", l);
				}
				EL.WriteInfo(L"MysticLightPlugin Succesfully Connected the MSI SDK");
			}
		}
		else if (data != NULL)
		{
			cHSV hsv = ProcessCoreTempData(data, Settings);
			for (int l = 0; l < ML.NrOfLEDS(); l++) {
				if (!ML.SetLedHSV(hsv.H, hsv.S, hsv.V, l)) {
					// re-apply Steady style that allow to change colors!
					ML.SetLedStyle(L"Steady", l);
					// Beep(1000, 10); // unexpected, unless other app changed the ledStyle
				}
			}
		}
	}
	catch (...) {
		if (Settings.doBeepForExceptions != 0) {
			Beep(1000, 10); // unexpected
		}
		EL.WriteError(L"MysticLightPlugin: Exception occured while using the Mysticlight SDK");
	}
}

void MysticLightPluginClass::Stop()
{
	ML.Disconnect();
	EL.WriteInfo(L"MysticLightPlugin is Stopped");
}

int MysticLightPluginClass::Configure()
{
	// Return 0 for not-implemented, non-zero for "Handled".
	Settings.SaveSettings();
	CString CommandLine = L"notepad.exe "+GetFullINIFilePath();
	if (launchApplicationAndWait((LPWSTR)(LPCWSTR)CommandLine) == 0) // success
	{
		Settings.ReadSettings();
		EL.WriteInfo(L"MysticLightPlugin Configured");
	}
	return 1; 
}

void MysticLightPluginClass::Remove(const wchar_t *path)
{
}

LPCoreTempPlugin MysticLightPluginClass::GetPluginInstance(HMODULE hModule)
{
	return &this->m_CoreTempPlugin;
}

void MysticLightPluginClass::configurePlugin()
{
	LPCoreTempPluginInfo pluginInfo = new CoreTempPluginInfo;
	
	m_CoreTempPlugin.pluginInfo = pluginInfo;
	pluginInfo->name = L"MSI Mystic Light Plugin";
	pluginInfo->description = L"Controls the MainBoard LED's based on CPU temp and CPU load";
	pluginInfo->version = L"1.1";
	
	// Interface version should be 1 for current plugin API.
	m_CoreTempPlugin.interfaceVersion = 1;
	m_CoreTempPlugin.type = General_Type;
}

