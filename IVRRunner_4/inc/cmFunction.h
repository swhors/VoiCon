#pragma once

#include "rvcommon.h"

#define UINT32	unsigned int
#define UINT16	unsigned short
#define INT8	signed char

int getFreeCall();

int RVCALLCONV cmEvNewCall( 
	IN      HAPP            hApp,
	IN      HCALL           hsCall,
	IN      LPHAPPCALL      lphaCall);

int RVCALLCONV cmEvRegEvent(
	IN HAPP hApp, IN cmRegState regState,
	IN cmRegEvent regEvent, IN int regEventHandle);
int     RVCALLCONV cmEvCallStateChanged(
                                      IN      HAPPCALL				haCall,
                                      IN      HCALL					hsCall,
                                      IN      cmCallState_e		state,
                                      IN      cmCallStateMode_e	stateMode);

int RVCALLCONV cmEvCallControlStateChanged(
		IN      HAPPCALL     haCall,
		IN      HCALL        hsCall,
		IN      cmControlState       state,
		IN      cmControlStateMode       stateMode);

int RVCALLCONV cmEvCallUserInfo(
                               IN	HAPPCALL haCall, 
                               IN	HCALL hsCall,
                               IN	UINT32 nodeId);

int RVCALLCONV cmEvCallNewRate( IN HAPPCALL haCall, IN HCALL hsCall, IN UINT32 rate);

int RVCALLCONV cmEvCallInfo( IN HAPPCALL haCall, IN HCALL hsCall, IN char* display, IN char* userUser, IN int userUserSize);

int RVCALLCONV cmEvCallFacility(
	IN HAPPCALL haCall,
	IN HCALL hsCall,
	IN int handle,
	OUT IN	RvBool *proceed);

int RVCALLCONV cmEvCallFastStartSetup(HAPPCALL haCall, HCALL hsCall,cmFastStartMessage *fsMessage);

int RVCALLCONV  cmEvCallUserInfo(
	IN	HAPPCALL haCall, 
	IN	HCALL hsCall,
	IN	int nodeId);
	
int RVCALLCONV  cmEvCallAdditionalAddress(
		IN	HAPPCALL haCall, 
		IN	HCALL hsCall,
		char	* address,
		unsigned int sendingComplete);

int RVCALLCONV  cmEvCallIncompleteAddress(
		IN	HAPPCALL haCall, 
		IN	HCALL hsCall);

int RVCALLCONV cmEvCallCapabilities(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN cmCapStruct* capabilities[]);
int RVCALLCONV cmEvCallCapabilitiesExt(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN cmCapStruct*** capabilities[]);
int RVCALLCONV cmEvCallCapabilitiesResponse(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN UINT32 status);
int RVCALLCONV cmEvCallMasterSlaveStatus(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN UINT32 status);
int RVCALLCONV cmEvCallUserInput(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN INT32 userInputId /* user input node id */
);
/*
typedef struct 
{
    cmEvRASRequestT cmEvRASRequest;
    cmEvRASConfirmT cmEvRASConfirm;
    cmEvRASRejectT  cmEvRASReject;
    cmEvRASTimeoutT cmEvRASTimeout;
} SCMRASEVENT,*CMRASEVENT;
*/

int RVCALLCONV cmEvRASRequest(
                            IN  HRAS             hsRas,
                            IN  HCALL            hsCall,
                            OUT LPHAPPRAS        lphaRas,
                            IN  cmRASTransaction transaction,
                            IN  cmRASTransport*  srcAddress, 
                            IN  HAPPCALL         haCall         OPTIONAL);



int RVCALLCONV cmEvRASConfirm(
                            IN  HAPPRAS          haRas,
                            IN  HRAS             hsRas);


int RVCALLCONV cmEvRASReject(
                           IN  HAPPRAS          haRas,
                           IN  HRAS             hsRas,
                           IN  cmRASReason      reason);


int RVCALLCONV cmEvRASTimeout(
                            IN  HAPPRAS          haRas,
                            IN  HRAS             hsRas);


int RVCALLCONV cmEvChannelNew(
	IN HAPPCALL haCall,
	IN HCALL hsCall,
	IN HCHAN hsChannel,
	OUT LPHAPPCHAN lphaChannel);

int  RVCALLCONV cmEvCallControlStateChanged(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN cmControlState state,
		IN cmControlStateMode stateMode);

int RVCALLCONV cmEvCallMasterSlave(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN UINT32 terminalType,
		IN UINT32 statusDeterminationNumber);

int RVCALLCONV cmEvChannelStateChanged(
		IN      HAPPCHAN        haChannel,
		IN      HCHAN           hsChannel,
		IN      cmChannelState_e    state,
		IN      cmChannelStateMode_e stateMode);

