#define TERMIANL_NUM	2

#define ID_ADMIN_DEFAULT	"root"
#define PW_ADMIN_DEFAULT	"root"

#define LOGIN				"login: "
#define PASSWORD			"Password: "

#define LOGIN_INCORRECT		"Login incorrect\r\n"
#define DISCONNECTED		"Connection closed by foreign host."
#define WELCOM_STRING		"AcroIvr Version 1.0\r\n"

//암호 변경과 관련된 메세지.
#define INPUT_NEW_PWD1		"\r\nNew password:"
#define INPUT_NEW_PWD2		"\n\rRetype new password:"
#define PW_CHANGE_NOTIFY	"\r\nChanging password for user root.\r\n"
#define USER_ERROR			"\r\npasswd: Unknown user name %s.\r\n"
#define NOT_MATCH			"\r\nSorry, passwords do not match\r\n"
#define TOOLONG_PWD			"\r\nSorry, passwords is too long\r\n"
#define	RETRIAL_OVER		"\r\nSorry, can't change passwords\r\n"
#define MODIFY_SUCCESS		"\r\nPasswords changed\r\n"
// Key Input Cahracter

#define END_LINE "\r\n"

#define KEY_BACKSPACE	8
#define KEY_CTRL_C	3

#define KEY_ARROW	27
#define KEY_UP		65
#define KEY_DOWN	66
#define KEY_RIGHT	67
#define KEY_LEFT	68

#define KEY_CTRL_C_STR	"^C"

#define COMMAND_EXIT	"exit"

#define COMMAND_1_HELP		"help"
#define COMMAND_2_VIEW		"view"
#define COMMAND_3_IVR		"ivr"
#define COMMAND_4_LOG		"log"
#define COMMAND_2_CONF		"configure"
#define COMMAND_2_SCE		"scenario"
#define COMMAND_2_SC_L		"scenario list"
#define COMMAND_3_ADDU		"adduser"
#define COMMAND_3_SETPWD	"passwd"
#define	COMMAND_3_START		"start"
#define	COMMAND_3_STOP		"stop"
#define COMMAND_3_UP		"up"
#define COMMAND_3_DOWN		"down"
#define COMMAND_3_UPSTART	"upstart"
#define COMMAND_3_DOWNKILL	"end"
#define COMMAND_3_SET		"set"
#define COMMAND_3_STATE		"state"
#define COMMAND_3_ISRUN		"isrun"
#define COMMAND_4_CLOG		"calllog"
#define COMMAND_4_DLOG		"detaillog"
#define COMMAND_5_CALLRESET	"callReset"

#define ITEM_GK_IP			"gkip"
#define ITEM_GK_PORT		"gkport"
#define ITEM_E164			"e164"
#define ITEM_H323ID			"h323id"
#define ITEM_TTL			"ttl"
#define ITEM_LANGUAGE		"language"
#define ITEM_SCENARIO		"scenario"
#define ITEM_DELAY			"delay"
#define ITEM_DEBUG_CON		"debugCon"
#define ITEM_CALL_COUPLE	"callcoupl"
#define ITEM_USERINPUTDELAY	"userinputdelay"
#define ITEM_USERINPUTWAIT	"userinputwait"

#define ITEM_GK_IP_S			"default gk ip:"
#define ITEM_GK_PORT_S			"default gk port:"
#define ITEM_E164_S				"e164id:"
#define ITEM_H323ID_S			"h323id:"
#define ITEM_TTL_S				"ttl:"
#define ITEM_LANGUAGE_S			"LANGUAGE:"
#define ITEM_SCENARIO_S			"SCENARIO:"
#define ITEM_DELAY_S			"maxDelayJitter:"
#define ITEM_DEBUG_CON_S		"enableDebugConsole:"
#define ITEM_CALL_COUPLE_S		"enableCallCoupling:"
#define ITEM_USERINPUTDELAY_S	"DTMF Input Delay:"
#define ITEM_USERINPUTWAIT_S	"User Input Wait:"

#define ERROR_OVER_ALLOWED		"Error : Over allowed number.\r\n"
#define ERROR_INVALID_ARG		"Error : Argument is invalid.\r\n"
#define ERROR_NOT_FOUND_SCE		"Error : not found scenario"
#define ERROR_NOT_OPEN_CONF		"Error : Can't open configuration file.\r\n"
#define ERROR_UNKOWN_COMMAND	"Error : Unknown set command.\r\n"
#define ERROR_CONF_ERROR_1		"Error : File order fail or over max count\r\n"
#define ERROR_CONF_ERROR_2		"Error : language not ordered Fail.\r\n"
#define ERROR_CONF_ERROR_3		"Error : Incorrect language.\r\n"
#define ERROR_CONF_ERROR_4		"Error : Can't open configuration file for writing.\r\n"