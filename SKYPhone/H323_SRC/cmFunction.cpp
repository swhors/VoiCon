#include "stdafx.h"

#include "cm.h"

#include "resource.h"

#include "UbiDialer.h"
#include "UbiDialerDlg.h"

#include "netutils.h"
#include "H323CallInfo.h"
#include "RadMiscFunction.h"

extern void RTPChannelOpen(int callNum);

extern HAPP hApp;

// first enter
//
//SCMEVENT
//새로운 콜 연결이 들어 오면 발생 됨.
//
int RVCALLCONV cmEvNewCall(
		IN HAPP			hApp,
		IN HCALL		hsCall,
		OUT LPHAPPCALL	lphaCall)
{
//	WriteLog(PROG_NAME, -1, "NewCall", "enter");
	if( !hApp )
		return ERROR;
	if( !hsCall)
		return ERROR;
	int iCallNum = OnEvNewCall(hsCall, lphaCall);
	if( iCallNum == -2 )
		return ERROR;
	//cmCallSetParam(hsCall,cmParamEstablishH245,1,0,NULL);
	return 0;
}

//
//SCMEVENT
// GateKeeper에 등록하거나, 등록을 해제 할 경우의 이벤트 처리. 
//
int RVCALLCONV cmEvRegEvent(
		IN HAPP hApp, IN cmRegState regState,
		IN cmRegEvent regEvent, IN int regEventHandle)
{
	OnEvRegEvent(regState, regEvent, regEventHandle);
    return 0;
}

int RVCALLCONV cmEvCallUserInfo(
		IN	HAPPCALL haCall, 
		IN	HCALL hsCall,
		IN	int nodeId)
{
	char display[78];
	char tel[32];
	cmNonStandardParam param;
	CH323CallInfo *s = (CH323CallInfo*)haCall;
	//CONNECTION * s = (CONNECTION * )haCall;
	//call* s=(call*)haCall;
	cmCallUserInformationGet( hApp, nodeId,display,78,&param);
	if (cmCallGetOrigin(s->hsCall,NULL) )
	{
		cmRASTransport tr;
		int root;
		cmGetLocalCallSignalAddress(hApp,&tr);
		sprintf(tel,"%d.%d.%d.%d:%d",
			(int)((unsigned char*)&(tr.ip))[0],(int)((unsigned char*)&(tr.ip))[1],
			(int)((unsigned char*)&(tr.ip))[2],(int)((unsigned char*)&(tr.ip))[3],tr.port);
		root =cmCallUserInformationCreate(s->hsCall,tel,NULL);
		cmCallUserInformationSend(s->hsCall,root);
	}
	return 0;
}

