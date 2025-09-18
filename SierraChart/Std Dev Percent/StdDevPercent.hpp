#pragma once
#include "pch.h"
#include "Math/Sum.hpp"

SCSFExport scsf_StdPercent(SCStudyGraphRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Standard Deviation Percent";
		sc.AutoLoop = true;
		sc.FreeDLL = true;

		sc.Input[0].Name = "Length";
		sc.Input[0].SetInt(252);

		sc.Subgraph[0].Name = "Standard Deviation Percent";
	}

	if (!sc.Index) return;

	sc.Subgraph[0].Arrays[0][sc.Index] = sc.BaseData[SC_LAST][sc.Index] - sc.BaseData[SC_LAST][sc.Index - 1];

	/* Daily percent change */
	sc.Subgraph[0].Arrays[1][sc.Index] = sc.Subgraph[0].Arrays[0][sc.Index] / sc.BaseData[SC_LAST][sc.Index - 1];

	/* Squared daily percent change */
	sc.Subgraph[0].Arrays[2][sc.Index] = sc.Subgraph[0].Arrays[1][sc.Index] * sc.Subgraph[0].Arrays[1][sc.Index];

	if (sc.Index <= sc.Input[0].GetInt()) return;

	float sum = Sum(sc.Index, sc.Subgraph[0].Arrays[2], sc.Input[0].GetInt());
	sum /= sc.Input[0].GetInt() - 1;

	sc.Subgraph[0][sc.Index] = sqrt(sum) * 100.f;
}