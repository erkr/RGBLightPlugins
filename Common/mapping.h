#pragma once
int mapCPUTempOnHUE(int Temp, int TCold = 25, int TWarm = 55, int THot = 60, int TCritical = 85,
	int HUECold = 240 /*20=BLUE*/,
	int HRangeC2W = -240 /*60=RED*/,
	int HRangeW2H = 0, /*60-70=RED */
	int HRangeH2C = -20); /* above 80 is violet-red */

float mapCPULoadOnIntensity(int CPULoad, float IMin = 0.20, float IMax = 1.0);

typedef struct cHSV_ST
{
	int	H;
	float S;
	float V;
} cHSV;

cHSV ProcessCoreTempData(const LPCoreTempSharedData data, const PluginSettings Settings);