//SCMCALLEVENT
int	RVCALLCONV cmEvCallStateChanged(
		IN	HAPPCALL	haCall,
		IN	HCALL		hsCall,
		IN	cmCallState_e	state,
		IN	cmCallStateMode_e	stateMode)
{
	CH323CallInfo *s = (CH323CallInfo *)haCall;
	
	if( !s )
	{
		if( hsCall )
			cmCallClose(hsCall);
		return ERROR;
	}
	
	if( !hsCall)
	{
		//WriteLog("Error", -1, "ERR", "Error Call Idle [1]");
		return ERROR;
	}
	if( s->hsCall != hsCall)
	{
		//WriteLog("Error", -1, "ERR", "Error Call Idle [1]");
		return ERROR;
	}
	if( OnEvCallStateChanged(s, hsCall, state, stateMode) != 0)
	{
		//WriteLog("Error", -1, "ERR", "Error Call Idle [1]");
		return ERROR;
	}
	return 0;
	//return 0;
}
/*
Fast Connect or Fast Start is a new procedure added to H.323 version II.
It reduces the number of round trips to one by combining the call connect
procedure into a single H.225 transaction. The design of this procedure
is fully compatible with the original multi-step procedure.
*/
int RVCALLCONV
cmEvCallFastStartSetup(HAPPCALL haCall, HCALL hsCall,cmFastStartMessage *fsMessage)
{
	cmTransportAddress rtp;
	cmTransportAddress rtcp;
	CH323CallInfo *s = (CH323CallInfo *)haCall;
	CUbiDialerDlg *pDlg = (CUbiDialerDlg *)AfxGetApp()->m_pMainWnd;
	CCallConf *pConf = (CCallConf *)&pDlg->m_CallConf;
	if( !s )
	{
		if( hsCall )
			cmCallClose(hsCall);
		return ERROR;
	}
	int cNum=0;
	int j,k;
	int i;
	if( !s->m_iFastStart )
		return 0;
	//WRITEL6OG(PROG_NAME, s->iCallNumber, "Q931", "FastStart Setup");
	if( !s->hsCall) return ERROR;
	if( s->hsCall != hsCall) return ERROR;
	for (i=0;i<fsMessage->partnerChannelsNum;i++)
	{
		switch(fsMessage->partnerChannels[i].type)
		{
		case cmCapAudio:
			{
				rtp.port=pConf->m_InitOfRtpPort+cNum*20;
				rtp.ip = 0;
				rtcp.port = pConf->m_InitOfRtpPort+1+cNum*20;
				rtcp.ip = 0;
				for (j=0;j<fsMessage->partnerChannels[i].transmit.altChannelNumber;j++)
					for (k=0;k<fsMessage->partnerChannels[i].receive.altChannelNumber;k++)
					{
						BOOL bEqual = FALSE;
						if (!strcmp(fsMessage->partnerChannels[i].receive.channels[k].channelName,
							fsMessage->partnerChannels[i].transmit.channels[j].channelName))
						{
							for( int b = 0; b < 4; b++)
							{
								char tbuf[64],sPath[256];
								int iNodeID;
								HPVT hVal=cmGetValTree(hApp);
								UINT handleH245 = cmGetH245ConfigurationHandle(hApp);
								wsprintf(sPath, "channels.%d.name", b+1);
								iNodeID = pvtGetNodeIdByPath(hVal, handleH245, sPath);
								if( iNodeID > 0)
								{
									pvtGetString(hVal, iNodeID, 64,tbuf);
									char *lStr1, *lStr2;
									lStr1 = strlwr(tbuf);
									lStr2 = strlwr(fsMessage->partnerChannels[i].transmit.channels[j].channelName);
									if( lStr1 && lStr2 )
									{
										if( !memcmp(lStr2, lStr1, strlen(lStr1)))
											bEqual = TRUE;
										if( !memcmp(lStr2, "g7231", 4))
										{
											int RemoteCapaPVT = cmCallGetRemoteCapabilities(hsCall);
											
											//if( iIvrDefaultSilenceSupression )
											{
												/*
												cmEvCallFastStartSetup(HAPPCALL haCall, HCALL hsCall,cmFastStartMessage *fsMessage);
												*/
											}
										}
									}
								}
							}
							if( bEqual )
							{
								fsMessage->partnerChannels[i].transmit.channels[k].rtp = rtp;
								fsMessage->partnerChannels[i].transmit.channels[k].rtcp = rtcp;
								fsMessage->partnerChannels[i].receive.channels[j].rtp = rtp;
								fsMessage->partnerChannels[i].receive.channels[j].rtcp = rtcp;
								cmFastStartChannelsAck(hsCall,&(fsMessage->partnerChannels[i].transmit.channels[j]));
								cmFastStartChannelsAck(hsCall,&(fsMessage->partnerChannels[i].receive.channels[k]));
								j=100;
								break;
							}
						}
					}
			}
			break;
		case cmCapVideo:
			{
				rtp.port=pConf->m_InitOfRtpPort+2 +cNum*20;
				rtp.ip = 0;
				rtcp.port = pConf->m_InitOfRtpPort+3 +cNum*20;
				rtcp.ip = 0;
				for (j=0;j<fsMessage->partnerChannels[i].transmit.altChannelNumber;j++)
					for (k=0;k<fsMessage->partnerChannels[i].receive.altChannelNumber;k++)
					{
						if (!strcmp(fsMessage->partnerChannels[i].receive.channels[k].channelName,
							fsMessage->partnerChannels[i].transmit.channels[j].channelName))
						{
							cmFastStartChannelsAck(hsCall,&(fsMessage->partnerChannels[i].transmit.channels[j]));
							cmFastStartChannelsAck(hsCall,&(fsMessage->partnerChannels[i].receive.channels[k]));
							j=100;
							break;
						}
					}
			}
			break;
		}
	}
	cmFastStartChannelsReady( hsCall);
	return TRUE;
}

