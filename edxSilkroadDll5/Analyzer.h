#pragma once

#include <windows.h>

void Analyzer_Setup();
void Analyzer_Cleanup();

void Analyzer_SetOpcode(WORD opcode, bool s2c);
void Analyzer_StreamData(LPBYTE stream, DWORD count);
void Analyzer_EndPacket();
