#pragma once

#include "pch.h"

SCSFExport scsf_ATRPercentage(SCStudyGraphRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "ATR Percentage";

		sc.AutoLoop = 1;

		sc.FreeDLL = 1;

		sc.Input[0].Name = "Length";
		sc.Input[0].SetInt(14);

		sc.Subgraph[0].Name = "ATR";
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[1].Name = "ATR Percentage";
	}

	auto Length = sc.Input[0].GetInt();

	sc.ATR(sc.BaseData, sc.Subgraph[0], Length, MOVAVGTYPE_SIMPLE);

	sc.Subgraph[1][sc.Index] = (sc.BaseData[SC_HIGH][sc.Index] - sc.BaseData[SC_LOW][sc.Index]) / sc.Subgraph[0][sc.Index];
}