//SCMCALLEVENT
int RVCALLCONV  cmEvCallAdditionalAddress(
		IN	HAPPCALL haCall, 
		IN	HCALL hsCall,
		char	* address,
		RvBool	sendingComplete)
{
	CH323CallInfo *s = (CH323CallInfo *)haCall;
	if( !s )
	{
		if( hsCall )
			cmCallClose(hsCall);
		return ERROR;
	}
	s->overlapState.sendingComplete = sendingComplete;
	if( ( address != NULL ) && ((
		strlen(s->overlapState.address) + strlen( address ) ) 
		< sizeof( s->overlapState.address) - 1 ))
		strcat( s->overlapState.address, address );
	return 0;
}

int RVCALLCONV cmEvCallControlStateChanged(
		IN      HAPPCALL           haCall,
		IN      HCALL		       hsCall,
		IN      cmControlState     state,
		IN      cmControlStateMode stateMode)
{
	return 0;
}

int RVCALLCONV cmEvCallFacility(
                          IN      HAPPCALL            haCall,
                          IN      HCALL               hsCall,
                          IN      int                 handle,
                          OUT IN	RvBool		    *proceed)
{
// modified 09/08/07
	cmNonStandardParam nsParam= {{"1 1 1 1",7}, 2, "yu"};
	cmCallSetParam(hsCall,cmParamSetupNonStandard,0,
			sizeof(cmNonStandardParam),(char*)&nsParam);
	*proceed=1;
	return 0;
}

int RVCALLCONV  cmEvCallIncompleteAddress(
		IN	HAPPCALL haCall, 
		IN	HCALL hsCall)
{
	CH323CallInfo *s = (CH323CallInfo *)haCall;
	if( !s )
	{
		if( hsCall )
			cmCallClose(hsCall);
		return ERROR;
	}
	s->overlapState.running = (overlapStateT)1;
	s->overlapState.sendingComplete = 0;
	s->overlapState.address[0] = 0;
	return 0;
}

/*
typedef struct
{
char* name;
INT32 capabilityId;
int capabilityHandle;
cmCapDirection direction;
cmCapDataType type;
} cmCapStruct;
*/
//#define __CAPA_DEBUG
int RVCALLCONV
cmEvCallCapabilities(
                     IN         HAPPCALL        haCall,
                     IN         HCALL           hsCall,
                     IN         cmCapStruct*    capabilities[])
{
	//if( !s->bOpendChannel)
	//{
	//	RTPChannelOpen(s->iCallNumber);
	//}
    return TRUE;
}

int RVCALLCONV
cmEvCallCapabilitiesExt(
                        IN      HAPPCALL        haCall,
                        IN      HCALL           hsCall,
                        IN      cmCapStruct***  capabilities[])
{
//	if( !s->bOpendChannel )
//	RTPChannelOpen(s->iCallNumber);
	return TRUE;
}

/*
*/

int RVCALLCONV cmEvCallMasterSlaveStatus(
		IN HAPPCALL	haCall,
		IN HCALL	hsCall,
		IN UINT32	status)
{
    return 0;
}

