#pragma once
#include <windows.h>

//Plugin types
#define General_Type 1

//Errors
#define USER					0x20000000

typedef struct core_temp_shared_data
{
	unsigned int	uiLoad[256];
	unsigned int	uiTjMax[128];
	unsigned int	uiCoreCnt;
	unsigned int	uiCPUCnt;
	float			fTemp[256];
	float			fVID;
	float			fCPUSpeed;
	float			fFSBSpeed;
	float			fMultiplier;	
	char			sCPUName[100];
	unsigned char	ucFahrenheit;
	unsigned char	ucDeltaToTjMax;
}CoreTempSharedData,*LPCoreTempSharedData,**PPCoreTempSharedData;

typedef struct CoreTempPluginInfo CoreTempPluginInfo, *LPCoreTempPluginInfo;
typedef struct CoreTempPlugin CoreTempPlugin, *LPCoreTempPlugin;
typedef void (*RemotePluginStop)(LPCoreTempPlugin);
struct CoreTempPluginInfo
{
	wchar_t *version;
	wchar_t *name;
	wchar_t *description;
	HWND hwndParent;
	HINSTANCE dllInstance;
	RemotePluginStop remoteStopProc;
};

struct CoreTempPlugin
{
	int interfaceVersion;
	int type;
	LPCoreTempPluginInfo pluginInfo;
	
	// interfaceVersion = 1
	int (*Start)();
	void (*Update)(const LPCoreTempSharedData data);
	void (*Stop)();
	int (*Configure)();
	void (*Remove)(const wchar_t *path);
};

typedef LPCoreTempPlugin (WINAPI *fnGetPlugin)(HMODULE hModule);
typedef void (WINAPI *fnReleasePlugin)();