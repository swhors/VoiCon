//
#include <stdafx.h>

#include "pcmwave.h"
#include "mmsystem.h"

#define    MAX_SIGNAL    0xFF

#define    UBI_PID	0x3556

BOOL bRTPSendOff;

extern BOOL bDevForceEnd;

CPCMWave::CPCMWave()
{
    hPCMWnd = 0;

    bRTPSendOff = FALSE;

    outbuf = new BYTE[512];
    outbufLength = 512;

    inDeviceName[0] = 0;
    outDeviceName[0] = 0;

    hwaveOut = 0;
    hwaveIn = 0;
    isadd = false;
	bStart = FALSE;

    //침묵모드 초기화
    bInTalk = FALSE;
    SDThreshold = 0;
    signalMin = MAX_SIGNAL;
    silenceMax = 0;
    nSignalFrameReceived = 0;
    nSilenceFrameReceived = 0;
    nFrameReceived = 0;
	bForceClose = 0;
}

CPCMWave::~CPCMWave()
{
    if( outbuf )
    {
        delete[] outbuf;
        outbuf = NULL;
    } 
    TRACE(_T("%s(%i) : ~CPCMWave()\n"), __FILE__,__LINE__); 
}

int CPCMWave::open(int dir, int bits, int samples, int bytes, int opt/*=0*/)
{
    int    devid;    
    MMRESULT ret;
    char *pszClass = "PCMWaveWinClass";

    dataSize = bytes;
    mode = dir;
    if( bDevForceEnd )
		return 0;
    hfmt.wFormatTag = WAVE_FORMAT_PCM;
    hfmt.nChannels = 1;
    hfmt.wBitsPerSample = bits;
    hfmt.cbSize = 0;
    hfmt.nSamplesPerSec = samples;
    hfmt.nBlockAlign = ( hfmt.wBitsPerSample / 8 ) * hfmt.nChannels;
    hfmt.nAvgBytesPerSec = hfmt.nSamplesPerSec * hfmt.nBlockAlign;
    
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc    = WaveWinProc;
    wc.hInstance      = ghInstance;
    wc.lpszClassName  = pszClass;
    RegisterClass(&wc);
    
    hPCMWnd = CreateWindow( pszClass, NULL,WS_OVERLAPPED | WS_MINIMIZE, 0, 0, 0, 0, NULL, NULL, ghInstance, NULL );
    
	if( bDevForceEnd )
		return 0;
    if(outDeviceName[0])
    {
        devid = searchOutDev(outDeviceName);
    }
    else devid = WAVE_MAPPER;

    if(!dir)
    {    //record
        ret = waveInOpen(&hwaveIn, devid, &hfmt, (DWORD)hPCMWnd, 0, CALLBACK_WINDOW);

        if ( ret != MMSYSERR_NOERROR ) TRACE("wave in open err\n");
        else TRACE("wave in no err\n");        
    }
    else
    {    // wave play
        ret = waveOutOpen(&hwaveOut, devid, &hfmt, (DWORD)hPCMWnd, 0, CALLBACK_WINDOW);
        
        if(ret != MMSYSERR_NOERROR) TRACE("wave out open err\n");
        else TRACE("wave out no err\n");
    }
    
    return ret;
}

int CPCMWave::waveOut(BYTE* data)
{
    int ret=0;
    WAVEHDR *phdr;
	if( bDevForceEnd )
		return 0;
	//char databuf[512];
	if(hwaveOut && !bForceClose)
	{
		phdr = new WAVEHDR;
		phdr->lpData = new char[dataSize];
		phdr->dwBufferLength = dataSize;
		phdr->dwFlags = 0;
		phdr->dwLoops = 0;
		phdr->dwUser = (DWORD)phdr;
		//memset(phdr->lpData, 0, dataSize);
		ret = waveOutPrepareHeader(hwaveOut, phdr, sizeof(WAVEHDR));
		if( ret == MMSYSERR_NOERROR )
		{
			memcpy(phdr->lpData, data, dataSize);
			ret = waveOutWrite(hwaveOut, phdr, sizeof(WAVEHDR));
			//if( ret == MMSYSERR_NOERROR)
			//{
			//	waveOutUnprepareHeader(hwaveOut, phdr, sizeof(WAVEHDR));
			return ret;
			//}
		}
	}
	bForceClose = 1;
    SendMessage(hPCMWnd, WM_DESTROY, 0, 0);
	if( !bDevForceEnd )
	{
		bDevForceEnd = 1;
#if 0
        SendMessage(gMainDlgHwnd, WM_USB_CLOSE_MSG, 0, 0);
#else
		AfxGetApp()->m_pMainWnd->SendMessage(WM_USB_CLOSE_MSG, 0, 0);
#endif
	}
    return ret;
}

