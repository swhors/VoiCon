#if !defined(_GLOBAL_H_)
#define _GLOBAL_H_

#define IVRNAME			"DSIVR"

#define CALL_REQUEST	"Request"
#define CALL_PROCESS	"Process"
#define CALL_CONNECT	"Connect"
#define CALL_WAITING	"Waiting"
#define CALL_BUSY		"CallBusy"
#define CALL_DICON_PROC "Disconnecting"


typedef struct tagIVR_SYSTEM
{
	int				Ivr_MaxCall;
	int				Auth_Server_Port;
	int				Ment_Interval;
	int				DTMF_Waiting_TimeOut;
	int				DTMF_Init_TimeOut;
	CString			Ivr_Server_IP;
	CString			Auth_Server_IP;
	CString			Ivr_Scenario_File;

	int				Ras_MultiCast_Port;
	int				Ras_Response_TimeOut;
	int				Ras_MaxFail;
	int				GateKeeper_Port;
	int				m_RtpPort;
	int				m_Q931Port;
	int				m_RASPort;
	CString			Ras_MultiCastIP;
	CString			H323ID;
	CString			E164ID;
	CString			GateKeeper_IP;

	int				m_nSceCount;
	CString			Scenario[100];

	int				m_nLangCount;
	CString			Language[100];

	int				TTL;
	int				Q931_Response_TimeOut;
	int				Q931_Connect_TimeOut;
	
	int				H245_MasterSlave_TimeOut;
	int				Capability_TimeOut;
	int				Max_AudioDelay_Jitter;
	int				H245_Channel_TimeOut;
	int				iAutoStartTerminal;
}IVR_SYSTEM;


typedef struct tagCommandData{
	char	receiveCommand[1024];
	int		command_number;
}CommandData;




#endif