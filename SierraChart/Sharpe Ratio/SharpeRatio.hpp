#pragma once
#include "pch.h"	
#include "Math/Sum.hpp"

enum ESharpRatioSubgraphs
{
	SHARPE_AVERAGE,
	SHARPE_ERROR,
	SHARPE_SQUARED_ERROR,
	SHARPE_SUM_SQUARED_ERROR,
	SHARPE_STANDARD_DEVIATION,
	SHARPE_RETURN,
	SHARPE_AVERAGE_DAILY_RETURN,
	SHARPE_RATIO
};

SCSFExport scsf_SharpeRatio(SCStudyGraphRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Sharpe Ratio";

		sc.AutoLoop = true;

		sc.FreeDLL = true;

		sc.Input[0].Name = "Length";
		sc.Input[0].SetInt(252);

		sc.Subgraph[SHARPE_AVERAGE].Name = "Average";
		sc.Subgraph[SHARPE_AVERAGE].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[SHARPE_ERROR].Name = "Error";
		sc.Subgraph[SHARPE_ERROR].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[SHARPE_SQUARED_ERROR].Name = "Squared Error";
		sc.Subgraph[SHARPE_SQUARED_ERROR].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[SHARPE_SUM_SQUARED_ERROR].Name = "Sum Squared Error";
		sc.Subgraph[SHARPE_SUM_SQUARED_ERROR].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[SHARPE_STANDARD_DEVIATION].Name = "Standard Deviation";
		sc.Subgraph[SHARPE_STANDARD_DEVIATION].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[SHARPE_RETURN].Name = "Total Return";
		sc.Subgraph[SHARPE_RETURN].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[SHARPE_AVERAGE_DAILY_RETURN].Name = "Average Daily Return";
		sc.Subgraph[SHARPE_AVERAGE_DAILY_RETURN].DrawStyle = DRAWSTYLE_IGNORE;

		sc.Subgraph[SHARPE_RATIO].Name = "Sharpe Ratio";
		sc.Subgraph[SHARPE_RATIO].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[SHARPE_RATIO].PrimaryColor = RGB(128, 255, 128);
		sc.Subgraph[SHARPE_RATIO].SecondaryColor = RGB(255, 128, 128);
		sc.Subgraph[SHARPE_RATIO].AutoColoring = 2;

		return;
	}

	auto Length = sc.Input[0].GetInt();

	sc.MovingAverage(sc.BaseData[SC_LAST], sc.Subgraph[0], MOVAVGTYPE_SIMPLE, Length);

	sc.Subgraph[SHARPE_ERROR][sc.Index] = (sc.BaseData[SC_LAST][sc.Index] - sc.Subgraph[SHARPE_AVERAGE][sc.Index]) / sc.BaseData[SC_LAST][sc.Index];

	sc.Subgraph[SHARPE_SQUARED_ERROR][sc.Index] = sc.Subgraph[SHARPE_ERROR][sc.Index] * sc.Subgraph[SHARPE_ERROR][sc.Index];

	if (sc.Index < Length) return;

	sc.Subgraph[SHARPE_SUM_SQUARED_ERROR][sc.Index] = Sum(sc.Index, sc.Subgraph[SHARPE_SQUARED_ERROR], Length);

	sc.Subgraph[SHARPE_STANDARD_DEVIATION][sc.Index] = sqrt(sc.Subgraph[SHARPE_SUM_SQUARED_ERROR][sc.Index] / (Length - 1));

	sc.Subgraph[SHARPE_RETURN][sc.Index] = sc.BaseData[SC_LAST][sc.Index] / sc.BaseData[SC_LAST][sc.Index - Length];

	sc.Subgraph[SHARPE_AVERAGE_DAILY_RETURN][sc.Index] = pow(sc.Subgraph[SHARPE_RETURN][sc.Index], 1.0f / Length) - 1.0f;

	sc.Subgraph[SHARPE_RATIO][sc.Index] = sc.Subgraph[SHARPE_AVERAGE_DAILY_RETURN][sc.Index] / sc.Subgraph[SHARPE_STANDARD_DEVIATION][sc.Index];
}