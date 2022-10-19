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

				int nTokenPos = 0;
				CString strToken = Settings.ControlIdx.Tokenize(_T(","), nTokenPos);

				while (!strToken.IsEmpty()) {
					int idx = _ttoi(strToken);

					if (control.size() > idx)
					{
						if (control[idx]->GetMode() != Settings.OpenRGBMode) {
							control[idx]->SetMode(Settings.OpenRGBMode);
						}
						control[idx]->SetAllLEDs(cRGB);
						control[idx]->UpdateLEDs();
					}

					strToken = Settings.ControlIdx.Tokenize(_T(","), nTokenPos);

					if (!strToken.IsEmpty()) {
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
					}
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
	pluginInfo->version = L"1.1";
	
	// Interface version should be 1 for current plugin API.
	m_CoreTempPlugin.interfaceVersion = 1;
	m_CoreTempPlugin.type = General_Type;
}