void CPCMWave::close()
{
    int ret;

	if( bForceClose )
		return;
    if(hwaveOut)
    {
        ret = waveOutReset(hwaveOut);
        ret = waveOutClose(hwaveOut);
        hwaveOut = NULL;
    }

    if(hwaveIn)
    {
        isadd = false;
        waveInStop(hwaveIn);
        waveInReset(hwaveIn);
        waveInClose(hwaveIn);
        hwaveIn = NULL;
    }
    
    if(hPCMWnd)
    {
        DestroyWindow(hPCMWnd);
        hPCMWnd = NULL;
    }

    UnregisterClass("PCMWaveWinClass", ghInstance);
}


BOOL CPCMWave::isPlaying()
{
    MMTIME mm;

    mm.wType = TIME_BYTES;
    waveOutGetPosition(hwaveOut, &mm, sizeof(mm));
    if(!mm.u.cb || mm.u.cb == outhdr.dwBufferLength) return true;
    else return false;

}

BOOL CPCMWave::selectInDev(char *name)
{
    strcpy(inDeviceName, name);
    return 0;
}


BOOL CPCMWave::selectOutDev(char *name)
{
    strcpy(outDeviceName, name);
    return 0;
}


int CPCMWave::searchInDev(char *name)
{
    int ret = -1;
    int devnum, i;
    devnum = waveInGetNumDevs();

    WAVEINCAPS incaps;    
    for(i=0;i<devnum;i++)
    {
        memset(&incaps, 0, sizeof(incaps));
        waveInGetDevCaps(i, &incaps, sizeof(incaps));
        if(!strcmp(name, incaps.szPname))
        {
            ret = i;
            break;
        }
    }
    return ret;
}

int CPCMWave::searchOutDev(char *name)
{
    int ret=-1;
    int devnum, i;

    devnum = waveOutGetNumDevs();

    WAVEOUTCAPS outcaps;
    ret = -1;
    for(i=0;i<devnum;i++)
    {
        memset(&outcaps, 0, sizeof(outcaps));
        waveOutGetDevCaps(i, &outcaps, sizeof(outcaps));
		if( UBI_PID == outcaps.wPid )
		{
			ret = i;
			break;
		}
        if(!strncmp(name, outcaps.szPname, strlen(name)))
        {
            ret = i;
            break;
        }
    }
    return ret;
}


LRESULT WINAPI WaveWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{     
    WAVEHDR *phdr;
    CPCMWave *pwave;

    switch(msg)
    {
	case MM_WIM_CLOSE:
	case MM_WOM_CLOSE:
        SendMessage(hWnd, WM_DESTROY, 0, 0);
		if( !bDevForceEnd )
		{
    		bDevForceEnd = 1;
#if 0
	        SendMessage(gMainDlgHwnd, WM_USB_CLOSE_MSG, 0, 0);
#else
			AfxGetApp()->m_pMainWnd->SendMessage(WM_USB_CLOSE_MSG, 0, 0);
#endif
		}
		break;
    case MM_WOM_DONE:
		{
			if( bDevForceEnd )
				return NULL;
            waveOutUnprepareHeader((HWAVEOUT) wParam, (LPWAVEHDR) lParam, sizeof(WAVEHDR) ); 
            phdr = (LPWAVEHDR)lParam;
            delete[] phdr->lpData;
            delete phdr;
		}
        break; 
    case MM_WIM_DATA:
        LPWAVEHDR inphdr = (LPWAVEHDR) lParam;
		if( bDevForceEnd )
		    break;
		if( waveInUnprepareHeader((HWAVEIN) wParam, (LPWAVEHDR)lParam, sizeof(WAVEHDR)) == MMSYSERR_NOERROR )
		{

			WAVEHDR* pRtpSendHdr = new WAVEHDR;
			memcpy(pRtpSendHdr,inphdr,sizeof(WAVEHDR));
			BYTE * pRtpSending = new BYTE[inphdr->dwBufferLength];
			memcpy(pRtpSending,inphdr->lpData,inphdr->dwBufferLength);
			pRtpSendHdr->lpData = (char*)pRtpSending;

			phdr = (LPWAVEHDR)lParam;
			pwave = (CPCMWave*)(phdr->dwUser);
			if( pwave )
			{
			    //if(!pwave->DetectSilence((short *)pRtpSendHdr->lpData))
			    {
		            if ( !bRTPSendOff && !bDevForceEnd) 
		            {
				        pwave->OnBuffered((unsigned char*)(pRtpSendHdr->lpData), pRtpSendHdr->dwBytesRecorded);
		            }
			    }
			    delete[] phdr->lpData;
			    delete phdr;
				if( pwave )
			        pwave->addInBuffer();
			    delete[] pRtpSendHdr->lpData;
			    delete pRtpSendHdr;
			}
		}
        break;
    } 
    return DefWindowProc(hWnd, msg, wParam, lParam); 
}

