//
#ifndef __PCMWAVE_H__
#define __PCMWAVE_H__

#include <mmsystem.h>

typedef struct
{
    unsigned char buffer[256];
} tWaveData;

LRESULT WINAPI WaveWinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
 
 
class CPCMWave : public CObject
{
public:
	CPCMWave();
    virtual ~CPCMWave();

public:
	int         bForceClose;
	int			mode;	
    int			dataSize;
	UINT		outbufLength;
	BYTE		*outbuf;	
	BOOL		bStart;
	
	HWAVEIN		hwaveIn;
	HWAVEOUT	hwaveOut;	
	WAVEHDR		outhdr;
	WAVEFORMATEX hfmt;
	
	// Global Variables For Silence Detection
	BOOL	bInTalk;
	UINT	SDThreshold;
	UINT	signalMin;
	UINT	silenceMax;
	int		nSignalFrameReceived;
	int		nSilenceFrameReceived;
	int		nFrameReceived;

	int		curBytes;
	char	inDeviceName[50];
	char	outDeviceName[50];
	BOOL	isadd;
	
	HWND	hPCMWnd;    

public:    
	int open(int dir, int bits, int samples, int bytes, int opt=0);	// dir = 0 : record, dir = 1 : play
	void close();	
    
	//Ä§¹¬¸ðµå...
	BYTE L2Mu(int pcm_val);
	BOOL DetectSilence(short *pPCM);
	BOOL isPlaying();
	
	int addInBuffer();
	virtual void OnBuffered(BYTE *buffer, int size, int ttl=0);
		
	int waveOut(BYTE* data);
	int recordStart();
	int recordStop();
	
	BOOL selectInDev(char *name);
	BOOL selectOutDev(char *name);
	int	searchInDev(char *name);
	int searchOutDev(char *name);  	
	
	int playDTMF(char *FileName);
	void SendDTMFinBand(char dtmf);
};

#endif
 