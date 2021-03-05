///////////////////////////////////////////////////////////////////////////////
//
// header file for IVR&GK interface
//
// made by BaRamDoRi 2002.
// edit date : 2002.12.
//
///////////////////////////////////////////////////////////////////////////////


#if !defined ( __IVR_CONNECTION_H__)
#define __IVR_CONNECTION_H__


////////////////////////////////////////////////////////////////
// definitions 

#define IVR_CONNECT_SOCKET_PORT		9100
#define IVR_CONFERENCE_SOCKET_PORT	9200

// element typede fine
typedef char						FLAG_TYPE;			// STX, ETX
typedef char						HEADER_TYPE;		// packet type
typedef char						RETURN_CODE;		// authentication return value
typedef char						BILLING_MODEL;		// prepayment or postpaid

// FLAG_TYPE : packet flag
#define STX							0x02			// start of packet
#define ETX							0x03			// end of packet

// HEADER_TYPE : packet header type
#define IVR_SEND_CALL_INFORMATION	0x30
#define IVR_SEND_CALL_ACK			0x31

// RETURN_CODE : authenticaiton return code
#define RC_SUCCESS					0x31			// authentication success
#define RC_FAIL						0x32			// database access error
#define RC_INVALID_ARGUMENT			0x33			// unknown error
#define RC_USER_NOT_FOUND			0x34			// not found costomer
#define	RC_USER_DENIED				0x35			// invalid E.164
#define RC_REF_NOT_FOUND			0x36			// not found incoming dial number
#define	RC_UNAVAILABLE				0x37			// temporary out of use
#define	RC_WRONG_FORMET				0x38			// undefined service code
#define	RC_RECORD_EXIST				0x39			// not found H.323 ID
#define	RC_NO_RATE_FOR_CALL			0x41			//
#define	RC_ACCOUNT_HAS_EXPIRED		0x42			// exfired term of balidity of card
#define	RC_NOT_ENOUGH_MONEY			0x43			// not enough amount of prepaid phone card
#define	RC_WRONG_OLD_PASSWORD		0x44			//
#define	RC_ACCOUNT_IN_USE			0x45			// in use (PIN Lock)
#define	RC_INVALID_NUMBER			0x46			// not exist full dial number
#define	RC_ALL_CALLING				0x47			// all phones are busy

// BILLING_MODEL : billing model
#define PREPAYMENT					0x31			
#define POSTPAID					0x32
#define UNKNOWN_BILLING_MODEL		0x33


#define GK_RC_SEND_ERROR			0x60
#define GK_RC_RECV_ERROR			0x61
//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//

typedef struct {
	char			conference_id[36];				// conference ID : String
} conference_information;

#if!defined(__MIGK2_DEFINE_H__)
typedef struct {
	RETURN_CODE		return_code;
	char			customer_no[20];				// customer's number(ID)
	char			pin_number[20];					// customer's PIN number
	char			rate_group[3];					// rate group
	char 			destination_number[20];			// 	
	char			remain_charge[10];				// remains of customer
	char			use_call_time[8];				// call max durations
	BILLING_MODEL	c_billing_mode;					// prepayment or postpaid	
} customer_information;
#endif

//
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// packets

typedef struct tg_ivr_send_call_information_packet 
{
	struct 
	{
		FLAG_TYPE				Flag;			// STX
		HEADER_TYPE				Header;			// IVR_SEND_CALL_INFORMATION
	} Header;

	struct 
	{
		conference_information				ConferenceInfo;
		customer_information				CustomerInfo;
	} Body;

	struct 
	{
		FLAG_TYPE				Flag;			// ETX
	} Tail;

	tg_ivr_send_call_information_packet()		// constructor & initialize
	{
		Header.Flag = STX;
		Header.Header = IVR_SEND_CALL_INFORMATION;
		memset(&Body.ConferenceInfo, 0x20, sizeof(conference_information));
		memset(&Body.CustomerInfo, 0x20, sizeof(customer_information));
		Tail.Flag = ETX;
	}

	void setConferenceInfo(conference_information tempInfo) 
	{
		memcpy(&Body.ConferenceInfo, &tempInfo, sizeof(conference_information));
	}

	void setCustomerInfo(customer_information tempInfo)
	{
		memcpy(&Body.CustomerInfo, &tempInfo, sizeof(customer_information));
	}

	int getPacketSize() { return sizeof(tg_ivr_send_call_information_packet); }
	
} ivr_send_call_information_packet;


typedef struct tg_ivr_send_call_ack_packet
{
	struct 
	{
		FLAG_TYPE				Flag;			// STX
		HEADER_TYPE				Header;			// IVR_SEND_CALL_ACK
	} Header;

	struct 
	{
		conference_information				ConferenceInfo;
	} Body;

	struct 
	{
		FLAG_TYPE				Flag;			// ETX
	} Tail;

	tg_ivr_send_call_ack_packet()				// constructor & initialize
	{
		Header.Flag = STX;
		Header.Header = IVR_SEND_CALL_ACK;
		memset(&Body.ConferenceInfo, 0x20, sizeof(conference_information));
		Tail.Flag = ETX;
	}

	void setConferneceID(char *conferenceID)
	{		
		memcpy(Body.ConferenceInfo.conference_id, conferenceID, strlen(conferenceID));
	}

	int getPacketSize() { return sizeof(tg_ivr_send_call_ack_packet); }

} ivr_send_call_ack_packet;

typedef struct {						// conference send.
	char incoming_conference_ID[36];
	char outgoging_conference_ID[36];
} ivr_send_conference_packet;

typedef struct {
	char E164[128];
	ivr_send_call_information_packet tempReceive;
} ivr_query_data;

//
////////////////////////////////////////////////////////////////


#endif