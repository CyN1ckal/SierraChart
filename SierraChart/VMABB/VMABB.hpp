#pragma once
#include "pch.h"

SCSFExport scsf_VMABB(SCStudyGraphRef sc)
{
	static constexpr uint32_t EndingTime = 46800 - 300;
	static constexpr uint32_t StartingTime = 23400 + 300;

	auto& FastVMAInput = sc.Input[0];
	auto& SlowVMAInput = sc.Input[1];
	auto& BollingerBandInput = sc.Input[2];

	auto& FastVMASubgraph = sc.Subgraph[0];
	auto& SlowVMASubgraph = sc.Subgraph[1];
	auto& ATRSubgraph = sc.Subgraph[2];
	auto& BollingerTopSubgraph = sc.Subgraph[3];
	auto& BollingerBottomSubgraph = sc.Subgraph[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "VMA BB 1.0";

		sc.AutoLoop = 1;

		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		FastVMAInput.Name = "Fast VMA";
		FastVMAInput.SetStudyID(1);

		SlowVMAInput.Name = "Slow VMA";
		SlowVMAInput.SetStudyID(2);

		BollingerBandInput.Name = "Bollinger Band";
		BollingerBandInput.SetStudyID(3);

		FastVMASubgraph.Name = "Fast VMA";
		FastVMASubgraph.DrawStyle = DRAWSTYLE_LINE;

		SlowVMASubgraph.Name = "Slow VMA";
		SlowVMASubgraph.DrawStyle = DRAWSTYLE_LINE;

		ATRSubgraph.Name = "ATR";
		ATRSubgraph.DrawStyle = DRAWSTYLE_IGNORE;

		BollingerTopSubgraph.Name = "Bollinger Top";
		BollingerTopSubgraph.DrawStyle = DRAWSTYLE_LINE;
		BollingerTopSubgraph.LineWidth = 3;
		BollingerTopSubgraph.PrimaryColor = RGB(128, 128, 128);

		BollingerBottomSubgraph.Name = "Bollinger Bottom";
		BollingerBottomSubgraph.DrawStyle = DRAWSTYLE_LINE;
		BollingerBottomSubgraph.LineWidth = 3;
		BollingerBottomSubgraph.PrimaryColor = RGB(128, 128, 128);
	}

	SCFloatArray FastVMAData;
	sc.GetStudyArrayUsingID(FastVMAInput.GetStudyID(), 0, FastVMAData);
	FastVMASubgraph[sc.Index] = FastVMAData[sc.Index];

	SCFloatArray SlowVMAData;
	sc.GetStudyArrayUsingID(SlowVMAInput.GetStudyID(), 0, SlowVMAData);
	SlowVMASubgraph[sc.Index] = SlowVMAData[sc.Index];

	SCFloatArray BollingerTopData;
	sc.GetStudyArrayUsingID(BollingerBandInput.GetStudyID(), 0, BollingerTopData);
	BollingerTopSubgraph[sc.Index] = BollingerTopData[sc.Index];

	SCFloatArray BollingerBottomData;
	sc.GetStudyArrayUsingID(BollingerBandInput.GetStudyID(), 2, BollingerBottomData);
	BollingerBottomSubgraph[sc.Index] = BollingerBottomData[sc.Index];

	sc.ATR(sc.BaseDataIn, ATRSubgraph, 120, MOVAVGTYPE_SIMPLE);

	if (sc.IsFullRecalculation) return;

	auto& CurrentFastVMA = FastVMASubgraph[sc.Index];
	auto& CurrentSlowVMA = SlowVMASubgraph[sc.Index];
	auto& CurrentPrice = sc.BaseDataIn[SC_LAST][sc.Index];
	auto& CurrentATR = ATRSubgraph[sc.Index];
	auto& CurrentBottomBand = BollingerBottomSubgraph[sc.Index];
	auto& CurrentTopBand = BollingerTopSubgraph[sc.Index];

	auto CurrentTime = sc.BaseDateTimeIn[sc.Index].GetTime();

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);
	auto& PositionSize = PositionData.PositionQuantity;

	if (PositionSize && CurrentTime > EndingTime)
	{
		sc.FlattenAndCancelAllOrders();
		return;
	}

	if (CurrentTime < StartingTime || CurrentTime > EndingTime) return;

	auto VMADelta = CurrentFastVMA - CurrentSlowVMA;

	s_SCNewOrder NewOrder = {};
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
	NewOrder.Symbol = sc.Symbol;

	if (PositionSize)
	{
		if (PositionSize > 0 && CurrentFastVMA < CurrentSlowVMA)
			sc.BuyExit(NewOrder);
		else if (PositionSize < 0 && CurrentFastVMA > CurrentSlowVMA)
			sc.SellExit(NewOrder);
	}
	else
	{
		if (std::abs(VMADelta) < CurrentATR) return;

		if (CurrentFastVMA < CurrentSlowVMA && CurrentPrice > CurrentTopBand)
			sc.SellEntry(NewOrder);
		else if (CurrentFastVMA > CurrentSlowVMA && CurrentPrice < CurrentBottomBand)
			sc.BuyEntry(NewOrder);
	}
}