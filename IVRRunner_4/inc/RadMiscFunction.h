#ifndef _RADMISCFUNCTION_H
#define _RADMISCFUNCTION_H

int OnEvNewCall(HCALL hsCall, LPHAPPCALL lphaCall);
int OnEvChannelStateChanged(CONNECTION * pCon, HAPPCHAN haChannel, HCHAN hsChannel, UINT32 state, UINT32 stateMode);
int OnEvRegEvent(IN HAPP hApp,IN cmRegState regState, IN cmRegEvent regEvent, IN int regEventHandle);
int OnEvCallStateChanged(CONNECTION * pCon, HCALL _hsCall, UINT32 state, UINT32 stateMode);
int OnUserInput(CONNECTION * pCon, HCALL hsCall, INT32 userInputId);
int OnChannelNew(CONNECTION * pCon, HCALL _hsCall, HCHAN hsChannel, LPHAPPCHAN lphaChannel);
int OnChannelNewRate(CONNECTION * pCon, channel* c, HCHAN hsChan, UINT32 rate);
int MakeCall(char *szPhoneNum, char * szCalling, int iCallNum );
int ForwardCall(char *szPhoneNum, char * szCalling, int iCallNum );
void DropCall(int iCallNum, cmReasonType iReason);

#endif //_RADMISCFUNCTION_H