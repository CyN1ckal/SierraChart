#pragma once
#include "pch.h"

SCSFExport scsf_ScanHelper(SCStudyGraphRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Scan Helper";

		sc.AutoLoop = 1;

		sc.FreeDLL = 1;

		sc.Input[0].Name = "Study ID To Save";
		sc.Input[0].SetStudyID(0);

		sc.Input[1].Name = "Study Subgraph To Save";
		sc.Input[1].SetSubgraphIndex(0);

		sc.Subgraph[0].Name = "Study Subgraph";
	}

	if (sc.IsFullRecalculation || sc.DownloadingHistoricalData) return;

	SCFloatArray StudyReference;
	if (sc.GetStudyArrayUsingID(sc.Input[0].GetStudyID(), sc.Input[1].GetSubgraphIndex(), StudyReference) && StudyReference.GetArraySize())
	{
		sc.Subgraph[0][sc.Index] = StudyReference[sc.Index];

		std::ofstream outputFile("scan_output.txt", std::ios::app);
		if (outputFile.is_open())
		{
			std::string Symbol = sc.Symbol.GetChars();
			outputFile << std::format("{0:s}, {1:.4f}\n", Symbol, sc.Subgraph[0][sc.Index]);
			outputFile.close();
		}
	}
}