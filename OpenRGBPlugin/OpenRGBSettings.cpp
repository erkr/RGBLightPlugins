#include "OpenRGBSettings.h"

void OpenRGBSettings::SaveSettings()
{
	CString FullIniName = GetFullINIFilePath();
	CString stringValue;

	
	// OpenRGB settings
	stringValue = OpenRGBIP;
	WritePrivateProfileString(L"OpenRGB_Settings", L"OpenRGBIP", stringValue, FullIniName);
	stringValue.Format(L"%d", OpenRGBPort);
	WritePrivateProfileString(L"OpenRGB_Settings", L"OpenRGBPort", stringValue, FullIniName);
	stringValue.Format(L"%d", OpenRGBMode);
	WritePrivateProfileString(L"OpenRGB_Settings", L"OpenRGBMode", stringValue, FullIniName);

	PluginSettings::SaveSettings();
}

bool OpenRGBSettings::ReadSettings()
{
	CString FullIniName = GetFullINIFilePath();
	if (!PathFileExists(FullIniName)) {
		SaveSettings(); // create default file
		return false;
	}

	CString defaultValue;
	WCHAR   resultString[50];
	// OpenRGB settings
	defaultValue = OpenRGBIP;
	if (0 < GetPrivateProfileString(L"OpenRGB_Settings", L"OpenRGBIP", defaultValue, resultString, sizeof(resultString) - 1, FullIniName)) {
		//wcstombs(OpenRGBIP, resultString, sizeof(OpenRGBIP));
		size_t   i;
		wcstombs_s(&i, OpenRGBIP, resultString, sizeof(OpenRGBIP));
	}
	OpenRGBPort = GetPrivateProfileInt(L"OpenRGB_Settings", L"OpenRGBPort", OpenRGBPort, FullIniName);
	OpenRGBMode = GetPrivateProfileInt(L"OpenRGB_Settings", L"OpenRGBMode", OpenRGBMode, FullIniName);

	return PluginSettings::ReadSettings();
}