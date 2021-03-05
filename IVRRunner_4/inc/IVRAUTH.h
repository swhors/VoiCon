#if !defined ( __IVR_AUTHPACKET_H__)
#define __IVR_AUTHPACKET_H__


#define STX							0x02			// start of packet
#define ETX							0x03			// end of packet
// IVR AUTHEN
// function tag list
#define PIN_AUTH_REQUEST			'A'  // 단순 핀넘버 인증
#define PIN_AUTH_REPONSE			'B'
#define CALLNUM_INFO_REQUEST		'C'  // 전화번호 인증
#define CALLNUM_INFO_RESPONSE		'D'
#define ANI_STATE_REQUEST			'E'  // Ani번호 상태 요구 - Ani등록여부
#define ANI_STATE_RESPONSE			'F'
#define ANI_MATCH_PIN_REQUEST		'G'  // Ani번호등록 및 핀번호 인증 동시 처리
#define ANI_MATCH_PIN_RESPONSE		'H'
#define ANI_PIN_DELETE_REQUEST		'I'  // 핀넘버 삭제 
#define ANI_PIN_DELETE_RESPONSE		'J'
#define PIN_ADD_PIN_REQUEST			'K'  // 핀넘버 충전
#define PIN_ADD_PIN_RESPONSE		'L'
#define CHARGE_INQUIRY_REQUEST		'M'  // 핀넘버 잔액 조회
#define CHARGE_INQUIRY_RESPONSE		'N'
#define ANI_PRE_STATE_REQUEST		'O'
#define ANI_PRE_STATE_RESPONSE		'P'
#define CALLNUM_PRE_INFO_REQUEST	'Q'
#define CALLNUM_PRE_INFO_RESPONSE	'R'
#define CALLNUM_LATER_INFO_REQUEST	'S'
#define CALLNUM_LATER_INFO_RESPONSE	'T'


// return_code tag list
#define AAA_RC_SUCCESS					'0'	// 성공
#define	AAA_RC_USE_TIME_EXPIRATION		'1'	// 유효기간 만료
#define AAA_RC_NOT_ENOUGH_MONEY			'2'	// 잔액부족
#define AAA_RC_ACCOUNT_IN_USE			'3'	// 등록번호 사용중
#define AAA_RC_ONCE_STOP				'4'	// 일시정지
#define AAA_RC_ALL_STOP					'5'	// 정지
#define AAA_RC_USER_NOT_FOUND			'6'	// 사용자를 찿을수 없다.
#define AAA_RC_INVALID_NUMBER			'7'	// 번호가 유효하지 않다.
#define AAA_RC_ALL_CALLING				'8'	// 모든 라인 통화중
#define AAA_RC_UNKOWN_ERROR				'9'	// 알수없는 에러
#define AAA_RC_NOT_ACTIVE				'A' // 비활성화
#define AAA_RC_TERMINATE				'B' // 고객 해지
#define AAA_RC_ANIREG					'C' // 애니번호 등록된 상태
#define AAA_RC_FAIL						'Z' 	// 실패

//Currency select
#define	CURRENCY_KOREA					'0'
#define	CURRENCY_JAPAN					'1'
#define CURRENCY_ENGLISH				'2'
#define CURRENCY_CHINA					'3'
#define CURRENCY_VIETNAM				'4'
#define CURRENCY_MONGOLIA				'5'

// Language Select
#define	LANGUAGE_KOREA				'0'
#define	LANGUAGE_JAPAN				'1'
#define LANGUAGE_ENGLISH			'2'
#define LANGUAGE_CHINA				'3'
#define LANGUAGE_VIETNAM			'4'
#define LANGUAGE_MONGOLIA			'5'
#define LANGUAGE_RUSSIA				'6'
#define LANGUAGE_THAILAND			'7'
#define LANGUAGE_INDONESIA			'8'

// BILLING_MODEL : billing model
#define PREPAYMENT					0x31			
#define POSTPAID					0x32
#define UNKNOWN_BILLING_MODEL		0x33


// IVR-Authentication server packet
typedef struct
{
	char function;			// pin, call number authentication tag
	char return_code;		//  
	char conferenceID[36];	//
	char InIpAddr[16];		// 인입 G/W별 Rating 정책 차별화로 추가(2003/07/10)
	char Pin[20];			// pin number
	char Language;			// Language
	char Destination[20];	// 
	char DID[20];			//
	char CustomerID[20];	// Ani Number
	char BillingModel;		// prepaid system tag
	char RemainCharge[10];	// remain money
	char Currency;			//
	char Use_Ment_Call_Time[8];//
	char Use_Call_Time[8];  // 
	char Rate_Group[3];		// 
}IVR_AUTHENTICATION_PACKET_BODY;

typedef struct tg_IVR_AUTHENTICATION_PACKET
{
	char	tagSTX;
	IVR_AUTHENTICATION_PACKET_BODY pBody;
	char	tagETX;

	tg_IVR_AUTHENTICATION_PACKET()
	{
		tagSTX = STX;
		tagETX = ETX;

		memset( &pBody, 0x20 , sizeof(IVR_AUTHENTICATION_PACKET_BODY));

	}
	

}IVR_AUTHENTICATION_PACKET;

#endif




 



