#include "stdafx.h"
#include "audiointerface.h"
#include "usermsg.h"
#include "mediathread.h"
#include "g711.h"
#include "G711Codec.h"
#include "G729Codec.h"
#include "G7231Codec.h"

extern BOOL bplay1;

extern unsigned int gUsedCodec;

extern CG729Codec *gDecode729Codec, *gEncode729Codec;
extern CACMCodec *gDecodeCodec, *gEncodeCodec;
extern CG711Codec *gDecodec711Codec, *gEncode711Codec;
extern CG7231Codec *gDecodeG7231Codec, *gEncodeG7231Codec;

void send_media(void *rData, int len, int type, int opt);

CAudioInterface::CAudioInterface(void)
{
	cur_length = 0;
	memset( send_buff, 0, 21);
}

CAudioInterface::~CAudioInterface(void)
{
}
void CAudioInterface::OnBuffered(unsigned char *buffer, int size,int ttl/*=0*/)
{
    switch(gUsedCodec)
	{
	case 1:
		OnBufferedG723(buffer, size, ttl);
		break;
	case 2:
	case 3:
		OnBufferedG711(buffer, size, ttl);
		break;
	case 4:
	default:
		OnBufferedG729(buffer, size, ttl);
		break;
	}
}

void CAudioInterface::OnBufferedG729(unsigned char *buffer, int size, int ttl/*=0*/)
{
	unsigned char *coded_buf=NULL;
	coded_buf = &send_buff[0];
	{
	    gEncode729Codec->Encode(buffer, size);
		memcpy(coded_buf+cur_length,gEncode729Codec->dataBuffer,10);
		cur_length += 10;
	}
    
	if( gMediaWnd != NULL )
	{
		if( cur_length == 20)
		{
			send_media(&send_buff[0], cur_length, gUsedCodec, 0);
			cur_length = 0;
			if( ttl )
			    Sleep(16);
		}
	}
}

void CAudioInterface::OnBufferedG711(unsigned char *buffer, int size, int ttl/*=0*/)
{
	int ret;
    ret = gEncode711Codec->Encode(buffer, size);
	if( ret > 0 )
	{
		send_media(gEncode711Codec->dataBuffer, 160, gUsedCodec, 0);
		if( ttl )
            Sleep(16);
	}
}
void CAudioInterface::OnBufferedG723(unsigned char *buffer, int size, int ttl/*=0*/)
{
	int len;
	{
		len = gEncodeG7231Codec->Encode(buffer, size);
	}
	if( len > 0 )
	{
	    send_media(gEncodeG7231Codec->dataBuffer, len, gUsedCodec, 0);
	    if( ttl )
	    {
            Sleep(30);
	    }
	}
}

void CAudioInterface::set_g729()
{
	cur_length = 0;
	memset( send_buff, 0, 21);
	codecType = 4;
}
void CAudioInterface::set_g723()
{
	codecType = 1;
}
void CAudioInterface::set_g711u()
{
	codecType = 2;
}
void CAudioInterface::set_g711a()
{
	codecType = 3;
}

int CAudioInterface::OutDevOpen(int type)
{
	selectOutDev("USB Audio Device");
	switch(type)
	{
	case 1:
		return DevOpenG723(1, 1);
	case 2:
	case 3:
		return DevOpenG711(type, 1);
	case 4:
	default:
		return DevOpenG729(2, 1);
		break;
	}
    return 0;
}
int CAudioInterface::InDevOpen(int type)
{
	selectOutDev("USB Audio Device");
	switch(type)
	{
	case 1:
		return DevOpenG723();
	case 2:
	case 3:
		return DevOpenG711(type);
	case 4:
		return DevOpenG729();
	default:
		break;
	}
    return 0;
}

int CAudioInterface::DevOpenG723(int type, int dir)
{
	set_g723();
    return open(dir, 16, 8000, 480);
}
int CAudioInterface::DevOpenG729(int type, int dir)
{
	set_g729();
	if( dir == 0 )
	    return open(dir, 16, 8000, 160);
	return open(dir, 16, 8000, 320);
}
int CAudioInterface::DevOpenG711(int type, int dir)
{
	if( type == 3)
	    set_g711u();
	else if( type == 4)
        set_g711a();
	else
		return 0;
	return open(dir, 16, 8000, 320);
}

void CAudioInterface::OnDataPlay(unsigned char *buffer, int size )
{
	switch(gUsedCodec)
	{
	case 1:
		OnDataPlayG723(buffer, size);
		break;
	case 2:
	case 3:
        OnDataPlayG729(buffer, size);
		break;
	case 4:
		OnDataPlayG711(buffer, size);
		break;
	default:
		break;
	}
}
void CAudioInterface::OnDataPlayG729(unsigned char *buffer, int size )
{
	int rtpCnt,decodelen;
	if( size > 20 )
	{
		rtpCnt = size / 20;
		for (int i = 0; i < rtpCnt; i++)
		{
			decodelen = 160;
			if( gDecode729Codec && bplay1 )
				gDecode729Codec->Decode((buffer+(20*i) ), 10, gDecode729Codec->dataBuffer, &decodelen);
			if( gDecode729Codec && bplay1 )
				gDecode729Codec->Decode((buffer + 10+(20*i)), 10,gDecode729Codec->dataBuffer+160, &decodelen);
			if( gDecode729Codec && bplay1 )
				waveOut(gDecode729Codec->dataBuffer);
		}
	}
	else
	{
		if( size == 20 )
		{
			decodelen = 160;
			if( gDecode729Codec && bplay1 )
				gDecode729Codec->Decode(buffer , 10,
				gDecode729Codec->dataBuffer, &decodelen);
			if( gDecode729Codec && bplay1 )
				gDecode729Codec->Decode(buffer + 10, 10,
				gDecode729Codec->dataBuffer+160, &decodelen);
			if( gDecode729Codec && bplay1 )
				waveOut(gDecode729Codec->dataBuffer);
		}
	}

}

void CAudioInterface::OnDataPlayG711(unsigned char *buffer, int size )
{
	int rtpCnt;
	if( size > 160 )
	{
		rtpCnt = size / 160;
		for (int i = 0; i < rtpCnt; i++)
		{
			if( gDecodec711Codec->Decode((buffer+(160*i)),160) > 0 )
			{
			    if( gDecodec711Codec && bplay1 )
				    waveOut(gDecodec711Codec->dataBuffer);
			}
		}
	}
	else
	{
		if( size != 160 )
			return;
		if( gDecodec711Codec && bplay1 )
		{
			if( gDecodec711Codec->Decode(buffer,160) > 0 )
			{
			    if( gDecodec711Codec && bplay1 )
				    waveOut(gDecodec711Codec->dataBuffer);
			}
		}
	}
}
void CAudioInterface::OnDataPlayG723(unsigned char *buffer, int size )
{
	int repeatCnt,decodelen=24;
	if(size> 24)
	{
	    repeatCnt = size / 24;
		for (int i = 0; i < repeatCnt; i++)
		{
			if( gDecodeG7231Codec && bplay1 )
			    gDecodeG7231Codec->Decode(buffer + (24 * i), 24, &decodelen);
			if( bplay1 )
			    waveOut(gDecodeG7231Codec->dataBuffer);
		}
	}
	else
	{
	    if( gDecodeG7231Codec && bplay1 )
            gDecodeG7231Codec->Decode(buffer, 24, &decodelen);
        if( gDecodeG7231Codec && bplay1 )
            waveOut(gDecodeG7231Codec->dataBuffer);
    }
}

//////////////////////////////////////////////////////////////////
