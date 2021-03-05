#include "stdafx.h"
#include "scm.h"
#include "ivrkeydata.h"
#include "voice.h"
#include "plugindef.h"
#include "stdio.h"

#include "SystemBaseSet.h"

void LoadPlugDll()
{
	WIN32_FIND_DATA findData;
	HANDLE hSearch;
	BOOL fFinished = FALSE;
	int		nFileLength = 0;
	FILE * fp;
	char	szLineBuf[256];
	char	szDirPath[256];
	char	szNewPath[256];
	iDllCount=0;
	for( int i = 0; i < 10; i++)
	{
		handleBuffer[i].hMod = NULL;
		memset( handleBuffer[i].szModName, 0, 256);
	}
	::GetCurrentDirectory(256, szDirPath);
	memset( szNewPath, 0, 256);
	wsprintf(szNewPath, "%s", szDirPath);
	strcat(szNewPath, "\\*.plg");
	hSearch = ::FindFirstFile(szNewPath, &findData);
	if( hSearch == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	
	while(!fFinished)
	{
		memset(szNewPath, 0, 256);
		lstrcpy(szNewPath, szDirPath);
		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, findData.cFileName); 
		if( (fp = fopen(szNewPath, "rt") ))
		{
			memset( szLineBuf, 0, 256);
			
			if( fgets( szLineBuf, 256, fp))
			{
				char modPath[MAX_PATH];
				szLineBuf[strlen(szLineBuf) - 1] = 0;
				sprintf(modPath, "%s.dll", (char *)&szLineBuf[9]);
				HMODULE hModule = ::LoadLibrary(modPath);
				
				if( hModule )
				{
					handleBuffer[iDllCount].hMod = hModule;
					sprintf(handleBuffer[iDllCount++].szModName, (char *)&szLineBuf[9]);
				}
			}
			fclose(fp);
		}
		if (!FindNextFile(hSearch, &findData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = TRUE; 
			} 
		}
	}
	// Close the search handle. 
	FindClose(hSearch);
}

HMODULE GetPlugInHandle(char *sModule)
{
	int iLen = strlen(sModule);
	for( int i = 0; i < iDllCount ; i++)
	{
		
		if(!memcmp( handleBuffer[i].szModName, sModule, iLen))
		{
			if( (int) strlen(handleBuffer[i].szModName) == iLen)
				return handleBuffer[i].hMod;
		}
	}
	return NULL;
}

void InitPlugInModule()
{
	InitPlugin * initPlug;
	for( int i = 0; i < iDllCount ; i++)
	{
		if( handleBuffer[i].hMod )
		{
			initPlug = (InitPlugin*)::GetProcAddress(handleBuffer[i].hMod, "InitPlugin");
			if(initPlug)
			{
				initPlug(iMaxAllowedCall, szGlobalGkIp);
			}
		}
	}
}

void release_plug()
{
	for( int it = 0; it < iDllCount; it++)
	{
		::FreeLibrary(handleBuffer[it].hMod);
	}
}

void UnInitPlugInModule()
{
	for( int it = 0; it < iDllCount; it++)
	{
		ReleasePlugin * release;
		release = (ReleasePlugin *)::GetProcAddress(handleBuffer[it].hMod,"ReleasePlugin");
		if( release)
			release(0);
	}
}