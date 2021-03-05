#pragma once
#include "pcmwave.h"

class CAudioInterface :
	public CPCMWave
{
public:
	CAudioInterface(void);
	~CAudioInterface(void);
	int codecType;
	unsigned char outbuf[1024];
	unsigned char send_buff[21]; // g729a
	unsigned char in_buff[21]; // g729a
	int cur_length; // g729a
	void OnBuffered(unsigned char *buffer, int size, int ttl=0);
	void OnBufferedG729(unsigned char *buffer, int size, int ttl=0);
	void OnBufferedG711(unsigned char *buffer, int size, int ttl=0);
	void OnBufferedG723(unsigned char *buffer, int size, int ttl=0);
	void set_g729();
	void set_g723();
	void set_g711a();
    void set_g711u();
	int OutDevOpen(int type);
	int InDevOpen(int type);
	int DevOpenG723(int type=1, int dir=0);
	int DevOpenG729(int type=2, int dir=0);
	int DevOpenG711(int type=3, int dir=0);
	void OnDataPlay(unsigned char *buffer, int size );
	void OnDataPlayG729(unsigned char *buffer, int size );
	void OnDataPlayG711(unsigned char *buffer, int size );
    void OnDataPlayG723(unsigned char *buffer, int size );
};
