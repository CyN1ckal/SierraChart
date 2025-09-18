#pragma once
#include "pch.h"

inline float Sum(int scIndex, SCFloatArrayRef InputData, int length)
{
	if (!length) return 0.0f;

	if (scIndex < length) return 0.0f;

	float sum = 0.0f;

	for (int i = 0; i < length; i++)
	{
		sum += InputData[scIndex - i];
	}

	return sum;
}