//SCMCONTROLEVENT
int RVCALLCONV cmEvCallMasterSlave(
		IN HAPPCALL haCall,
		IN HCALL hsCall,
		IN UINT32 terminalType,
		IN UINT32 statusDeterminationNumber)
{
	//CONNECTION * s = (CONNECTION * ) haCall;
	//if( s && ((int)s!=0xffffffff) && ((int)s != 0xdddddddd) )
	//	s->iCallMaster = terminalType;

	cmCallMasterSlaveDetermineExt(hsCall,terminalType,rand());
	return 0;
}

//SCMCONTROLEVENT
int RVCALLCONV cmEvCallUserInput(
		IN  HAPPCALL haCall,
		IN  HCALL    hsCall,
		IN  INT32    userInputId /* user input node id */
)
{
	CH323CallInfo *s=(CH323CallInfo*)haCall;
	if( !s )
	{
		if( hsCall )
			cmCallClose(hsCall);
		return ERROR;
	}
	if( (hsCall == s->hsCall) && hsCall != NULL)
	{
		OnUserInput(s, hsCall,userInputId);
	}
	return 0;
}

//SCMCONTROLEVENT
int RVCALLCONV cmEvChannelNew(
	IN HAPPCALL haCall,
	IN HCALL hsCall,
	IN HCHAN hsChannel,
	OUT LPHAPPCHAN lphaChannel)
{
	CH323CallInfo *s=(CH323CallInfo*)haCall;
	if( !s )
	{
		if( hsCall )
			cmCallClose(hsCall);
		return ERROR;
	}
	return OnChannelNew(s, hsCall, hsChannel, lphaChannel);
}

//SCMCHANEVENT
int RVCALLCONV cmEvChannelStateChanged(
		IN      HAPPCHAN        haChannel,
		IN      HCHAN           hsChannel,
		IN      cmChannelState_e    state,
		IN      cmChannelStateMode_e  stateMode)
{
	channel * c = (channel*)haChannel;
	if (!c) return ERROR;
	CH323CallInfo *s=(CH323CallInfo*)c->haCall;
	if( !s && ((int)s==0xdddddddd) && ((int)s == 0xffffffff))
	{
		return ERROR;
	}
	OnEvChannelStateChanged( s, haChannel, hsChannel, state, stateMode);
	return  0;
}

//SCMCHANEVENT
int     RVCALLCONV cmEvChannelNewRate(
		IN      HAPPCHAN        haChan,
		IN      HCHAN           hsChan,
		IN      UINT32          rate)
{
    channel* c=(channel*)haChan;
	if( !c )
		return ERROR;
	CH323CallInfo *s=(CH323CallInfo*)c->haCall;
	if( !s && ((int)s==0xdddddddd) && ((int)s == 0xffffffff))
	{
		return ERROR;
	}
	OnChannelNewRate(s, c, hsChan, rate);
	return 0;
}

//SCMCHANEVENT
int RVCALLCONV cmEvChannelSetRTCPAddress(
		IN HAPPCHAN	haChan,
		IN HCHAN	hsChan,
		IN UINT32	ip,
		IN UINT16	port)
{
    channel* c=(channel*)haChan;
	if (!c)
		return ERROR;
	if( !c->haCall )
		return ERROR;
	CH323CallInfo *s=(CH323CallInfo*)c->haCall;
	if( !s)
		return ERROR;
	//lRtpSetRtcp(c->sNum, ip, port);
	return 0;
}

