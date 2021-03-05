#include "stdafx.h"
#include "FileInterface.h"


int rwNewgkIP(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	if( bWrite )
		return WriteStrIni("GKIP", dBuffer);
	else
	{
		ReadStrIni("GKIP", dBuffer,bufLength);
		return 1;
	}
	return 0;
}

int rwNewe164Id(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	if( bWrite )
		return WriteStrIni("E164", dBuffer);
	else
	{
		ReadStrIni("E164", dBuffer,bufLength);
		return 1;
	}
	return 0;
}

int rwNewh323Id(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	if( bWrite )
		return WriteStrIni("H323ID", dBuffer);
	else
	{
		ReadStrIni("H323ID", dBuffer,bufLength);
		return 1;
	}
	return 0;
}

int rwRtpPort(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("RTPPORT", *iValue);
	else
	{
		ReadIntIni("RTPPORT", iValue);
		return 1;
	}
	return 0;
}

int rwNewgkPort(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("GKPORT", *iValue);
	else
	{
		ReadIntIni("GKPORT", iValue);
		return 1;
	}
	return 0;
}

int rwEnableCallCouple(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("EnableCallCouple", *iValue);
	else
	{
		ReadIntIni("EnableCallCouple", iValue);
		return 1;
	}
	return 0;
}

int rwInputWaitTime(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("InputWaitTime", *iValue);
	else
	{
		ReadIntIni("InputWaitTime", iValue);
		return 1;
	}
	return 0;
}

int rwMentInterval(int *iVal, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("MentInterval", *iVal);
	else
	{
		ReadIntIni("MentInterval", iVal);
		return 1;
	}
	return 0;
}

int rwInputInitWaitTime(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("InputInitWaitTime", *iValue);
	else
	{
		ReadIntIni("InputInitWaitTime", iValue);
		return 1;
	}
	return 0;
}

int rwFastStart(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("FastStart", *iValue);
	else
	{
		ReadIntIni("FastStart", iValue);
		return 1;
	}
	return 0;
}

int rwH245Tunneling(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("H245Tunneling", *iValue);
	else
	{
		ReadIntIni("H245Tunneling", iValue);
		return 1;
	}
	return 0;
}

int rwAutoStart(int *iValue, BOOL bWrite)
{
	if( bWrite )
		return WriteIntIni("AutoStart", *iValue);
	else
	{
		ReadIntIni("AutoStart", iValue);
		return 1;
	}
	return 0;
}

int WriteIniData(IN char * sKey, IN char * dBuffer);

// read or write string type data.
// if fail, return 0.
int WriteStrIni(IN char *sKey, IN char * dBuffer)
{
	return WriteIniData(sKey, dBuffer);
}

// read or write integer type data
int WriteIntIni(IN char *sKey, IN int iValue )
{
	char tBuff[256];
	wsprintf(tBuff, "%d", iValue);
	return WriteIniData(sKey, tBuff);
}

int WriteIniData(IN char * sKey, IN char * dBuffer)
{
	DWORD dwVal = 0;
	dwVal = WritePrivateProfileString(SECTION_NAME, sKey, dBuffer, ".\\BisCon.ini");
	//dwVal = WriteProfileString(SECTION_NAME, sKey, dBuffer);
	if( dwVal <= 0)
		return 0;
	return 1;
}

// if success, return real read data length.
int ReadStrIni(IN char * sKey, OUT char * dBuffer, IN int dLength)
{
	DWORD dwRet = 0;
	memset(dBuffer, 0, dLength);
#if 0
	dwRet = GetProfileString(SECTION_NAME, sKey, "--", dBuffer, dLength);
#else
	dwRet = GetPrivateProfileString(SECTION_NAME, sKey,  "--",
						dBuffer,dLength, ".\\BisCon.ini" );
#endif
	
	// check, is buffer enough?
	if( !dwRet )
		return 0;
	
	//check, have value?
	if( !memcmp(dBuffer, "--",2) && (strlen(dBuffer)==2) )
		return 0;
	return 1;
}

void ReadIntIni(IN char * sKey, OUT int * iValue)
{
#if 0
	* iValue = GetProfileInt(SECTION_NAME, sKey, 1);
#else
	* iValue = GetPrivateProfileInt(SECTION_NAME, sKey,  1,".\\BisCon.ini" );
#endif
}