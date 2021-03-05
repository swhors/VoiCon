//

#include <stdafx.h>
#include "acmcodec.h"
//#include "mmsystem.h"
//#include "mmreg.h"
//#include "msacm.h"

HACMDRIVERID			DeviceId;
HACMDRIVERID			pcmID;
HACMSTREAM				m_hStream;
HACMDRIVER				m_hDriver;
DRIVERINFO				driverInfo;
WAVEFORMATEX			pcmfmt;


CACMCodec::CACMCodec()
{
    m_hStream = NULL;
    dataBuffer = NULL;
    bufferSize = 0;
	bStart = FALSE;
}

CACMCodec::~CACMCodec()
{
    Close();
}

BOOL CALLBACK FindFormatEnum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport)
{	
	PDRIVERINFO di = (PDRIVERINFO) dwInstance;
	if (pafd->dwFormatTag == (DWORD)di->wFormatTag)
	{
		di->hadid = hadid;
		if( pafd->dwFormatTag == WAVE_FORMAT_PCM)
		{
			if( pafd->pwfx->nChannels == 1 &&
				pafd->pwfx->nBlockAlign == 2 &&
				pafd->pwfx->wBitsPerSample == 16 &&
				pafd->pwfx->nSamplesPerSec == 8000)
			{
				return FALSE;
			}
			else
				return TRUE;
		}
		else
			return FALSE; // stop enumerating
	}
	return TRUE; // continue enumerating
}