//SCMCHANEVENT
int RVCALLCONV cmEvChannelRTPDynamicPayloadType(
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN RvInt8			dynamicPayloadType)
{
	channel* c=(channel*)haChan;
	if( !c)
		return ERROR;
	c->dpt=dynamicPayloadType;
	return 0;
}
//SCMCHANEVENT
int RVCALLCONV cmEvChannelParameters(
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN char*		channelName,
		IN HAPPCHAN		haChanSameSession,
		IN HCHAN		hsChanSameSession,
		IN HAPPCHAN		haChanAssociated,
		IN HCHAN		hsChanAssociated,
		IN UINT32		rate)
{
	channel* c=(channel*)haChan;
	if (!c) return ERROR;
	if (channelName)
	{
		CH323CallInfo *s = (CH323CallInfo*)c->haCall;
		if( !s )
			return ERROR;
		if( memcmp( channelName, "g711Ulaw", 8) == 0)
		{
			s->iChannelCodec = 1;
		}
		else if( memcmp( channelName, "g711Alaw", 8) == 0)
		{
			s->iChannelCodec = 2;
		}
		else if( memcmp( channelName, "g7231", 5) == 0)
		{
			s->iChannelCodec = 0;
		}
		strcpy(c->channelName,channelName);
    }
	//int ret = cmChannelGetDependency(hsChan,&haBase,&hsBase);
	return 1;
}

//SCMCHANEVENT
int RVCALLCONV cmEvChannelHandle(
		/* provide data type tree handle for this channel */
		IN HAPPCHAN		haChan,
		IN HCHAN		hsChan,
		IN int			dataTypeHandle,
		IN cmCapDataType	dataType)
{
	channel* c=(channel*)haChan;
	if (!c) return ERROR;
	c->handle=dataTypeHandle;
	return TRUE;
}

//SCMCHANEVENT
int RVCALLCONV cmEvChannelRequestCloseStatus(
		IN HAPPCHAN              haChan,
		IN HCHAN                 hsChan,
		IN cmRequestCloseStatus  status)
{
    cmCloseLcnReason reason;
    cmQosCapability  cmQOSCapability;
	if (!haChan) return ERROR;
	if (status==cmRequestCloseRequest)
	{
		cmChannelAnswer(hsChan);
        cmChannelGetRequestCloseParam(	hsChan,&reason,&cmQOSCapability);
		cmChannelDrop(hsChan);
	}
	return 0;
}

int RVCALLCONV	cmEvChannelMediaLoopStatus( 
		IN HAPPCHAN				haChan, 
		IN HCHAN				hsChan, 
		IN cmMediaLoopStatus	status)
{
	channel* c=(channel*)haChan;
	if( !c ) return ERROR;
	switch(status)
	{
	case cmMediaLoopRequest:
		cmChannelMediaLoopConfirm(hsChan);
		break;
	case cmMediaLoopOff:
		break;
	}
	return 0;
}

extern int startPort;
//SCMCHANEVENT
int RVCALLCONV cmEvChannelSetAddress(
	IN HAPPCHAN            haChan,
	IN HCHAN               hsChan,
	IN UINT32              ip,
	IN UINT16              port)
{
	channel* c=(channel*)haChan;
	if (!c) return ERROR;
	CH323CallInfo *s=(CH323CallInfo*)c->haCall;
	if( !s )
		return ERROR;
	//if( port > 0 )
	//	lRtpSetRtp(c->sNum, ip, port);
	return 0;
}

/////////////////////////////////////////////////////////
//
// Hooking Function 
//
/////////////////////////////////////////////////////////
//
//	SCMPROTOCOLEVENT cmProtocolEvent={cmHookListen,
//		cmHookListening,cmHookConnecting,cmHookInConn,
//		cmHookOutConn,cmHookSend,cmHookRecv,cmHookSendTo,
//		cmHookRecvFrom,cmHookClose};

//SCMPROTOCOLEVENT
RvBool RVCALLCONV cmHookListen(
		IN HPROTCONN hConn,
		IN int nodeId )
{
	return 0;
}

//SCMPROTOCOLEVENT
int RVCALLCONV cmHookListening(
		IN HPROTCONN hConn,
		IN int nodeId,
		IN RvBool error )
{
	//char text[100];
	//wsprintf(text,"New Call Listening <- %s on %s",cmGetProtocolMessageName(hApp,nodeId),
	//	cmProtocolGetProtocolName(cmProtocolGetProtocol(hApp,nodeId)));
	return 0;
}