int     RVCALLCONV cmEvChannelNewRate(
		IN      HAPPCHAN        haChan,
		IN      HCHAN           hsChan,
		IN      UINT32          rate);

int RVCALLCONV cmEvChannelMaxSkew(
		IN HAPPCHAN	haChan1,
		IN HCHAN		hsChan1,
		IN HAPPCHAN	haChan2,
		IN HCHAN		hsChan2,
		IN UINT32		skew);

int RVCALLCONV cmEvChannelSetAddress(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan,
		IN unsigned int        ip,
		IN unsigned short      port);

int RVCALLCONV cmEvChannelSetRTCPAddress(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan,
		IN UINT32	ip,
		IN UINT16	port);

int RVCALLCONV cmEvChannelRTPDynamicPayloadType(
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN INT8			dynamicPayloadType);

int RVCALLCONV cmEvChannelVideoFastUpdatePicture(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan);

int RVCALLCONV cmEvChannelVideoFastUpdateGOB(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan,
		IN int		firstGOB,
		IN int		numberOfGOBs);
int RVCALLCONV cmEvChannelVideoFastUpdateMB(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan,
		IN int		firstGOB,
		IN int		firstMB,
		IN int		numberOfMBs);
int RVCALLCONV cmEvChannelRequestCloseStatus(
		IN HAPPCHAN              haChan,
		IN HCHAN                 hsChan,
		IN cmRequestCloseStatus  status);

int RVCALLCONV cmEvChannelMediaLoopStatus(
		IN HAPPCHAN haChan,
		IN HCHAN hsChan,
		IN cmMediaLoopStatus status);

int RVCALLCONV cmEvChannelParameters(
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN char*		channelName,
		IN HAPPCHAN		haChanSameSession,
		IN HCHAN		hsChanSameSession,
		IN HAPPCHAN		haChanAssociated,
		IN HCHAN		hsChanAssociated,
		IN UINT32		rate);

int RVCALLCONV cmEvChannelHandle(
		/* provide data type tree handle for this channel */
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN int			dataTypeHandle,
		IN cmCapDataType	dataType);

int RVCALLCONV cmEvChannelRequestCloseStatus(
		IN HAPPCHAN              haChan,
		IN HCHAN                 hsChan,
		IN cmRequestCloseStatus  status);

int RVCALLCONV  cmEvChannelReplace(
		IN 	HAPPCHAN	haChan,
		IN 	HCHAN		hsChan,
		IN 	HAPPCHAN	haReplacedChannel,
		IN 	HCHAN		hsReplacedChannel );

int RVCALLCONV  cmEvChannelFlowControlToZero(
		IN HAPPCHAN		haChan,
		IN  HCHAN		hsChan);

int RVCALLCONV cmEvChannelMiscCommand(
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN cmMiscellaneousCommand miscCommand);

int RVCALLCONV cmEvChannelTransportCapInd(
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN int nodeId );

int RVCALLCONV cmEvChannelSetNSAPAddress(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan,
		IN char*	address,
		IN int		length,
		IN RvBool		multicast);

int RVCALLCONV cmEvChannelSetATMVC(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan,
		IN int		portNumber);

// HOOK Function
RvBool RVCALLCONV cmHookListen(
		IN HPROTCONN hConn,
		IN int nodeId );
int RVCALLCONV cmHookListening(
		IN HPROTCONN hConn,
		IN int nodeId,
		IN unsigned int error );
int RVCALLCONV cmHookConnecting(
		IN HPROTCONN hConn,
		IN int nodeId);
int RVCALLCONV cmHookInConn(
		IN HPROTCONN hConn,
		IN int nodeIdFrom,
		IN int nodeIdTo );
int RVCALLCONV cmHookOutConn(
		IN HPROTCONN hConn, IN int nodeIdFrom,
		IN int nodeIdTo, IN RvBool error );
RvBool RVCALLCONV cmHookSend(
		IN HPROTCONN hConn,
		IN int nodeId,
		IN RvBool error );
RvBool RVCALLCONV cmHookRecv(
		IN HPROTCONN hConn,
		IN int nodeId,
		IN RvBool error );
#if 0
RvBool RVCALLCONV cmHookSendTo(
		IN HPROTCONN	hConn,
		IN int			nodeId,
		IN int			nodeIdTo,
		IN RvBool			error );
#else
RvBool RVCALLCONV cmHookSendTo(
                      IN      HPROTCONN           hConn,
                      IN      int                 nodeId,
                      IN      int                 nodeIdTo,
                      IN      RvBool                error
                      );
#endif
RvBool RVCALLCONV cmHookRecvFrom( IN HPROTCONN hConn,
		IN int nodeId,
		IN int	nodeIdFrom,
		IN RvBool	multicast,
		IN RvBool	error );
void RVCALLCONV cmHookClose( IN HPROTCONN hConn );