BOOL CALLBACK DriverEnumProc(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
{
	PDRIVERINFO di =  (PDRIVERINFO)dwInstance;
	BOOL bRet = FALSE;
	
	ACMDRIVERDETAILS dd;
	dd.cbStruct = sizeof(ACMDRIVERDETAILS);
	HACMDRIVER driver;
	MMRESULT mmr = acmDriverOpen(&driver, hadid, 0);
	DWORD dwSize = 0;
	
	if (mmr)
	{
		di->hadid = NULL;
		goto endcallBack;
	}
	
	// enumerate the formats it supports
	mmr = acmMetrics((HACMOBJ)driver, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
	if(mmr)
	{
		di->hadid = NULL;
		goto endcallBack;
	}

	if (dwSize < sizeof(WAVEFORMATEX)) dwSize = sizeof(WAVEFORMATEX); // for MS-PCM
	if( di->pWf )
		VirtualFree(di->pWf,sizeof(WAVEFORMATEX),MEM_RELEASE);
	di->pWf =(LPWAVEFORMATEX)VirtualAlloc(NULL,sizeof(WAVEFORMATEX),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE); 
	
	di->pWf->cbSize = LOWORD(dwSize) - sizeof(WAVEFORMATEX);
	di->pWf->wFormatTag = di->wFormatTag;
	ACMFORMATDETAILS fd;
	memset(&fd, 0, sizeof(fd));
	fd.cbStruct = sizeof(fd);
	fd.pwfx = di->pWf;
	fd.cbwfx = dwSize;
	fd.dwFormatTag = di->wFormatTag;
	mmr = acmFormatEnum(driver, &fd, FindFormatEnum, (DWORD)(VOID*)di, 0);
	
	if(mmr ||di->hadid)
		bRet = FALSE;
	else 
		bRet = TRUE;
endcallBack:
	acmDriverClose(driver, 0);
	return bRet;
}

HACMDRIVERID AcmFindCodecDriver(WORD wFormatTag)
{
	driverInfo.hadid = NULL;
	driverInfo.wFormatTag = wFormatTag;
	MMRESULT mmr = acmDriverEnum(DriverEnumProc, (DWORD)(VOID*)&driverInfo, 0); 
	if(mmr)
	{
		AfxMessageBox("Can't find Audio Codec Driver..");
		return NULL;
	}
	return driverInfo.hadid;
}

void CACMCodec::build_pcm(PWAVEFORMEX pWaveForm)
{
	pWaveForm->wf.wFormatTag = WAVE_FORMAT_PCM;
	pWaveForm->wf.nChannels = 1;
	pWaveForm->wf.wBitsPerSample = 16;
	pWaveForm->wf.cbSize = 0;
	pWaveForm->wf.nSamplesPerSec = 8000;
	pWaveForm->wf.nAvgBytesPerSec = pWaveForm->wf.nSamplesPerSec*(pWaveForm->wf.wBitsPerSample/8);;
	pWaveForm->wf.nBlockAlign = 2;
	pWaveForm->wf.nBlockAlign = (pWaveForm->wf.wBitsPerSample/8)*pWaveForm->wf.nChannels;
}

void CACMCodec::build_g7231(PWAVEFORMEX pWaveForm)
{
	WAVEFORMEX g7231format = 
	{
		{66, 1, 8000, 800, 24, 0, 10 },
		{2, 0, 0xce, 0x9a, 0x32, 0xf7, 0xa2, 0xae, 0xde, 0xac }
	};
	if( pWaveForm )
		memcpy(pWaveForm, &g7231format, sizeof( WAVEFORMEX ));
}

int CACMCodec::Open( int direction )
{
	MMRESULT ret;
	WORD wTag=WAVE_FORMAT_PCM;
	WAVEFORMEX waveOut;
	WAVEFORMEX waveIn;
	if( !bStart )
	{
		memset(&waveIn, 0, sizeof( WAVEFORMEX));
		memset(&waveOut, 0, sizeof( WAVEFORMEX));
		switch(this->dwInType )
		{
		case WAVE_FORMAT_ON_G723:
			build_g7231(&waveIn);
			break;
		case WAVE_FORMAT_PCM:
		default:
			build_pcm(&waveIn);
			break;
		}
		
		switch(this->dwOutType)
		{
		case WAVE_FORMAT_ON_G723:
			build_g7231(&waveOut);
			break;
		case WAVE_FORMAT_PCM:
		default:
			build_pcm(&waveOut);
			break;
		}
		if( dwOutType == WAVE_FORMAT_ON_G723 ||
			dwInType == WAVE_FORMAT_ON_G723 )
			wTag = WAVE_FORMAT_ON_G723;
		HACMDRIVERID DeviceId = AcmFindCodecDriver(wTag);
		if( !DeviceId )
			return -1;
		ret = acmDriverOpen(&m_hDriver, DeviceId, 0);
		if( ret )
		{
			AfxMessageBox("error : acmDriverOpen!!");
			return -1;
		}
		
		ret = acmStreamOpen(&m_hStream,
			m_hDriver, // driver
			(WAVEFORMATEX*)&waveIn,//srcfmt, // source format
			(WAVEFORMATEX*)&waveOut, //destfmt, // destination format
			NULL, // no filter
			NULL, // no callback
			0, // instance data (not used)
			ACM_STREAMOPENF_NONREALTIME); // flags
		if(!ret)
		{
			dataBuffer = new unsigned char[5000];
			if(dataBuffer) bufferSize = 5000;
			else bufferSize = 0;
		}
		else
		{
			Close();
			return -1;
		}
		bStart = TRUE;
		return ret;
	}
	return 0;
}

void CACMCodec::Close()
{
    //TODO: Add your source code here
	if( bStart )
	{
		if(m_hStream)
		{
			acmStreamClose(m_hStream, 0);
			m_hStream = 0;
		}
		
		if( m_hDriver )
		{
			acmDriverClose(m_hDriver,TRUE);
			m_hDriver = NULL;
		}
		
		if(dataBuffer)
		{
			delete dataBuffer;
			dataBuffer = NULL;
			bufferSize = 0;
		}
		bStart = FALSE;
	}
}

BOOL CACMCodec::Decode(PBYTE buffer, int length, int *retlen)
{
    //TODO: Add your source code here
    int ret;
    ACMSTREAMHEADER header;
    memset(&header, 0, sizeof(header));
    header.cbStruct = sizeof(header);
    header.pbSrc = (BYTE *)buffer;
    header.cbSrcLength = length;
    header.pbDst = (BYTE *)dataBuffer;
    header.cbDstLength = bufferSize;

  // prep the header
    ret = acmStreamPrepareHeader(m_hStream, &header, 0);
    if (ret != 0)
    {
        TRACE("Codec\tError in decode acmStreamPrepareHeader: error=%d\n", ret);
        return FALSE;
    }

    ret = acmStreamConvert(m_hStream, &header, 0);
    if (ret != 0) 
    {
        TRACE("Codec\tError in decode acmStreamConvert: error=%d\n",ret);
        return FALSE;
    }
	ret = acmStreamUnprepareHeader(m_hStream, &header, 0);
    *retlen = header.cbDstLengthUsed;
    return TRUE;

}

int CACMCodec::Encode(PBYTE buffer, int size)
{
	int ret;
    ACMSTREAMHEADER header;
    memset(&header, 0, sizeof(header));
    header.cbStruct = sizeof(header);
    header.pbSrc = (BYTE *)buffer;
    header.cbSrcLength = size;
    header.pbDst = (BYTE *)dataBuffer;
    header.cbDstLength = 24;

  // prep the header
    ret = acmStreamPrepareHeader(m_hStream, &header, 0);
    if (ret != 0)
    {
        TRACE("Codec\tError in encode acmStreamPrepareHeader: error=%d\n", ret);
        return FALSE;
    }

    ret = acmStreamConvert(m_hStream, &header, 0);
    if (ret != 0)
    {
        TRACE("Codec\tError in encode acmStreamConvert: error=%d\n",ret);
        return FALSE;
    }

	ret = acmStreamUnprepareHeader(m_hStream, &header, 0);
    
    return header.cbDstLengthUsed;
}