//SCMPROTOCOLEVENT
int RVCALLCONV cmHookConnecting(
		IN HPROTCONN hConn,
		IN int nodeId )
{
#ifdef __HOOK_LOG
	char text[100];
	wsprintf(text,"New Call Connect <- %s on %s nodeid : %d",cmGetProtocolMessageName(hApp,nodeId),
		cmProtocolGetProtocolName(cmProtocolGetProtocol(hApp,nodeId)), nodeId);
	LOGHOOK(-1, "cmHookConnecting", text);
#endif
	return 0;
}

//SCMPROTOCOLEVENT
int RVCALLCONV cmHookInConn(
		IN HPROTCONN hConn,
		IN int nodeIdFrom,
		IN int nodeIdTo )
{
	return 0;
}

//SCMPROTOCOLEVENT
int RVCALLCONV cmHookOutConn(
		IN HPROTCONN hConn, IN int nodeIdFrom,
		IN int nodeIdTo, IN RvBool error )
{
	return 0;
}

int RVCALLCONV cmEvRASRequest(
                            IN  HRAS             hsRas,
                            IN  HCALL            hsCall,
                            OUT LPHAPPRAS        lphaRas,
                            IN  cmRASTransaction transaction,
                            IN  cmRASTransport*  srcAddress, 
                            IN  HAPPCALL         haCall         OPTIONAL)
{
	
    return 0;
}

int RVCALLCONV cmEvRASConfirm(
                            IN  HAPPRAS          haRas,
                            IN  HRAS             hsRas)
{
#if 0
	// cmRASParamCID
	unsigned char CID[18];
	memset(CID, 0, 18);
	int iLength = 18;
    cmRASGetParam(hsRas,cmRASTrStageConfirm,cmRASParamCID,0,&iLength,(char*)&CID[0]);
	WriteLog("RAD", 0, "CID", (char*)CID);
/*
	cmNonStandardParam nsParam={{""},100,buff};
    cmRASGetParam(hsRas,cmRASTrStageConfirm,cmRASParamNonStandard,0,0,(char*)&nsParam);*/
    /*OutputDebugString("-------------CONFIRM--------------/n");*/
#endif
    return 0;
}

int RVCALLCONV cmEvRASReject(
                           IN  HAPPRAS          haRas,
                           IN  HRAS             hsRas,
                           IN  cmRASReason      reason)
{
    /*OutputDebugString("-------------REJECT--------------/n");*/
    return 0;
}

int RVCALLCONV cmEvRASTimeout(
                            IN  HAPPRAS          haRas,
                            IN  HRAS             hsRas)
{
	//if( hApp )
	//	cmRegister(hApp);
    return 0;
}

//SCMPROTOCOLEVENT
RvBool RVCALLCONV cmHookSend(
		IN HPROTCONN hConn,
		IN int nodeId,
		IN RvBool error )
{
#ifdef __HOOK_LOG
	char text[100];
	char * protocolMesg=cmGetProtocolMessageName(hApp,nodeId);
	wsprintf(text,"New message send -> %s on %s",protocolMesg,
		cmProtocolGetProtocolName(cmProtocolGetProtocol(hApp,nodeId)));
	LOGHOOK(-1, "cmHookSend", text);
#endif
	return 0;
}

