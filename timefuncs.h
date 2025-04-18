#pragma once

#ifndef TIMEFUNCS_DEF
#define TIMEFUNCS_DEF

#include <windows.h>

inline long long int getTimeInMilliseconds(void) {
	LARGE_INTEGER ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&ElapsedMicroseconds);

	ElapsedMicroseconds.QuadPart *= 1000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

	return ElapsedMicroseconds.QuadPart;
}

#endif
