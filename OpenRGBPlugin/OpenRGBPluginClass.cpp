#include "NetworkClient.h"
//#include "RGBController.h"
//#include "RGBController_Network.h"

#include <Windows.h>
#include "OpenRGBPluginClass.h"
#include "mapping.h"
#include "LaunchApp.h"


using namespace std;
std::vector<RGBController*> control;
NetworkClient* RGBClient = NULL;

RGBColor hsv2rgb(int H, double S, double V)
{
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	// ERKR added full range for H, so e.g. -40 -> 320, 400 -> 40 etc
	H %= 360;
	if (H < 0) {
		H += 360;
	}

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}
	RGBColor RGB = ToRGBColor((DWORD)((Rs + m) * 255), (DWORD)((Gs + m) * 255), (DWORD)((Bs + m) * 255));
	return RGB;
}

OpenRGBPluginClass::OpenRGBPluginClass(void) 
{
	this->configurePlugin();
}

OpenRGBPluginClass::~OpenRGBPluginClass(void)
{
	delete m_CoreTempPlugin.pluginInfo;
}

int OpenRGBPluginClass::Start()
{
	// Return 0 for success, non-zero for failure.
	Settings.ReadSettings();
	RGBClient = new NetworkClient(control);
	RGBClient->SetName("CoreTempClientPlugin");
	RGBClient->SetIP(Settings.OpenRGBIP);
	RGBClient->SetPortW(Settings.OpenRGBPort);
	RGBClient->StartClient();
	EL.Initialize(L"OpenRGBPlugin");
	EL.WriteInfo(L"OpenRGBPlugin is Started");
	return 0;
}

void OpenRGBPluginClass::Update(const LPCoreTempSharedData data)
{
	try {
		if (data != NULL)
		{
			if (RGBClient->GetOnline())
			{
				// connected
				if (!wasConnected)
				{
					wasConnected = true;
					EL.WriteInfo(L"OpenRGBPlugin Succesfully Connected OpenRGB Server");
				}
				cHSV hsv = ProcessCoreTempData(data, Settings);
				RGBColor cRGB = hsv2rgb(hsv.H, hsv.S, hsv.V);
				RGBClient->ControllerListMutex.lock();
				if (control.size() > Settings.ControlIdx)
				{
					if (control[Settings.ControlIdx]->GetMode() != Settings.OpenRGBMode) {
						control[Settings.ControlIdx]->SetMode(Settings.OpenRGBMode);
					}
					control[Settings.ControlIdx]->SetAllLEDs(cRGB);
					control[Settings.ControlIdx]->UpdateLEDs();
				}
				RGBClient->ControllerListMutex.unlock();
			}
			else // NOT connected
			{ 
				if (wasConnected)
				{
					wasConnected = false;
					EL.WriteWarning(L"OpenRGBPlugin lost connection with OpenRGB Server");
				}
			}
		}
	}
	catch (...) {
		if (Settings.doBeepForExceptions != 0) {
			Beep(1000, 10); // unexpected
		}
		EL.WriteError(L"OpenRGBPlugin: Exception occured while using the OpenRGB SDK");
	}
}

void OpenRGBPluginClass::Stop()
{
	RGBClient->StopClient();
	delete RGBClient;
	RGBClient = NULL;
	EL.WriteInfo(L"OpenRGBPlugin is Stopped");
}

int OpenRGBPluginClass::Configure()
{
	// Return 0 for not-implemented, non-zero for "Handled".
	Settings.SaveSettings();
	CString CommandLine = L"notepad.exe " + GetFullINIFilePath();
	if (launchApplicationAndWait((LPWSTR)(LPCWSTR)CommandLine) == 0) // success
	{
		Settings.ReadSettings();
		EL.WriteInfo(L"OpenRGBPlugin Configured");
	}
	return 1;
}

void OpenRGBPluginClass::Remove(const wchar_t *path)
{
}

LPCoreTempPlugin OpenRGBPluginClass::GetPluginInstance(HMODULE hModule)
{
	return &this->m_CoreTempPlugin;
}

void OpenRGBPluginClass::configurePlugin()
{
	LPCoreTempPluginInfo pluginInfo = new CoreTempPluginInfo;
	
	m_CoreTempPlugin.pluginInfo = pluginInfo;
	pluginInfo->name = L"OpenRGB Light Plugin";
	pluginInfo->description = L"Controls the MainBoard LED's based on CPU temp and CPU load";
	pluginInfo->version = L"1.0";
	
	// Interface version should be 1 for current plugin API.
	m_CoreTempPlugin.interfaceVersion = 1;
	m_CoreTempPlugin.type = General_Type;
}

