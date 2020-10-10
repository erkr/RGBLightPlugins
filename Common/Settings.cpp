#include <Windows.h>
#include <WinBase.h>
#include <atlstr.h>
#include "settings.h"



CString GetFullINIFilePath()
{
	WCHAR thisModulePath[MAX_PATH];
	HMODULE hModule = NULL;
	//retrieve the module handle where this function is running in: EXE or DLL
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPWSTR)&GetFullINIFilePath, &hModule) == 0)
	{
		return L"";
	}
	GetModuleFileName(hModule, thisModulePath, sizeof(thisModulePath) - 1);
	PathRenameExtension(thisModulePath, L".ini");
	CString Result(thisModulePath);
	return Result;
}

void PluginSettings::SaveSettings()
{
	CString FullIniName = GetFullINIFilePath();
	CString stringValue;

	// General
	stringValue.Format(L"%d", ControlIdx);
	WritePrivateProfileString(L"General_Settings", L"ControlIdx", stringValue, FullIniName);
	// Core Temp mappings
	stringValue.Format(L"%d", TCold);
	WritePrivateProfileString(L"Core_Temp_Mappings", L"TColdBelow", stringValue, FullIniName);
	stringValue.Format(L"%d", TWarm);
	WritePrivateProfileString(L"Core_Temp_Mappings", L"TWarm", stringValue, FullIniName);
	stringValue.Format(L"%d", THot);
	WritePrivateProfileString(L"Core_Temp_Mappings", L"THot", stringValue, FullIniName);
	stringValue.Format(L"%d", TCritical);
	WritePrivateProfileString(L"Core_Temp_Mappings", L"TCriticalAbove", stringValue, FullIniName);
	// Core Load mapping 
	stringValue.Format(L"%d", LoadBasedIntensity);
	WritePrivateProfileString(L"Core_Load_Mappings", L"LoadBasedIntensity", stringValue, FullIniName);
	stringValue.Format(L"%d", ILowPct);
	WritePrivateProfileString(L"Core_Load_Mappings", L"ILowPct", stringValue, FullIniName);
	stringValue.Format(L"%d", IHighPct);
	WritePrivateProfileString(L"Core_Load_Mappings", L"IHighPct", stringValue, FullIniName);
	// color mappings HUE 360 degrees circle (0=RED, 120=Green, 240=Blue)
	stringValue.Format(L"%d", HUECold);
	WritePrivateProfileString(L"Color_HUE360_Mappings", L"HUECold", stringValue, FullIniName);
	stringValue.Format(L"%d", HRangeC2W);
	WritePrivateProfileString(L"Color_HUE360_Mappings", L"HRangeCold2Warm", stringValue, FullIniName);
	stringValue.Format(L"%d", HRangeW2H);
	WritePrivateProfileString(L"Color_HUE360_Mappings", L"HRangeWarm2Hot", stringValue, FullIniName);
	stringValue.Format(L"%d", HRangeH2C);
	WritePrivateProfileString(L"Color_HUE360_Mappings", L"HRangeHot2Critical", stringValue, FullIniName);
}

bool PluginSettings::ReadSettings()
{
	CString FullIniName = GetFullINIFilePath();
	if (!PathFileExists(FullIniName)) {
		SaveSettings(); // create default file
		return false;
	}
	// General
	ControlIdx = GetPrivateProfileInt(L"General_Settings", L"ControlIdx", ControlIdx, FullIniName);
	// Core Temp mappings
	TCold     = GetPrivateProfileInt(L"Core_Temp_Mappings", L"TColdBelow", TCold, FullIniName);
	TWarm     = GetPrivateProfileInt(L"Core_Temp_Mappings", L"TWarm", TWarm, FullIniName);
	THot      = GetPrivateProfileInt(L"Core_Temp_Mappings", L"THot", THot, FullIniName);
	TCritical = GetPrivateProfileInt(L"Core_Temp_Mappings", L"TCriticalAbove", TCritical, FullIniName);
	// Core Load mapping
	LoadBasedIntensity = GetPrivateProfileInt(L"Core_Load_Mappings", L"LoadBasedIntensity", LoadBasedIntensity, FullIniName);
	ILowPct            = GetPrivateProfileInt(L"Core_Load_Mappings", L"ILowPct", ILowPct, FullIniName);
	IHighPct           = GetPrivateProfileInt(L"Core_Load_Mappings", L"IHighPct", IHighPct, FullIniName);
	// color mappings HUE 360 degrees circle (0=RED, 120=Green, 240=Blue)
	HUECold   = GetPrivateProfileInt(L"Color_HUE360_Mappings", L"HUECold", HUECold, FullIniName);
	HRangeC2W = GetPrivateProfileInt(L"Color_HUE360_Mappings", L"HRangeCold2Warm", HRangeC2W, FullIniName);
	HRangeW2H = GetPrivateProfileInt(L"Color_HUE360_Mappings", L"HRangeWarm2Hot", HRangeW2H, FullIniName);
	HRangeH2C = GetPrivateProfileInt(L"Color_HUE360_Mappings", L"HRangeHot2Critical", HRangeH2C, FullIniName);

	return true;
}
