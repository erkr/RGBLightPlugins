//#include "pch.h"
#include "CMysticLightCtrl.h"
#define MLAPI_OK                  0  // Request is completed.
#define MLAPI_ERROR              -1  // Generic error.
#define MLAPI_TIMEOUT            -2  // Request is timeout.
#define MLAPI_NO_IMPLEMENTED     -3  // MSI application not found or installed version not supported.
#define MLAPI_NOT_INITIALIZED    -4  // MLAPI_Initialize has not been called successful.
#define MLAPI_INVALID_ARGUMENT -101  // The parameter value is not valid.
#define MLAPI_DEVICE_NOT_FOUND -102  // The device is not found.
#define MLAPI_NOT_SUPPORTED    -103  // Requested feature is not supported in the selected LED

#include <Windows.h>
#include <math.h>

HINSTANCE Load_MSI_SDK_Dll() // don't put this method in a class as GetModuleHandleEx will fail then
{
	WCHAR thisDllDirPath[MAX_PATH];
	HMODULE hModule = NULL;
	//retrieve the module handle where this function is running in: EXE or DLL
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPWSTR)&Load_MSI_SDK_Dll, &hModule) == 0)
	{
		return NULL;
	}
	GetModuleFileName(hModule, thisDllDirPath, sizeof(thisDllDirPath) - 1);
	PathRemoveFileSpecW(thisDllDirPath);
	CString dllName(thisDllDirPath);
#ifdef _M_X64 
	dllName += "\\MysticLight_SDK_x64.dll";
#else
	dllName += "\\MysticLight_SDK.dll";
#endif
	//Beep(1000, 10);

	return LoadLibrary(dllName);
}


/*
  Two generic helper macros to convert HSV to RGB or back
*/
#define min_f(a, b, c)  (fminf(a, fminf(b, c)))
#define max_f(a, b, c)  (fmaxf(a, fmaxf(b, c)))

/*
 rgb2hsv is Based on the example of GitHub author yoggy. Source: https://gist.github.com/yoggy/8999625
 in: 
   RGB: 0-255
 out: 
   H: 0-360
   S: 0-1.0
   V: 0-1.0
*/
// void rgb2hsv(const unsigned char &src_g, const unsigned char &src_b, unsigned char &dst_h, unsigned char &dst_s, unsigned char &dst_v)
void CMysticLightCtrl::rgb2hsv(DWORD R, DWORD G, DWORD B, int *H, double *S, double *V)
{
	float r = R / 255.0f;
	float g = G / 255.0f;
	float b = B / 255.0f;

	float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

	float max = max_f(r, g, b);
	float min = min_f(r, g, b);

	v = max;

	if (max == 0.0f) {
		s = 0;
		h = 0;
	}
	else if (max - min == 0.0f) {
		s = 0;
		h = 0;
	}
	else {
		s = (max - min) / max;

		if (max == r) {
			h = 60 * ((g - b) / (max - min)) + 0;
		}
		else if (max == g) {
			h = 60 * ((b - r) / (max - min)) + 120;
		}
		else {
			h = 60 * ((r - g) / (max - min)) + 240;
		}
	}

	if (h < 0) h += 360.0f;

	*H = (int)(h); // 0-360
	*S = s; // 0.0-1.0
	*V = v; // 0.0-1.0
}

/* HSV2RGB code is inspired by hsvtorgb.cpp from Kuat Hadianto on GitHub: https://gist.github.com/kuathadianto/200148f53616cbd226d993b400214a7f
 * H(Hue): 0 - 360 degree (integer); other angles (even when negative) will be mapped on the 0-360 range
 * S(Saturation): 0 - 1.00 (double)
 * V(Value): 0 - 1.00 (double)
 * output: R,G,B: 0-255
 */
 
void CMysticLightCtrl::hsv2rgb(int H, double S, double V, DWORD *R, DWORD *G, DWORD *B)
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
	*R = (DWORD)((Rs + m) * 255);
	*G = (DWORD)((Gs + m) * 255);
	*B = (DWORD)((Bs + m) * 255);
}

void  CMysticLightCtrl::SetLastError(CString msg, int status) 
{
	BSTR errStr;
	_GetErrorMessage(status, &errStr);
	lastError = msg+errStr;
	::SysFreeString(errStr);
}

void  CMysticLightCtrl::SetLastError(CString msg)
{
	lastError = msg;
}

CString CMysticLightCtrl::GetLastError()
{
	return lastError;
}

int CMysticLightCtrl::NrOfLEDS() {
	return nrOfLEDs;
}



