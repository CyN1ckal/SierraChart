#pragma once
#include "pch.h"

struct RiskMetrics
{
	float VAR = 0.0f;
	float ES = 0.0f;
};

RiskMetrics GetRiskMetrics(SCStudyGraphRef sc, SCSubgraphRef PercentChanges, int Length, float Percentile)
{
	RiskMetrics rm;

	if (Percentile > 100.0f) return rm;

	std::vector<float> Changes;
	Changes.reserve(Length);

	for (int i = sc.Index; i > sc.Index - Length; i--)
		Changes.push_back(PercentChanges[i]);

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
	auto& VAR_Percent_Subgraph = sc.Subgraph[1];
	auto& ES_Percent_Subgraph = sc.Subgraph[2];
	auto& VAR_Dollar_Subgraph = sc.Subgraph[3];
	auto& ES_Dollar_Subgraph = sc.Subgraph[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Risk Metrics";

		sc.AutoLoop = 1;

		VARLength.Name = "Length (Bars)";
		VARLength.SetInt(252);

		VARPercentile.Name = "Confidence Interval (%)";
		VARPercentile.SetFloat(99.0f);

		DailyChange.Name = "Change %";

		VAR_Percent_Subgraph.Name = "VAR %";

		ES_Percent_Subgraph.Name = "CVAR %";

		VAR_Dollar_Subgraph.Name = "VAR $";
		VAR_Dollar_Subgraph.DrawStyle = DRAWSTYLE_IGNORE;

		ES_Dollar_Subgraph.Name = "CVAR $";
		ES_Dollar_Subgraph.DrawStyle = DRAWSTYLE_IGNORE;
	}

	auto& LastPrice = sc.BaseDataIn[SC_LAST][sc.Index];

	DailyChange[sc.Index] = (LastPrice / sc.BaseDataIn[SC_LAST][sc.Index - 1]) - 1;

	auto Length = VARLength.GetInt();
	auto Percentile = VARPercentile.GetFloat();

	if (sc.Index < Length) return;

	auto RiskMetrics = GetRiskMetrics(sc, DailyChange, Length, Percentile);

	VAR_Percent_Subgraph[sc.Index] = RiskMetrics.VAR;
	ES_Percent_Subgraph[sc.Index] = RiskMetrics.ES;

	VAR_Dollar_Subgraph[sc.Index] = RiskMetrics.VAR * LastPrice;
	ES_Dollar_Subgraph[sc.Index] = RiskMetrics.ES * LastPrice;
}