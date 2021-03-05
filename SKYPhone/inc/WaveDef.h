#pragma once

#define WAVE_FORMAT_ON_G723	66 // Microsoft g.723

typedef struct _WAVEDATAFORM
{
	char _riff[4];
	DWORD dwDataLength;
	char _wave[4];
	char _fmt[4];
	DWORD dwFormatSize;
	WAVEFORMATEX wf;
	DWORD  wExtraData;
	char _data[4];
} WAVEDATAFORM,*PWAVEDATAFORM;

typedef struct _WAVEFORMAEX
//typedef struct _G7231FORMAT
{
	WAVEFORMATEX wf;
	BYTE extra[10];
//} G7231FORMAT,*PG7231FORMAT;
}WAVEFORMEX, *PWAVEFORMEX;

typedef struct _DRIVERINFO
{
	HACMDRIVERID hadid;
	WORD wFormatTag;
	WAVEFORMATEX* pWf;
}DRIVERINFO,*PDRIVERINFO;