int CPCMWave::recordStart()
{
    MMRESULT ret;
	if( !bStart )
	{
		isadd = true;
		if( hwaveIn )
		{ 
			for(int i=0; i < 6; i++)
			{
				addInBuffer();
			}
			if( bDevForceEnd )
			    return -1;
		    ret = waveInStart(hwaveIn);
			if(ret!=MMSYSERR_NOERROR)
			{
				TRACE("wave in start error\n");
			}
			bStart = TRUE;
	   }
    }
    return 0;    
}



int CPCMWave::recordStop()
{
	if( bStart )
	{
		isadd = false;
		bStart = FALSE;
		
		if( bDevForceEnd )
			return -1;
		
		if(hwaveIn) waveInStop(hwaveIn);
	}
    return 0;
}

void CPCMWave::OnBuffered(unsigned char *buffer, int size, int ttl)
{

}

int CPCMWave::addInBuffer()
{    
    MMRESULT ret;
	if( bForceClose )
		return 0;
    if ( !isadd ) return 0;

    LPWAVEHDR phdr = new WAVEHDR;
    ZeroMemory(phdr, sizeof(WAVEHDR));
    BYTE* lpByte = new BYTE[dataSize];
    
    phdr->lpData = (char *) lpByte;
    phdr->dwBufferLength = dataSize;
    phdr->dwFlags = 0;
    phdr->dwLoops = 0;
    phdr->dwUser = (DWORD)this; // in case of wave call back window    
	if( bDevForceEnd )
	    return 0;
    
    ret = waveInPrepareHeader(hwaveIn, phdr, sizeof(WAVEHDR));
    if (ret!=MMSYSERR_NOERROR) 
        TRACE("wave in prepare error\n");

    ret = waveInAddBuffer(hwaveIn, phdr, sizeof(WAVEHDR));    
    if(ret!=MMSYSERR_NOERROR) 
        TRACE("wave in addbuffer error\n");
        
    return dataSize;    
}

/*****************************************************************************/
// Utility Functions
#define    BIAS        (0x84)        /* Bias for linear code. */


// Linear PCM to Mu-Law PCM
unsigned char CPCMWave::L2Mu(int pcm_val)
{
    int        mask, seg;
    unsigned char    uval;
    static short SEGMENT_END[8] = 
        {0xFF, 0x2FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};
    
    if(pcm_val < 0)
    {
        pcm_val = BIAS - pcm_val;
        mask = 0x7F;
    } 
    else
    {
        pcm_val += BIAS;
        mask = 0xFF;
    }

    for(seg=0; seg < 8; seg++)
    {
        if (pcm_val <= SEGMENT_END[seg])
            break;
    }

    if (seg >= 8)        /* out of range, return maximum value. */
    {
        return (0x7F ^ mask);
    }
    else
    {
        uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0xF);
        return (uval ^ mask);
    }
}

