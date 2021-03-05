#pragma once

#ifdef __cplusplus
extern "C"{
#endif

#ifndef __SKY_RTP_H
#define __SKY_RTP_H


#define RTP_OUT_DEF_PORT_START 4000
#define RTP_IN_DEF_PORT_START	7000
#define RTP_RTCP_GAP	3000

#include "rtp_str.h"

typedef rtp_hdr_t rtp_t,*p_rtp_t;

typedef rtcp_t *p_rtcp_t;

typedef enum
{
    PCMU  =  0,
    G7231 =  4,
    PCMA  =  8,
    G722  =  9,
    G728  = 15,
    G729  = 18,
    H261  = 31,
    H263  = 34

} payload_e;


typedef struct _rtp_proxy_
{
	char			indesip[24];
	char			outdesip[24];
	int				indesport;
	int				outdesport;
	int				inRtpPort;
	int				inRtcpPort;
	int				outRtpPort;
	int				outRtcpPort;
	int				inRtpHandle;
	int				inRtcpHandle;
	int				outRtpHandle;
	int				outRtcpHandle;
	long			rtpStamp;
	long			rtpTime;
	unsigned int	inssrc;
	unsigned int	outssrc;
	unsigned int	seq;
	FILE			*infp;
	FILE			*outfp;
}rtp_proxy_buf_t,*p_rtp_proxy_buf_t;

void enable_rtp_manager_flag();
void disable_rtp_manager_flag();

void free_rtp_buff();

int RtpPlay(int iCodec, 
			CONNECTION &pCon, 
			char * szFileName, 
			char * sLanguage, 
			int & iState,
			int iSkipState, 
			int &iReserved);

int reset_rtp_info(int iCount);
//int reset_rtp_info(char * se164, char *de164);
int set_rtp_in(char *ip, int port, int call_num);
int set_rtp_out(char *ip, int port, int call_num,int iCodec);

int start_rtp_manager(int u_num, int start_port);

#endif

#ifdef __cplusplus
}
#endif