//SCMPROTOCOLEVENT
RvBool RVCALLCONV cmHookRecv(
		IN HPROTCONN hConn,
		IN int nodeId,
		IN RvBool error )
{
#ifdef __HOOK_LOG
	char text[100];
	wsprintf(text,"New message recv <- %s on %s",nprn(cmGetProtocolMessageName(hApp,nodeId)),
		nprn(cmProtocolGetProtocolName(cmProtocolGetProtocol(hApp,nodeId))));
	LOGHOOK(-1, "cmHookRecv", text);
#endif
#ifdef _STACK_2_6_5
#ifdef __HOOK_TCS
	if( !strcmp("terminalCapabilitySet", cmGetProtocolMessageName(hApp,nodeId)))
	{
		HPVT hVal=cmGetValTree(hApp);
		WriteLog("hook", -1, "terminalCapabilitySet", "delete");
		int iTableId = pvtGetNodeIdByPath(hVal,nodeId,
					"*.terminalCapabilitySet.capabilityTable");
        int capDesc=pvtGetNodeIdByPath(hVal,nodeId,"*.terminalCapabilitySet.capabilityDescriptors");
		
		int child=pvtChild(hVal,iTableId);
		int pChild;
		HPST h245Syn=cmGetSynTreeByRootName(hApp,"h245");
		while(child>=0)
		{
			int fieldId;
			char name[100];
			int mediaNodeId=pvtGetByPath(hVal,child,"capability.*.*",(long*)&fieldId,NULL,NULL);
			pstGetFieldName(h245Syn,fieldId,sizeof(name),name);
			pChild=child;
			child=pvtBrother(hVal,child);
			if (!strcmp(name,"application"))
			{
				pvtGetByPath(hVal,mediaNodeId,"*",(long*)&fieldId,NULL,NULL);
				pstGetFieldName(h245Syn,fieldId,sizeof(name),name);
				if (!strcmp(name,"t38fax"))
				{
					int indexToRemove;
					int next=pvtNext(hVal,capDesc,capDesc);
					int pNext;
					int synNodeId1,synNodeId2;
					int value;
					pvtGet(hVal,pvtGetNodeIdByPath(hVal,capDesc,"*.simultaneousCapabilities.*.*"),NULL,&synNodeId1,NULL,NULL);
					pvtGetByPath(hVal,pChild,"capabilityTableEntryNumber",NULL,&indexToRemove,NULL);
					while(next>0)
					{
						pNext=next;
						next=pvtNext(hVal,capDesc,next);
						pvtGet(hVal,pNext,NULL,&synNodeId2,&value,NULL);
						if (value==indexToRemove && synNodeId1==synNodeId2)
						{
							int parent=pvtParent(hVal,pNext);
							int grandParent=pvtParent(hVal,parent);
							pvtDelete(hVal,(pvtNumChilds(hVal,parent)==1)?parent:pNext);
							if (pvtNumChilds(hVal,grandParent)==0)
								pvtDelete(hVal,pvtParent(hVal,grandParent));
						}
					}
					int iret = pvtDelete(hVal,pChild);
				}
			}
		}
	}
#endif // __HOOK_TCS
#endif // _STACK_2_6_5
	return 0;
}

//SCMPROTOCOLEVENT
RvBool RVCALLCONV cmHookSendTo(
		IN HPROTCONN	hConn,
		IN int			nodeId,
		IN int			nodeIdTo,
		IN RvBool			error )
{
	char * Mesg = cmGetProtocolMessageName(hApp,nodeId);
	char * Protocol = cmProtocolGetProtocolName(cmProtocolGetProtocol(hApp,nodeId));
#ifdef __HOOK_LOG
	char text[100];
	wsprintf(text,"New message Send -> %s on %s [%d:%d]",nprn(cmGetProtocolMessageName(hApp,nodeId)),
		nprn(cmProtocolGetProtocolName(cmProtocolGetProtocol(hApp,nodeId))), nodeId, nodeIdTo);
	LOGHOOK(-1, "cmHookSendTo", text);
#endif
	if( Mesg == NULL || Protocol == NULL)
	{
		return 1;
	}
	return 0;
}

//SCMPROTOCOLEVENT
RvBool RVCALLCONV cmHookRecvFrom( IN HPROTCONN hConn,
		IN int nodeId,
		IN int	nodeIdFrom,
		IN RvBool	multicast,
		IN RvBool	error )
{
	// 2.6.5의 스택에서는 아래의 부분을 정상적으로 인식하지 못한 것이 있었다.
	// 따라서, 이를 강제로 ACF에 삽입하는 루틴이 필요하다.
	return 0;
}

//SCMPROTOCOLEVENT
void RVCALLCONV cmHookClose( IN HPROTCONN hConn )
{
	return;
}
