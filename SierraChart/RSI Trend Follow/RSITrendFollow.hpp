#pragma once
#include "pch.h"

enum eCurrentTrend
{
	NO_TREND = 0,
	IN_UPTREND = 1,
	IN_DOWNTREND = 2
};

eCurrentTrend DetermineCurrentTrend(SCFloatArrayRef RSIArray, int Index, float LongThreshold, float ShortThreshold)
{
	for (int i = Index; i >= 0; --i)
	{
		if (RSIArray[i] > LongThreshold)
			return IN_UPTREND;
		else if (RSIArray[i] < ShortThreshold)
			return IN_DOWNTREND;
	}

	return NO_TREND;
}

SCSFExport scsf_RSITrendFollow(SCStudyGraphRef sc)
{
	auto& RSISubgraph = sc.Subgraph[0];
	auto& LongThresholdSubgraph = sc.Subgraph[1];
	auto& ShortThresholdSubgraph = sc.Subgraph[2];

	auto& RSILengthInput = sc.Input[0];
	auto& LongThresholdInput = sc.Input[1];
	auto& ShortThresholdInput = sc.Input[2];


	if (sc.SetDefaults)
	{
		sc.GraphName = "RSI Trend Follow";

		RSILengthInput.Name = "RSI Length";
		RSILengthInput.SetInt(21);

		LongThresholdInput.Name = "Long Threshold";
		LongThresholdInput.SetFloat(70.0f);

		ShortThresholdInput.Name = "Short Threshold";
		ShortThresholdInput.SetFloat(20.0f);

		RSISubgraph.Name = "RSI";

		LongThresholdSubgraph.Name = "Long";

		ShortThresholdSubgraph.Name = "Short";

		sc.AutoLoop = 1;
	}

	LongThresholdSubgraph[sc.Index] = LongThresholdInput.GetFloat();
	ShortThresholdSubgraph[sc.Index] = ShortThresholdInput.GetFloat();

	sc.RSI(sc.BaseDataIn[SC_LAST], RSISubgraph, MOVAVGTYPE_SIMPLE, RSILengthInput.GetInt());

	auto& CurrentRSI = RSISubgraph[sc.Index];

	if (sc.IsFullRecalculation) return;

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);
	auto& PositionSize = PositionData.PositionQuantity;

	s_SCNewOrder NewOrder = {};
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
	NewOrder.Symbol = sc.Symbol;

	auto CurrentTime = sc.BaseDateTimeIn[sc.Index].GetTime();

	static constexpr uint32_t EndingTime = 46800 - 300;
	static constexpr uint32_t StartingTime = 23400 + 300;

	if(PositionSize && CurrentTime > EndingTime)
	{
		sc.FlattenAndCancelAllOrders();

		return;
	}

	if (CurrentTime > EndingTime || CurrentTime < StartingTime)
		return;

	auto Trend = DetermineCurrentTrend(RSISubgraph, sc.Index, LongThresholdInput.GetFloat(), ShortThresholdInput.GetFloat());

	if (PositionSize)
	{
		if(PositionSize > 0 && Trend == IN_DOWNTREND)
			sc.BuyExit(NewOrder);
		else if (PositionSize < 0 && Trend == IN_UPTREND)
			sc.SellExit(NewOrder);
	}
	else
	{
		if (Trend == IN_UPTREND && CurrentRSI < 50.0f)
			sc.BuyEntry(NewOrder);
		else if (Trend == IN_DOWNTREND && CurrentRSI > 50.0f)
			sc.SellEntry(NewOrder);
	}
}