// Function name    : DetectSilence
// Description        : Adaptive Silence Detect
// Return type        : Silence -> TRUE, Talk -> FALSE
BOOL CPCMWave::DetectSilence(short * pPCM)
{
    unsigned int    sum=0;
    unsigned int    average;
    int             i;
    BOOL            bHasSignal;

    for(i=0; i < (dataSize/2)/(sizeof(short)*10); i++)
    {
        if (*pPCM < 0)
            sum -= *pPCM;
        else
            sum += *pPCM;
        pPCM += 10;            
    }

    average = sum / i;
    average = L2Mu(average)^0xFF;

    bHasSignal = average > SDThreshold;

    if (bInTalk == bHasSignal)
        nFrameReceived = 0;
    else
    {
        nFrameReceived++;
        if (nFrameReceived > (bInTalk ?  13 : 1))
        {
            bInTalk = !bInTalk;
            // TALK/SILENCE Transition !!
            silenceMax = 0;
            signalMin = MAX_SIGNAL;
            nSignalFrameReceived = 0;
            nSilenceFrameReceived = 0;
        }
    }

    if(SDThreshold == 0)
    {
        SDThreshold = average * 1/2;
        return TRUE;
    }

    if(bHasSignal)
    {
        if (average < signalMin)
            signalMin = average;
        nSignalFrameReceived++;
    }
    else
    {
        if (average > silenceMax) 
            silenceMax = average;
        nSilenceFrameReceived++;
    }

    if((nSignalFrameReceived + nSilenceFrameReceived) > 20)
    {
        if (nSignalFrameReceived >= 20)
            SDThreshold += (signalMin - SDThreshold)/4;
        else if (nSilenceFrameReceived >= 20)
            SDThreshold = (silenceMax + SDThreshold)/2 + 1;
        else if (nSignalFrameReceived > nSilenceFrameReceived) 
            SDThreshold++;
    
        signalMin = MAX_SIGNAL;
        silenceMax = 0;
        nSignalFrameReceived = 0;
        nSilenceFrameReceived = 0;
    }
    return !bInTalk;
}

int CPCMWave::playDTMF(char *FileName)
{
    FILE *pFile = fopen(FileName, "rb");

    if ( pFile != NULL )
    {        
        bRTPSendOff = TRUE;
        recordStop();
        //PlaySound(FileName, NULL, SND_FILENAME);

        BYTE *pDataBuff = new BYTE[dataSize];

        fread(pDataBuff, 40, 1, pFile); // 헤더. 
		memset(pDataBuff, 0, dataSize);
        while ( int readByte = fread(pDataBuff, dataSize, 1, pFile) )
        {
			//if( readByte == dataSize )
			{
                for ( int i=0; i<1; i++ )
                {
                    OnBuffered(pDataBuff, dataSize, 1);
                    //Sleep(30);    // 음원 재생 시간. 
                }
			}
			memset(pDataBuff, 0, dataSize);
        }
        delete[] pDataBuff;
        fclose(pFile);
        recordStart();
        bRTPSendOff = FALSE;
        return 0;
    }
    
    else
        return -1;
}

void CPCMWave::SendDTMFinBand(char dtmf)
{
    char FileName[128];

    switch (dtmf)
    {
        case '0':
            strcpy(FileName, ".\\dtmf\\d0.wav");
            break;
        case '1':
            strcpy(FileName, ".\\dtmf\\d1.wav");
            break;
        case '2':
            strcpy(FileName, ".\\dtmf\\d2.wav");
            break;
        case '3':
            strcpy(FileName, ".\\dtmf\\d3.wav");
            break;
        case '4':
            strcpy(FileName, ".\\dtmf\\d4.wav");
            break;
        case '5':
            strcpy(FileName, ".\\dtmf\\d5.wav");
            break;
        case '6':
            strcpy(FileName, ".\\dtmf\\d6.wav");
            break;
        case '7':
            strcpy(FileName, ".\\dtmf\\d7.wav");
            break;
        case '8':
            strcpy(FileName, ".\\dtmf\\d8.wav");
            break;
        case '9':
            strcpy(FileName, ".\\dtmf\\d9.wav");
            break;
        case '*':
            strcpy(FileName, ".\\dtmf\\star.wav");
            break;
        case '#':
            strcpy(FileName, ".\\dtmf\\sharp.wav");
            break;
        default:
            return;
    }
    playDTMF(FileName);
}