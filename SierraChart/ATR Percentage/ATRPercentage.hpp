#pragma once

#include "pch.h"

SCSFExport scsf_ATRPercentage(SCStudyGraphRef sc)
{
	auto& ATRSubgraph = sc.Subgraph[0];
	auto& ATRPercentSubgraph = sc.Subgraph[1];
	auto& ATRLengthInput = sc.Input[0];
	auto& ATRTypeInput = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "ATR Percentage";

		sc.AutoLoop = 1;

		ATRLengthInput.Name = "ATR Length";
		ATRLengthInput.SetInt(14);

		ATRTypeInput.Name = "ATR Type";
		ATRTypeInput.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		ATRSubgraph.Name = "ATR";
		ATRSubgraph.DrawStyle = DRAWSTYLE_IGNORE;

		ATRPercentSubgraph.Name = "ATR Percentage";
		ATRPercentSubgraph.DrawStyle = DRAWSTYLE_LINE;
	}

	auto Length = sc.Input[0].GetInt();
	auto MovAvgType = sc.Input[1].GetMovAvgType();

	sc.ATR(sc.BaseData, ATRSubgraph, Length, MovAvgType);

	auto& CurrentBarHigh = sc.BaseData[SC_HIGH][sc.Index];
	auto& CurrentBarLow = sc.BaseData[SC_LOW][sc.Index];
	auto CurrentBarRange = CurrentBarHigh - CurrentBarLow;

	ATRPercentSubgraph[sc.Index] = CurrentBarRange / ATRSubgraph[sc.Index];
}