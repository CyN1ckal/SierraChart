#pragma once
#include "pch.h"

SCSFExport scsf_RSITrader(SCStudyGraphRef sc)
{
	s_SCNewOrder OneLotMarket;
	OneLotMarket.OrderQuantity = 1;
	OneLotMarket.OrderType = SCT_ORDERTYPE_MARKET;
	OneLotMarket.Symbol = sc.Symbol;

	auto& RSISubgraph = sc.Subgraph[0];
	auto& SellThresholdSubgraph = sc.Subgraph[1];
	auto& BuyThresholdSubgraph = sc.Subgraph[2];

	auto& RSILengthInput = sc.Input[0];
	auto& RSIAvgTypeInput = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "RSI Trader";
		sc.StudyDescription = "A simple RSI based trading strategy.";

		sc.AutoLoop = 1;

		RSISubgraph.Name = "RSI";
		RSISubgraph.DrawStyle = DRAWSTYLE_LINE;
		RSISubgraph.PrimaryColor = RGB(180, 50, 180);

		SellThresholdSubgraph.Name = "Sell Threshold";
		SellThresholdSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SellThresholdSubgraph.LineStyle = LINESTYLE_DASH;
		SellThresholdSubgraph.PrimaryColor = RGB(255, 0, 0);

		BuyThresholdSubgraph.Name = "Buy Threshold";
		BuyThresholdSubgraph.DrawStyle = DRAWSTYLE_LINE;
		BuyThresholdSubgraph.LineStyle = LINESTYLE_DASH;
		BuyThresholdSubgraph.PrimaryColor = RGB(0, 255, 0);

		RSILengthInput.Name = "RSI Length";
		RSILengthInput.SetInt(14);

		RSIAvgTypeInput.Name = "RSI Moving Average Type";
		RSIAvgTypeInput.SetMovAvgType(MOVAVGTYPE_SIMPLE);
	}

	SellThresholdSubgraph[sc.Index] = 80.0f;
	BuyThresholdSubgraph[sc.Index] = 20.0f;

	auto& SellThreshold = SellThresholdSubgraph[sc.Index];
	auto& BuyThreshold = BuyThresholdSubgraph[sc.Index];

	auto RSILength = RSILengthInput.GetInt();
	auto RSIAvgType = RSIAvgTypeInput.GetIndex();

	sc.RSI(sc.BaseDataIn[SC_LAST], RSISubgraph, RSIAvgType, RSILength);

	auto& CurrentRSI = RSISubgraph[sc.Index];

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);

	/* If position quantity is non-zero, handle position exit */
	if (PositionData.PositionQuantity)
	{
		if (CurrentRSI > 50 && PositionData.PositionQuantity > 0)
			sc.BuyExit(OneLotMarket);
		else if (CurrentRSI < 50 && PositionData.PositionQuantity < 0)
			sc.SellExit(OneLotMarket);

		return;
	}

	/* Else, handle new positions */
	if (CurrentRSI > SellThreshold)
		sc.SellEntry(OneLotMarket);
	else if (CurrentRSI < BuyThreshold)
		sc.BuyEntry(OneLotMarket);
}