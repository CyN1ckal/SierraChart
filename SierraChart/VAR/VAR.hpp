#pragma once
#include "pch.h"

struct RiskMetrics
{
	float VAR = 0.0f;
	float ES = 0.0f;
};

RiskMetrics GetRiskMetrics(SCStudyGraphRef sc, SCSubgraphRef Change, int Length, float Percentile)
{
	RiskMetrics rm;

	if (Percentile > 100.0f) return rm;

	std::vector<float> Changes;
	Changes.reserve(Length);

	for (int i = sc.Index; i > sc.Index - Length; i--)
		Changes.push_back(Change[i]);

	std::sort(Changes.begin(), Changes.end());

	if (Percentile == 100.0f)
	{
		rm.VAR = Changes[0];
		rm.ES = Changes[0];
		return rm;
	}

	float ItemWidth = 100.0f / static_cast<float>(Changes.size());

	float Intermediate = (100.0f - Percentile) / ItemWidth;

	uint32_t VARIndex = std::round(Intermediate);

	SCString DebugString = std::format("Var Index: {0:d}, Item Width {1:.4f}, Intermediate {2:f}", VARIndex, ItemWidth, Intermediate).c_str();

	sc.AddMessageToLog(DebugString, 1);

	rm.VAR = Changes[VARIndex];

	for(int i = 0; i < VARIndex; i++)
		rm.ES += Changes[i];

	rm.ES /= static_cast<float>(VARIndex);

	return rm;
}

SCSFExport scsf_RiskMetrics(SCStudyGraphRef sc)
{
	auto& VARLength = sc.Input[0];
	auto& VARPercentile = sc.Input[1];

	auto& DailyChange = sc.Subgraph[0];
	auto& VARSubgraph = sc.Subgraph[1];
	auto& ESSubgraph = sc.Subgraph[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Risk Metrics";

		sc.AutoLoop = 1;

		VARLength.Name = "Length";
		VARLength.SetInt(252);

		VARPercentile.Name = "Confidence Interval";
		VARPercentile.SetFloat(99.0f);

		DailyChange.Name = "Daily Change";

		VARSubgraph.Name = "VAR";

		ESSubgraph.Name = "ES";
	}

	DailyChange[sc.Index] = sc.BaseDataIn[SC_LAST][sc.Index] - sc.BaseDataIn[SC_LAST][sc.Index - 1];

	auto Length = VARLength.GetInt();
	auto Percentile = VARPercentile.GetFloat();

	if (sc.Index < Length) return;

	auto RiskMetrics = GetRiskMetrics(sc, DailyChange, Length, Percentile);

	VARSubgraph[sc.Index] = RiskMetrics.VAR;
	ESSubgraph[sc.Index] = RiskMetrics.ES;
}