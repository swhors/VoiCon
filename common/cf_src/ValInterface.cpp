#include "stdafx.h"
#include "ValInterface.h"
//#include "netutils.c"

#include "cm.h"
#include "ci.h"

HCFG hCfg;
BOOL	bSave;

// if success, return 1
// else if unsuccess, return 0.
int OpenVal()
{
	hCfg = NULL;
	hCfg = ciConstructEx(VAL_FILE_NAME,1024,31);
	IS_VALID_HCFG(hCfg, 0);
	bSave = FALSE;
	return 1;
}

void CloseVal()
{
	if( bSave )
		ciSave(hCfg,VAL_FILE_NAME);
	ciDestruct(hCfg);
	bSave = FALSE;
}

int delE164Id1()
{
	return DeleteVal(E164IDPATH11);
}

int delE164Id2()
{
	return DeleteVal(E164IDPATH21);
}

int delE164Id3()
{
	return DeleteVal(E164IDPATH31);
}

int delE164Id4()
{
	return DeleteVal(E164IDPATH41);
}

int delE164Id5()
{
	return DeleteVal(E164IDPATH51);
}

int delE164Id6()
{
	return DeleteVal(E164IDPATH61);
}

// if fail, return 0.
// read or write e164 id.
int rwe164Id1(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	int iStatus;
	int iReadLength = bufLength;
	if( bWrite )
		iStatus = WriteStrVal(E164IDPATH1, dBuffer, bufLength);
	else
		iStatus = ReadStrVal(E164IDPATH1, dBuffer, &iReadLength);
	return iStatus;
}

int rwe164Id2(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	int iStatus;
	int iReadLength = bufLength;
	if( bWrite )
		iStatus = WriteStrVal(E164IDPATH2, dBuffer, bufLength);
	else
		iStatus = ReadStrVal(E164IDPATH2, dBuffer, &iReadLength);
	return iStatus;
}

int rwe164Id3(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	int iStatus;
	int iReadLength = bufLength;
	if( bWrite )
		iStatus = WriteStrVal(E164IDPATH3, dBuffer, bufLength);
	else
		iStatus = ReadStrVal(E164IDPATH3, dBuffer, &iReadLength);
	return iStatus;
}
int rwe164Id4(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	int iStatus;
	int iReadLength = bufLength;
	if( bWrite )
		iStatus = WriteStrVal(E164IDPATH4, dBuffer, bufLength);
	else
		iStatus = ReadStrVal(E164IDPATH4, dBuffer, &iReadLength);
	return iStatus;
}
int rwe164Id5(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	int iStatus;
	int iReadLength = bufLength;
	if( bWrite )
		iStatus = WriteStrVal(E164IDPATH5, dBuffer, bufLength);
	else
		iStatus = ReadStrVal(E164IDPATH5, dBuffer, &iReadLength);
	return iStatus;
}
int rwe164Id6(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	int iStatus;
	int iReadLength = bufLength;
	if( bWrite )
		iStatus = WriteStrVal(E164IDPATH6, dBuffer, bufLength);
	else
		iStatus = ReadStrVal(E164IDPATH6, dBuffer, &iReadLength);
	return iStatus;
}
// char to BMP String
// OUT	char *sBuffer	: output bmpstring buffer
// IN	int	isLen		: input sBuffer's length
// IN	char *cBuffer	: input character buffer
// IN	int iLen		: input cBuffer's length
void char_to_short(OUT char *sBuffer, int isLen, IN char *cBuffer, int iLen)
{
	if( iLen*2 < isLen)
	{
		memset( sBuffer, 0, isLen);
		return;
	}
	memset(sBuffer, 0, isLen);
	for( int i = 0; i < iLen; i++)
	{
		wsprintf(&sBuffer[i*2+1],"%c", cBuffer[i]);
	}
}

// BMP String to char
// OUT	char *cBuffer	: output character buffer
// IN	int	iLen		: cBuffer's length
// IN	char *sBuffer	: input bmpstring buffer
// IN	int isLen		: sBuffer's length
void short_to_char(OUT char *cBuffer, IN int iLen, IN char *sBuffer, IN int isLen)
{
	int t=0, repeat = isLen/2;
	memset( cBuffer, 0, iLen);
	if( iLen*2 < isLen) return;
	for( int i = 0; i < repeat;i++)
	{
		t = i*2+1;
		wsprintf(&cBuffer[i],"%c", sBuffer[t]);
	}
}

