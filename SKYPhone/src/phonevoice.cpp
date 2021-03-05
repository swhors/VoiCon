//////////////////////////////////////////////////////////////////
// phonevoice.cpp
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// start of include or global definition
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "phonevoice.h"
#include "usermsg.h"
#include "mediathread.h"
#include "G729Codec.h"

extern CG729Codec *gEncode729Codec;

//////////////////////////////////////////////////////////////////
// start of class implementation
//////////////////////////////////////////////////////////////////

CPhoneVoice::CPhoneVoice()
{
	codec_type = 0;
	cur_length = 0;
	memset( send_buff, 0, 21);
}

CPhoneVoice::~CPhoneVoice()
{
}

void CPhoneVoice::OnBuffered(unsigned char *buffer,int size)
{
	switch(codec_type)
	{
	case 1:
		OnG723Buffered(buffer,size);
		break;
	case 2:
		OnG729Buffered(buffer,size);
		break;
	default:
		break;
	}
}

int CPhoneVoice::OpenDevice(int dir)
{
	switch(codec_type)
	{
	case 1: // g723_1
		return open(dir, 16, 8000, 480);
	case 2: // g729_a
		return open(dir, 16, 8000, 160);
	default:
		return 0;
	}
}

void CPhoneVoice::OnG723Buffered(unsigned char *buffer,int size)
{
	int len;

	if( gDTMFCnt )
	{
		len = gEncodeCodec->Encode((unsigned char*)(gDTMFbuf[4-gDTMFCnt]), 480);
		gDTMFCnt--;
	}
	else
	{
		len = gEncodeCodec->Encode(buffer, size);
	}
	if ( gMediaWnd != NULL )
		::SendMessage(gMediaWnd, UM_SENDMEDIA, (DWORD)gEncodeCodec->dataBuffer, len);
	Sleep(30);
}

void CPhoneVoice::OnG729Buffered(unsigned char *buffer,int size)
{
	unsigned char *coded_buf=NULL;
	coded_buf = &send_buff[0];
	if( gDTMFCnt )
	{
		if( gEncode729Codec )
			gEncode729Codec->Encode((unsigned char*)(gDTMFbuf[4-gDTMFCnt]), 80);
		gDTMFCnt--;
	}
	else
	{
		gEncode729Codec->Encode(buffer, size);
		memcpy(coded_buf+cur_length,gEncode729Codec->dataBuffer,10);
		cur_length += 10;
	}
    
	if( gMediaWnd != NULL )
	{
		if( cur_length == 20)
		{
			if( gEncode729Codec )
				::SendMessage(gMediaWnd, UM_SENDMEDIA, (DWORD)&send_buff[0], cur_length);
			cur_length = 0;
			Sleep(16);
		}
	}
}

//////////////////////////////////////////////////////////////////
// end of file
//////////////////////////////////////////////////////////////////
