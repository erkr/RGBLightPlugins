// Routine to map Temperature on a HUE value
#include "CoreTempPlugin.h"
#include "Settings.h"	
#include "mapping.h"	

int mapCPUTempOnHUE(int Temp, int TCold, int TWarm, int THot, int TCritical,
	int HUECold     /*20=BLUE*/,
	int HRangeC2W   /*60=RED*/,
	int HRangeW2H   /*60-70=RED */,
	int HRangeH2C ) /* above 80 is violet-red */
{
	// Temps below TCold will set HUECold, and Temps above TCritical will return HUE for TCritical.
	// Three temp ranges, to support differnet slopes. These ranges are expected to be continues
	// HRanges can be positive or negative to determine the direction starting from HUECold
	// The defaults for HUE vary from Blue (cold) to VioletRed (hot)
	// Returns: HUE based on 0-360 degrees
	int HUE = HUECold;
	if (Temp >= TCritical)
	{
		HUE = HUECold + HRangeC2W + HRangeW2H + HRangeH2C;
	}
	else if (Temp >= THot)
	{
		int TRange = TCritical - THot;
		HUE = HUECold + HRangeC2W + HRangeW2H + ((Temp - THot)*HRangeH2C) / TRange;
	}
	else if (Temp >= TWarm)
	{
		int TRange = THot - TWarm;
		HUE = HUECold + HRangeC2W + ((Temp - TWarm)*HRangeW2H) / TRange;
	}
	else if (Temp >= TCold)
	{
		int TRange = TWarm - TCold;
		HUE = HUECold + ((Temp - TCold)*HRangeC2W) / TRange;
	}

	// assure the returned HUE is mapped in the 0-360 range
	HUE %= 360;
	if (HUE < 0)
		HUE += 360;

	return HUE;
}

float mapCPULoadOnIntensity(int CPULoad, float IMin, float IMax)
{
	// Returns an Intensity that scales with the CPULoad (0-100) between IMin and IMax
	return IMin + ((IMax - IMin) * CPULoad) / 100;
}

cHSV ProcessCoreTempData(const LPCoreTempSharedData data, const PluginSettings Settings)
{
	unsigned int AvgLoad = 0;
	double AvgTemp = 0;
	cHSV Result = { 0, 1.0, 1.0 };
	for (unsigned int i = 0; i < data->uiCoreCnt; i++)
	{
		AvgLoad += data->uiLoad[i];
		AvgTemp += data->fTemp[i];
	}
	AvgLoad /= data->uiCoreCnt;
	AvgTemp /= data->uiCoreCnt;
	Result.H = mapCPUTempOnHUE((int)AvgTemp, Settings.TCold, Settings.TWarm, Settings.THot, Settings.TCritical,
		Settings.HUECold, Settings.HRangeC2W, Settings.HRangeW2H, Settings.HRangeH2C);
	if (Settings.LoadBasedIntensity != 0)
	{
		Result.V = mapCPULoadOnIntensity(AvgLoad, (float)(Settings.ILowPct) / 100.0F, (float)(Settings.IHighPct) / 100.0F);
	}
	return Result;
}