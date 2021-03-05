//////////////////////////////////////////////////////////////////////////////////
// file		: management.cpp
//
// update	: ..
//			: 2006/06/10 02:47 swhors@naver.com
// 
// descrip	: This file is for IVR's ment management.

#include "stdafx.h"
#include "ivrkeydata.h"
#include "scm.h"
#include "voice.h"
#include "SystembaseSet.h"
#include "operate.h"

#include "management.h"

void WriteLog(char * filename, int a, char * c, char * b);
#if 0
#define WRITELOG(d, a,b,c) WriteLog( d, a, b, c);
#else
#define WRITELOG(d, a,b,c)
#endif

int FindAnyFileByExt(int iCodec, char *sbuf);

int flength(FILE *fp)
{
	int iLen=0;
	char buf[1];
	while(fread(buf,1,1, fp))
	{
		iLen ++;
	}
	fseek( fp, 0L, SEEK_SET );
	return iLen;
}

int LoadFolder(int iPage,int iCurCodecPage,int iCodec,char *sbuf)
{
	int		iFileSize = 0, iTotal;
	char	CurrentDirectory[256];
	char	fileName[256];
	FILE *fp;
	char *lowBuffer;
	iTotal = mentfileList_size();
	for(int i = 0; i<iTotal;i++)
	{
		memset(fileName, 0, 256);
		if( mentfileList_get_name(i,fileName) )
		{
			memset(CurrentDirectory, 0, 256);
			GetVoiceFileFolder(sbuf, CurrentDirectory, 256);
			
			CurrentDirectory[strlen(CurrentDirectory)-1] = 0;
			CurrentDirectory[strlen(CurrentDirectory)-1] = 0;
			for(int b = 0; b< 256; b++)
			{
				if( fileName[b]=='\r' && fileName[b+1]=='\n')
				{
					fileName[b] = 0;fileName[b+1] =0;
					break;
				}
				else if(fileName[b]=='\n')
				{
					fileName[b] = 0;
					break;
				}
			}
			switch(iCodec)
			{
			case 0:
				strcat(fileName, ".g7231");
				break;
			case 1:
				strcat(fileName, ".g711a");
				break;
			case 2:
				strcat(fileName, ".g711u");
				break;
			case 3:
				strcat(fileName, ".g729");
				break;
			case 4:
				strcat(fileName, ".g729a");
				break;
			default:
				break;
			}
			strcat(CurrentDirectory, fileName);
			fp = fopen(CurrentDirectory, "r+b");
			if( fp )
			{
				WRITELOG("LoadFile", loadfilecount++, "filename", CurrentDirectory );
				iFileSize = flength(fp);
				unsigned char *dBuf=NULL;
				dBuf = new unsigned char[iFileSize+1];
				memset(dBuf, 0, (iFileSize+1));
				fread( dBuf, 1, iFileSize, fp);
				lowBuffer = _strlwr( fileName );
				pVoicePage[iPage].insertData(iCodec, lowBuffer, dBuf, iFileSize);
				fclose(fp);
				delete dBuf;
			}
			
		}
	}
	return 1;
}

void LoadingMentFile()
{
	int iCount = 0, i =0;
	FILE * fp = fopen("language.ini", "rt");
	if(!fp)
	{
		return;
	}
	for( i= 0; i< 8; i++)
	{
		char sLine[16];
		memset(sLine, 0, 16);
		if( !fgets(sLine, 16, fp))
		{
			break;
		}
		if( strlen(sLine) > 0 )
		{
			if( sLine[0]!='\r' && sLine[0]!='\n')
			{
                sprintf(sLanguage[iCount],"%s", sLine);
			    int iPos = strcspn(sLanguage[iCount], "\r");
			    iInstalledLang++;
			    if( iPos >=0)
				   sLanguage[iCount][iPos] = 0;
			    iPos = strcspn(sLanguage[iCount], "\n");
			    if( iPos >=0)
				    sLanguage[iCount][iPos] = 0;
			    iCount++;
			}
		}
	}
	fclose(fp);
	if( iCount == 0)
	{
		return;
	}
	iVocePageCount = iCount;
	pVoicePage = new VOICE_PAGE[iCount];
	if( pVoicePage )
	{
		int iCodec[MAX_SUPP_CODEC];
		int	iCodecCount=0;
		memset(iCodec, 0, sizeof(int)*5);
		for(i = 0; i< iCount; i++)
		{
			for( int b = 0; b < MAX_SUPP_CODEC; b++)
			{
			    if( FindAnyFileByExt(b,sLanguage[i]) )
				{
					iCodec[iCodecCount++] = b;
				}
			}
			if( iCodecCount > 0 )
			{
				pVoicePage[i].allocPage(sLanguage[i], iCodec, i, iCodecCount);
				for(int c =0; c <iCodecCount; c++)
				{
				    LoadFolder(i, c, iCodec[c], sLanguage[i]);
				}
				pVoicePage[i].sortAllData();
				memset(iCodec, 0, sizeof(int)*5);
				iCodecCount = 0;
			}
		}
	}
}

