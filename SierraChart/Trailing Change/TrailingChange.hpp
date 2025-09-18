#pragma once
#include "pch.h"

SCSFExport scsf_TrailingPercentageChange(SCStudyGraphRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Trailing Price Change";

		sc.AutoLoop = 1;

		sc.FreeDLL = 1;

		sc.Input[0].Name = "Length";
		sc.Input[0].SetInt(252);

		sc.Subgraph[0].Name = "Percentage Change";
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[0].PrimaryColor = RGB(128, 255, 128);
		sc.Subgraph[0].SecondaryColor = RGB(255, 128, 128);
		sc.Subgraph[0].AutoColoring = 2;

		return;
	}

	auto Length = sc.Input[0].GetInt();

	sc.Subgraph[0][sc.Index] = (sc.BaseData[SC_LAST][sc.Index] - sc.BaseData[SC_LAST][sc.Index - Length]) / sc.BaseData[SC_OPEN][sc.Index - Length];
}