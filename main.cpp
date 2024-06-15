#include <Windows.h>

// Maybe it depends on computer environment
// Inspired : https://youtu.be/3z8Brq7Hb0c?si=gvwZz7yqhgivo14b

const float UPDATE_LATE = 1.f / 60.f;

const int KEY_ROLL		= VK_SPACE;
const int KEY_DEFAULT	= VK_LSHIFT;
const int TARGET_KEY	= 'P';

#if 0
#include "stdio.h"
#define LOG(_text, _code) printf("%-20s : code =\t%-10d, ticks =\t%-10llx\n", _text, _code, GetTickCount64())
#else
#define LOG(_text, _code) ((void*)0)
#endif


int 
main(int, char**)
{
	LARGE_INTEGER	llBaseCount;
	LARGE_INTEGER	llCurCount;
	LARGE_INTEGER	llFrequency;
	LONG64 			llCallCount = 0;
	float			fDeltaTime	= 0.f;
	float			fAcc		= 0.f;
	float			fAccTime	= 0.f;
	float			fLateTime	= 0.f;

	INPUT	input = { 0 };
	bool	key_roll_down		= false;
	bool	key_roll_down_delay = false;
	bool	key_default_down	= false;

	WORD sTargetKey = MapVirtualKey(LOBYTE(VkKeyScan(TARGET_KEY)), 0);

	QueryPerformanceCounter(&llBaseCount);
	if (0 == QueryPerformanceFrequency(&llFrequency))
	{
		goto lb_end;
	}

	fLateTime = UPDATE_LATE;

	input.type			= INPUT_KEYBOARD;
	input.ki.dwFlags	= KEYEVENTF_SCANCODE;
	input.ki.wScan		= sTargetKey;

	for (;;)
	{
		fAccTime = fAccTime + fDeltaTime;
		if (fLateTime <= fAccTime)
		{
			fAccTime = 0.0f;
			input.ki.dwFlags = KEYEVENTF_SCANCODE;

			if (GetAsyncKeyState(KEY_ROLL))
			{
				if (!key_roll_down)
				{
					if (key_default_down)
					{
						key_roll_down_delay = true;
						input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
						SendInput(1, &input, sizeof(input));
						input.ki.dwFlags = KEYEVENTF_SCANCODE;
						Sleep(10);
					}

					LOG("KEY_ROLL<DOWN>", KEY_ROLL);
					key_roll_down = true;
					SendInput(1, &input, sizeof(input));
					Sleep(20);
					input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					SendInput(1, &input, sizeof(input));
					continue;
				}
			} else
			{
				if (key_roll_down) LOG("KEY_ROLL<UP>", KEY_ROLL);
				key_roll_down = false;
			}

			if (GetAsyncKeyState(KEY_DEFAULT))
			{
				if (!key_default_down) LOG("KEY_DEFAULT<DOWN>", KEY_DEFAULT);
				if (key_roll_down_delay) Sleep(10);
				key_default_down = true;
				SendInput(1, &input, sizeof(input));

			} else
			{
				if (key_default_down) LOG("KEY_DEFAULT<UP>", KEY_DEFAULT);
				key_default_down = false;
				input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
				SendInput(1, &input, sizeof(input));
			}
		} 

		QueryPerformanceCounter(&llCurCount);
		fDeltaTime = (float)(llCurCount.QuadPart - llBaseCount.QuadPart)
			/ (float)(llFrequency.QuadPart);

		llBaseCount = llCurCount;

		fAcc = fAcc + fDeltaTime;
		++llCallCount;

		if (1.f <= fAcc)
		{
			llCallCount = 0;
			fAcc		= 0.f;
		}
	} 
	
lb_end:
	return 0;
}