void GetVoiceFileFolder(char *sbuf, char * drBuffer, int iLen)
{
	int iPathLen = 0;
	memset(drBuffer, 0, iLen);
	::GetCurrentDirectory(iLen, drBuffer);
	iPathLen = strlen(drBuffer);
	//아래의 루틴 수정 .. < 특정 폴더만 인식하던 오류 시정
	//
	//memset( (char *)&drBuffer[iPathLen-3], 0, 3);
	//strcat(drBuffer, "voice\\");
	//
	strcat(drBuffer, "\\..\\bvoice\\");
	//
	strcat(drBuffer, sbuf);
	strcat(drBuffer, "\\*.");
}

int FindAnyFileByExt(int iCodec, char *sbuf)
{
	WIN32_FIND_DATA findData;
	char	CurrentDirectory[256];
	memset(CurrentDirectory, 0, 256);
	GetVoiceFileFolder(sbuf, CurrentDirectory, 256);
	strcat(CurrentDirectory, sCodec[iCodec]);
	HANDLE hSearch = ::FindFirstFile(CurrentDirectory, &findData);
	if( hSearch == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	// Close the search handle. 
	FindClose(hSearch);	
	return 1;
}

void UnLoadingMentFile()
{
	if( pVoicePage )
	{
		delete []pVoicePage;
		pVoicePage = NULL;
	}
}

//unsigned char * GetMentData(IN int iFileLanguagePage,
//							IN int iFileCodecPage,
//							IN int iFileNum,
//							OUT PBYTE sOutbuffer)
//{
//	pVoicePage[iFileLanguagePage].getData(iFileCodecPage, NULL, sOutbuffer, iFileNum);
//	return NULL;
//}

int	GetMentData(IN char *sFileName,
						IN int iCodec,
						IN char *sLanguage,
						IN int &iFileLanguagePage,
						IN int &iFileCodecPage,
						OUT PBYTE &sOutBuffer)
{
	char *lowBuffer,*lowBuffer2, *lowFname;
	lowBuffer = _strlwr( sLanguage );
	int iLength=0;
	for( int i = 0; i < iVocePageCount; i++)
	{
		lowBuffer2 = _strlwr( pVoicePage[i].language );
		if(!memcmp(lowBuffer, lowBuffer2 ,2) )
		{
				// 0 g723, 1 g711alaw, 
				// 2 g711mulaw,3 g729, 4 g729a default -1 [none]
			lowFname = _strlwr(sFileName);
			char mFile[256];
			switch(iCodec)
			{
			case 0: // g7231
				sprintf(mFile,"%s.g7231", lowFname);
				break;
			case 1: // g711a
				sprintf(mFile,"%s.g711a", lowFname);
				break;
			case 2: // g711u
				sprintf(mFile,"%s.g711u", lowFname);
				break;
			case 3: // g729
				sprintf(mFile,"%s.g729", lowFname);
				break;
			case 4: // g729a
				sprintf(mFile,"%s.g729a", lowFname);
				break;
			default:
				return 0;
			}
			iLength = pVoicePage[i].getDataPtr(iCodec,mFile, sOutBuffer);
		    if( iLength > 0)
			{
				iFileLanguagePage = i;
				return iLength;
			}
		}
		
	}
	return 0;
}

int GetMentDataLength(char *sFileName, 
					  int iCodec, 
					  char *sLanguage, 
					  int &iFileLanguagePage, 
					  int &iFileCodecPage,OUT PBYTE sOutbuffer)
{
	char *lowBuffer,*lowBuffer2, *lowFname;
	lowBuffer = _strlwr( sLanguage );
	int iLength=0;
	for( int i = 0; i < iVocePageCount; i++)
	{
		lowBuffer2 = _strlwr( pVoicePage[i].language );
		if(!memcmp(lowBuffer, lowBuffer2 ,2) )
		{
				// 0 g723, 1 g711alaw, 
				// 2 g711mulaw,3 g729, 4 g729a default -1 [none]
			lowFname = _strlwr(sFileName);
			char mFile[256];
			switch(iCodec)
			{
			case 0: // g7231
				sprintf(mFile,"%s.g7231", lowFname);
				break;
			case 1: // g711a
				sprintf(mFile,"%s.g711a", lowFname);
				break;
			case 2: // g711u
				sprintf(mFile,"%s.g711u", lowFname);
				break;
			case 3: // g729
				sprintf(mFile,"%s.g729", lowFname);
				break;
			case 4: // g729a
				sprintf(mFile,"%s.g729a", lowFname);
				break;
			default:
				return 0;
			}
			iLength = pVoicePage[i].getData(iCodec,mFile, sOutbuffer);
		    if( iLength > 0)
			{
				iFileLanguagePage = i;
				return iLength;
			}
		}
		
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
// end of file..
//
