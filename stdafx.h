/* stdafx.h : include file for standard system include files, 
or project specific include files that are used frequently, but are changed infrequently */

#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <string>
#include <vector>
#include <sstream>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <timeapi.h>
#include <time.h>
#include <string>
#include <ctime>
#include <algorithm>

//#pragma warning(disable : 4244 4305 4551 4800 4805) // double <-> float conversions

#pragma comment(lib, "winmm.lib")

extern MODULEINFO g_MainModuleInfo;

#define version "v.38"

// Mine
#include "pattern.h"

// Rockstar
#include "types.h"
#include "pgcollection.h"
#include "scrthread.h"
#include "natives.h"
#include "rage.h"

// Main
#include "script.h"

void Tick();
void Run();
void RunUnreliable();