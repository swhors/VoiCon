#include "stdafx.h"
#include "IOCTrl.h"
#include "ValInterface.h"
#include "FileInterface.h"

//#include "dummy.h"

#define MAX_ITEM	31

char * sKeyList[31] = 
{
	"gkIP",
	"gkPort",
	"maxUser",
	"localRPort",
	"localQPort",
	"reponseTimeout",
	"connectTimeout",
	E1641,
	E1642,
	E1643,
	E1644,
	E1645,
	E1646,
	"h323Id",
	"EnableCallCouple",
	"InputWaitTime",
	"InputInitWaitTime",
	"MentInterval",
	"AutoStart",
	"FastStart",
	"H245Tunneling",
	"rtpPort",
	MULTICASTIP,
	MULTICASTPORT,
	E1641Del,
	E1642Del,
	E1643Del,
	E1644Del,
	E1645Del,
	E1646Del,
	"\n"
};

typedef struct _rwFunc
{
	void * pFunc;
} rwFunc;

rwFunc sFunc[MAX_ITEM] =
{
	{&rwgkIP},
	{&rwgkPort},
	{&rwmaxUser},
	{&rwlocalRPort},
	{&rwlocalQPort},
	{&rwreponseTimeout},
	{&rwconnectTimeout},
	{&rwe164Id1},
	{&rwe164Id2},
	{&rwe164Id3},
	{&rwe164Id4},
	{&rwe164Id5},
	{&rwe164Id6},
	{&rwh323Id},
	{&rwEnableCallCouple},
	{&rwInputWaitTime},
	{&rwInputInitWaitTime},
	{&rwMentInterval},
	{&rwAutoStart},
	{&rwFastStart},
	{&rwH245Tunneling},
	{&rwRtpPort},
	{&rwMulticastIP},
	{&rwMulticastPort},
	{&delE164Id1},
	{&delE164Id2},
	{&delE164Id3},
	{&delE164Id4},
	{&delE164Id5},
	{&delE164Id6},
	NULL
};

typedef int (intFunc)(int *, BOOL );
typedef int (charFunc)(char * , int , BOOL );

int rwConf(IN char * sKey, IN char * dBuffer, IN int *dLength, BOOL bChar , BOOL bWrite)
{
	if( !sKey )
		return 0;
	for(int i = 0 ; i< MAX_ITEM; i++)
	{
		if(!memcmp(sKeyList[i], sKey, strlen(sKey)))
		{
			if( !bChar )
			{
				if( !dLength)
					return 0;
				intFunc * pFunc = (intFunc*)sFunc[i].pFunc;
				return pFunc(dLength, bWrite);
			}
			else
			{
				if( !dBuffer )
					return 0;
				charFunc * pFunc = (charFunc*)sFunc[i].pFunc;
				return pFunc(dBuffer, *dLength, bWrite);
			}
		}
	}
	return 0;
}

int ConfRead(IN char *sKey, IN char *dBuffer, IN int *dLength, BOOL bChar)
{

	return rwConf(sKey, dBuffer, dLength, bChar , FALSE);
}

int ConfWrite(IN char * sKey, IN char * dBuffer, IN int *dLength, BOOL bChar )
{
#if 0
	{
		FILE *fp;
		fp = fopen("cf4.log", "a+t");
		if( fp )
		{
			fwrite(sKey, strlen(sKey), 1, fp);
			if( dBuffer )
			{
				fwrite(dBuffer,strlen(dBuffer), 1, fp);
				fwrite("\n", 1, 1, fp);
			}else
				fwrite("NULL buffer\n",12, 1, fp); 
			fclose(fp);
		}
	}
#endif
	return rwConf(sKey, dBuffer, dLength, bChar , TRUE);
}