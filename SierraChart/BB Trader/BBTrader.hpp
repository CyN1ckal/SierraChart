#pragma once
#include "pch.h"

bool IsValidTradeTime(int Time)
{
	/* after 12:55 pm */
	if (Time > 46500) return false;

	/* before 6:30 am */
	if (Time < 23400) return false;

	return true;
}

bool ShouldStopout(float StopAmount, int Time, float CurrentPrice, s_SCPositionData& Position)
{
	auto& PositionQuantity = Position.PositionQuantity;
	float Delta = CurrentPrice - Position.AveragePrice;

	if (PositionQuantity == 0)
		return false;

	if (!IsValidTradeTime(Time)) return true;

	if (PositionQuantity > 0 && Delta < StopAmount * -1.0f)
		return true;

	if (PositionQuantity < 0 && Delta > StopAmount)
		return true;

	return false;
}

SCSFExport scsf_BBTrader(SCStudyGraphRef sc)
{
	auto& HTF_TopBB = sc.Subgraph[0];
	auto& HTF_BottomBB = sc.Subgraph[1];
	auto& LTF_TopBB = sc.Subgraph[2];
	auto& LTF_BottomBB = sc.Subgraph[3];
	auto& RefPrice = sc.Subgraph[4];

	auto& LiveInput = sc.Input[0];
	auto& StopInput = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "BB Trader";

		sc.AutoLoop = 1;

		sc.AllowMultipleEntriesInSameDirection = false;

		LiveInput.Name = "Send Orders to Trade Service?";
		LiveInput.SetYesNo(false);

		StopInput.Name = "Stop Amount (P)";
		StopInput.SetFloat(250.0f);

		HTF_TopBB.Name = "HTF Top BB";
		HTF_TopBB.DrawStyle = DRAWSTYLE_LINE;
		HTF_TopBB.PrimaryColor = RGB(255, 0, 0);
		HTF_TopBB.LineWidth = 3;

		HTF_BottomBB.Name = "HTF Bottom BB";
		HTF_BottomBB.DrawStyle = DRAWSTYLE_LINE;
		HTF_BottomBB.PrimaryColor = RGB(0, 255, 0);
		HTF_BottomBB.LineWidth = 3;

		LTF_TopBB.Name = "LTF Top BB";
		LTF_TopBB.DrawStyle = DRAWSTYLE_LINE;
		LTF_TopBB.PrimaryColor = RGB(255, 0, 0);
		LTF_TopBB.LineWidth = 1;

		LTF_BottomBB.Name = "LTF Bottom BB";
		LTF_BottomBB.DrawStyle = DRAWSTYLE_LINE;
		LTF_BottomBB.PrimaryColor = RGB(0, 255, 0);
		LTF_BottomBB.LineWidth = 1;

		RefPrice.Name = "Ref Price";
		RefPrice.DrawStyle = DRAWSTYLE_LINE;
		RefPrice.PrimaryColor = RGB(255, 255, 255);
		RefPrice.LineWidth = 1;
	}

	sc.SendOrdersToTradeService = LiveInput.GetYesNo();

	sc.BollingerBands(sc.BaseDataIn[SC_LAST], RefPrice, 100, 2.0f, MOVAVGTYPE_SIMPLE);
	HTF_TopBB[sc.Index] = RefPrice.Arrays[0][sc.Index];
	HTF_BottomBB[sc.Index] = RefPrice.Arrays[1][sc.Index];

	sc.BollingerBands(sc.BaseDataIn[SC_LAST], LTF_TopBB, 21, 2.0f, MOVAVGTYPE_SIMPLE);
	LTF_TopBB[sc.Index] = LTF_TopBB.Arrays[0][sc.Index];
	LTF_BottomBB[sc.Index] = LTF_TopBB.Arrays[1][sc.Index];

	auto& CurrentPrice = sc.BaseDataIn[SC_LAST][sc.Index];
	auto& CurrentHTFTop = HTF_TopBB[sc.Index];
	auto& CurrentHTFBottom = HTF_BottomBB[sc.Index];
	auto& CurrentLTFTop = LTF_TopBB[sc.Index];
	auto& CurrentLTFBottom = LTF_BottomBB[sc.Index];
	auto& CurrentRefPrice = RefPrice[sc.Index];

	auto& CurrentTime = sc.BaseDateTimeIn[sc.Index];
	auto Time = CurrentTime.GetTime();

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);
	auto& PositionQuantity = PositionData.PositionQuantity;

	if (PositionQuantity == 0 && !IsValidTradeTime(Time)) return;

	if (ShouldStopout(StopInput.GetFloat(), Time, CurrentPrice, PositionData))
	{
		sc.FlattenAndCancelAllOrders();
		return;
	}

	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
	NewOrder.Symbol = sc.TradeAndCurrentQuoteSymbol;

	if (PositionQuantity)
	{
		if (PositionQuantity > 0 && CurrentPrice > CurrentRefPrice)
			sc.BuyExit(NewOrder);
		else if (CurrentPrice < CurrentRefPrice)
			sc.SellExit(NewOrder);
	}
	else
	{
		if (CurrentPrice > CurrentHTFTop && CurrentPrice > CurrentLTFTop)
			sc.SellEntry(NewOrder);
		else if (CurrentPrice < CurrentHTFBottom && CurrentPrice < CurrentLTFBottom)
			sc.BuyEntry(NewOrder);
	}
}