// if fail, return 0.
// read or write h323 id.
int rwh323Id(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	int iRetCode = 0;
	int iReadLeng=bufLength*2;
	char *tbuf = (char *) malloc(iReadLeng+2);
	memset( tbuf, 0, iReadLeng+2);
	if( bWrite )
	{
		char_to_short(tbuf, iReadLeng, dBuffer, bufLength);
		iRetCode = WriteStrVal(H323IDPATH, tbuf, iReadLeng);
	}
	else
	{
		iRetCode = ReadStrVal(H323IDPATH, tbuf, &iReadLeng);
		if( iRetCode != 0)
		{
			memset( dBuffer, 0, bufLength);
			short_to_char(dBuffer, bufLength, tbuf, iReadLeng);
		}
	}
	free(tbuf);
	return iRetCode;
}

int rwlocalRPort(IN int *iPortNum, IN BOOL bWrite)
{
	if( bWrite )
	{
		return WriteIntVal(RPORTPATH, *iPortNum);
	}
	else
	{
		return ReadIntVal(RPORTPATH, iPortNum);
	}
	return 0;
}

int rwlocalQPort(IN int *iPortNum, IN BOOL bWrite)
{
	if( bWrite )
	{
		return WriteIntVal(QPORTPATH, *iPortNum);
	}
	else
	{
		return ReadIntVal(QPORTPATH, iPortNum);
	}
	return 0;
}

int rwreponseTimeout(IN int *iNum, IN BOOL bWrite)
{
	if( bWrite )
	{
		return WriteIntVal(Q931RESTOUTPATH, *iNum);
	}
	else
	{
		return ReadIntVal(Q931RESTOUTPATH, iNum);
	}
	return 0;
}

int rwconnectTimeout(IN int *iNum, IN BOOL bWrite)
{
	if( bWrite )
	{
		return WriteIntVal(Q931CONTOUTPATH, *iNum);
	}
	else
	{
		return ReadIntVal(Q931CONTOUTPATH, iNum);
	}
	return 0;
}

// if fail, return 0.
// read of write gk address and port.
// dBuffer : gk address.
int rwgkIP(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	unsigned char tbuf[6];
	memset(tbuf, 0, 6);
	if( bWrite )
	{
		tbuf[0] = atoi(dBuffer);
		char *ttbuf = strstr(dBuffer, ".");
		tbuf[1] = atoi(++ttbuf);
		ttbuf = strstr(ttbuf, ".");
		tbuf[2] = atoi(++ttbuf);
		ttbuf = strstr(ttbuf, ".");
		tbuf[3] = atoi(++ttbuf);
		return WriteStrVal(GKADDRPATH, (char*)tbuf,4);
	}
	else
	{
		int iReadLength = 6;
		memset(dBuffer, 0, bufLength);
		int IPaddr=0;
		if( ReadStrVal(GKADDRPATH, (char*)&tbuf[0],&iReadLength))
		{
			wsprintf(dBuffer, "%d.%d.%d.%d", tbuf[0], tbuf[1],tbuf[2],tbuf[3]);
		}
		
	}
	return 0;
}

int	rwMulticastIP(IN OUT char * dBuffer, IN int bufLength, IN BOOL bWrite)
{
	unsigned char tbuf[6];
	memset(tbuf, 0, 6);
	if( bWrite )
	{
		tbuf[0] = atoi(dBuffer);
		char *ttbuf = strstr(dBuffer, ".");
		tbuf[1] = atoi(++ttbuf);
		ttbuf = strstr(ttbuf, ".");
		tbuf[2] = atoi(++ttbuf);
		ttbuf = strstr(ttbuf, ".");
		tbuf[3] = atoi(++ttbuf);
		return WriteStrVal(MIPPATH, (char*)tbuf,4);
	}
	else
	{
		int iReadLength = 6;
		memset(dBuffer, 0, bufLength);
		int IPaddr=0;
		if( ReadStrVal(MIPPATH, (char*)&tbuf[0],&iReadLength))
		{
			wsprintf(dBuffer, "%d.%d.%d.%d", tbuf[0], tbuf[1],tbuf[2],tbuf[3]);
		}
		
	}
	return 0;
}