bool CMysticLightCtrl::Connect(unsigned int deviceIdx)
{

//Beep(1500, 10);
	if (MLinstance != NULL)
	{
		SetLastError(L"Start already called. ");
		return false;
	}

	MLinstance = Load_MSI_SDK_Dll();
	if (MLinstance == NULL)
	{
#ifdef _M_X64 
		SetLastError(L"MysticLight_SDK_x64.dll not found. "); 
#else
		SetLastError(L"MysticLight_SDK.dll not found. ");
#endif
		return false;
	}
//Beep(1500, 10);
	//initialise the method fields with the methods offerd by the SDK
	_Initialize = (LPMLAPI_Initialize)GetProcAddress(MLinstance, "MLAPI_Initialize");
	_GetErrorMessage = (LPMLAPI_GetErrorMessage)GetProcAddress(MLinstance, "MLAPI_GetErrorMessage");
	_GetDeviceInfo = (LPMLAPI_GetDeviceInfo)GetProcAddress(MLinstance, "MLAPI_GetDeviceInfo");
	_GetLedInfo = (LPMLAPI_GetLedInfo)GetProcAddress(MLinstance, "MLAPI_GetLedInfo");
	_GetLedColor = (LPMLAPI_GetLedColor)GetProcAddress(MLinstance, "MLAPI_GetLedColor");
	_GetLedStyle = (LPMLAPI_GetLedStyle)GetProcAddress(MLinstance, "MLAPI_GetLedStyle");
	_GetLedMaxBright = (LPMLAPI_GetLedMaxBright)GetProcAddress(MLinstance, "MLAPI_GetLedMaxBright");
	_GetLedBright = (LPMLAPI_GetLedBright)GetProcAddress(MLinstance, "MLAPI_GetLedBright");
	_GetLedMaxSpeed = (LPMLAPI_GetLedMaxSpeed)GetProcAddress(MLinstance, "MLAPI_GetLedMaxSpeed");
	_GetLedSpeed = (LPMLAPI_GetLedSpeed)GetProcAddress(MLinstance, "MLAPI_GetLedSpeed");
	_SetLedColor = (LPMLAPI_SetLedColor)GetProcAddress(MLinstance, "MLAPI_SetLedColor");
	_SetLedStyle = (LPMLAPI_SetLedStyle)GetProcAddress(MLinstance, "MLAPI_SetLedStyle");
	_SetLedBright = (LPMLAPI_SetLedBright)GetProcAddress(MLinstance, "MLAPI_SetLedBright");
	_SetLedSpeed = (LPMLAPI_SetLedSpeed)GetProcAddress(MLinstance, "MLAPI_SetLedSpeed");

	int status = _Initialize();		//initialise the sdk
	if (status == MLAPI_OK)
	{	
		SAFEARRAY *psad = NULL, *psal = NULL;
		status = _GetDeviceInfo(&(psad), &(psal)); 
		if (status == MLAPI_OK)
		{
//Beep(1000, 10);
			devices.Attach(psad);
			ledCount.Attach(psal);
			if (devices.GetCount() > deviceIdx && ledCount.GetCount() > 0)
			{
				deviceName = devices.GetAt(deviceIdx);
				nrOfLEDs = _wtol(ledCount.GetAt(deviceIdx));
				return true;
			}
			else
			{
				SetLastError(L"No MSI Compatible Devices or LEDs found: ");
			}
		}
		else
		{
			SetLastError(L"SDK call GetDeviceInfo failed: ",status);
#if 0
			// Work around for buggy GetDeviceInfo; use defauls
			deviceName = ::SysAllocString(L"MSI_MB");
			nrOfLEDs = 1;
			return true;
#endif
		}
	}
	else
	{
		SetLastError(L"SDK call Initialize failed: ", status);
	}
	FreeLibrary(MLinstance);
	MLinstance = NULL;
	return false;
}

void CMysticLightCtrl::Disconnect()
{
	if (MLinstance != NULL) 
	{
		SafeArrayDestroy(devices);
		devices.m_psa = NULL;
		SafeArrayDestroy(ledCount);
		ledCount.m_psa = NULL;
		::SysFreeString(deviceName);
		FreeLibrary(MLinstance);
		MLinstance = NULL;
	}
}

bool CMysticLightCtrl::SetLedStyle(CString Style, int LedIdx)
{
	if (MLinstance == NULL)
	{
		SetLastError(L"SDK not connected. ");
		return false;
	}
	BSTR newStyle = ::SysAllocString(Style);
	int status = _SetLedStyle(deviceName, LedIdx, newStyle);
	::SysFreeString(newStyle);
	if (status != MLAPI_OK)
	{
		SetLastError(L"SDK call SetLedStyle failed: ", status);
		return false;
	}
	return true;
}


bool CMysticLightCtrl::SetLedRGB(DWORD R, DWORD G, DWORD B, int LedIdx)
{
	if (MLinstance == NULL)
	{
		SetLastError(L"SDK not connected. ");
		return false;
	}
	range255(R); range255(G); range255(B);
	int status = _SetLedColor(deviceName, LedIdx, R, G, B);
	if (status != MLAPI_OK)
	{
		SetLastError(L"SDK call SetLedColor failed: ", status);
		return false;
	}
	return true;
}

bool CMysticLightCtrl::GetLedRGB(DWORD *R, DWORD *G, DWORD *B, int LedIdx)
{
	if (MLinstance == NULL)
	{
		SetLastError(L"SDK not connected. ");
		return false;
	}
	int status = _GetLedColor(deviceName, LedIdx, R, G, B);
	if (status != MLAPI_OK)
	{
		SetLastError(L"SDK call GetLedColor failed: ", status);
		return false;
	}
	return true;
}

bool CMysticLightCtrl::SetLedHSV(int H, double S, double V, int LedIdx)
{
	if (MLinstance == NULL)
	{
		SetLastError(L"SDK not connected. ");
		return false;
    }
	DWORD R, G, B;
	range1(S); range1(V);
	hsv2rgb(H, S, V, &R, &G, &B);
	return SetLedRGB(R, G, B, LedIdx);
}

bool CMysticLightCtrl::GetLedHSV(int *H, double *S, double *V, int LedIdx)
{
	if (MLinstance == NULL)
	{
		SetLastError(L"SDK not connected. ");
		return false;
	}
	DWORD R, G, B;
	if (GetLedRGB(&R, &G, &B, LedIdx))
	{
		rgb2hsv(R, G, B, H, S, V);
		return true;
	}
	return false;
}