int rwMulticastPort(IN OUT int *iPortNum,IN BOOL bWrite)
{
	if( bWrite )
	{
		return WriteIntVal(MPORTPATH, *iPortNum);
	}
	else
	{
		return ReadIntVal(MPORTPATH, iPortNum);
	}
	return 0;
}


int rwgkPort(IN OUT int *bufLength, IN BOOL bWrite)
{
	if( bWrite )
	{
		return WriteIntVal(GKPORTPATH, *bufLength);
	}
	else
	{
		return ReadIntVal(GKPORTPATH, bufLength);
	}
	return 0;
}


// read or write max call number.
// if fail, return 0.
int rwmaxUser(IN int *iCallNum, IN BOOL bWrite)
{
	int iSysMaxCall;
	int iSysMaxChannel;
	int iSysMaxRasTransactions;
	int iQ931MaxCall;
	if( bWrite && *iCallNum < 0)
		return 0;
	else if( bWrite && *iCallNum >= 0)
	{
		iSysMaxCall = SYSMAXCALL(*iCallNum);
		iSysMaxChannel = SYSMAXCHANNEL(*iCallNum);
		iSysMaxRasTransactions = *iCallNum;
		iQ931MaxCall = Q931MAXCALL(*iCallNum);
		if( !WriteIntVal(MAXCALPATH, iSysMaxCall) )
			return 0;
		if( !WriteIntVal(MAXCHAPATH, iSysMaxChannel))
			return 0;
		if( !WriteIntVal(MAXRASINPATH, iSysMaxRasTransactions))
			return 0;
		if( !WriteIntVal(Q931MAXCALLPATH, iQ931MaxCall))
			return 0;
	}
	
	if( ! bWrite )
	{
		if( !ReadIntVal(MAXCALPATH, &iSysMaxRasTransactions) )
		{
			return 0;
		}
		*iCallNum = (iSysMaxRasTransactions-5)/2;
	}
	return 1;
}

int WriteStrVal(IN char *sPath, IN char * dBuffer, IN int dLength )
{
	bSave = TRUE;
	return WriteVal(sPath, TRUE, dBuffer, dLength);
}

int WriteIntVal(IN char *sPath, IN int iValue )
{
	bSave = TRUE;
	return WriteVal(sPath, FALSE, NULL, iValue);
}

int WriteVal(IN char * sPath, IN BOOL bString, IN char * dBuffer, IN int iBuffer)
{
	IS_VALID_HCFG(hCfg, 0);
	if( ciSetValue(hCfg,sPath,bString,iBuffer,dBuffer) < 0 )
		return 0;
	return 1;
}

int	DeleteVal(IN char *sPath)
{
	IS_VALID_HCFG(hCfg, 0 );
	if( ciDeleteValue(hCfg, sPath) < 0 )
		return 0;
	return 1;
}

// if success, return real read data length.
int ReadStrVal(IN char * sPath, OUT char * dBuffer, IN int * dLength)
{
	char tBuffer[1024];
	int bufLen=1024;
	IS_VALID_HCFG(hCfg, 0);
	RvBool isString=0;
	if( ciGetValue(hCfg, sPath, &isString, &bufLen) < 0)
		return 0;
	if( !isString )
		return 0;
	if( (bufLen+2) > *dLength )
		return 0;
	if( ciGetString(hCfg, sPath, tBuffer,bufLen+2) < 0)
		return 0;
	memset( dBuffer, 0, *dLength);
	memcpy( dBuffer, tBuffer, bufLen);
	*dLength = bufLen;
	return 1;
}

// if success, return value is 1.
int ReadIntVal(IN char * sPath, OUT int * iValue)
{
	RvBool isString=FALSE;
	int	Value;
	int iStatus = 0;
	IS_VALID_HCFG(hCfg, 0);
	if( ciGetValue(hCfg,sPath,&isString,&Value) >= 0)
	{
		*iValue = Value;
		iStatus = 1;
	}
	else
		*iValue = 0;
	return iStatus;
}