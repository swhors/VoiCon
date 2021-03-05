// ACMWAVE.cpp: implementation of the CACMWAVE class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ACMWAVE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline void BuildPCMWAVE(WAVEFORMATEX & pcm)
{
	pcm.cbSize = 0;
	pcm.wFormatTag = WAVE_FORMAT_PCM;
	pcm.nChannels = 1;
	pcm.nSamplesPerSec = 8000;
	pcm.wBitsPerSample = 16;
	pcm.nBlockAlign = pcm.nChannels * pcm.wBitsPerSample / 8;
	pcm.nAvgBytesPerSec = pcm.nSamplesPerSec * pcm.nBlockAlign;
}

CACMWAVE::CACMWAVE(char *fName)
{
	//CWave::CWave(fName);
	this->bAcm = 0;
	this->pOutBufLen = 0;
	this->pOutBuf = NULL;
	this->dwOutType = ACM_OUT_NONE;
	this->dwInType = ACM_IN_NONE;
	this->DeviceId = NULL;
	this->pcmID=NULL;
	this->m_hStream=NULL;
	this->m_hDriver=NULL;
	this->CodedLen = 0;
	memset(&driverInfo, 0, sizeof(DRIVERINFO));
}

CACMWAVE::~CACMWAVE()
{
	CWave::~CWave();
}

int CACMWAVE::open(IN char *fileName,IN int bRead)
{
	if( CWave::open(fileName, bRead) )
	{
		if( CWave::Read() )
		{
			if(!acm_open() )
				return 1;
		}
	}
	return 0;
}

int CACMWAVE::acm_convert()
{
	CString csbuf;
	int ret;
	BOOL bRet=FALSE;
	ACMSTREAMHEADER header;
	PBYTE pbuf=NULL;
	memset(&header, 0, sizeof(header));
	header.cbStruct = sizeof(header);
	header.pbSrc = this->GetData();
	header.cbSrcLength = this->GetDataLen();
	// 출력 메모리의 양이 충분하지 않은 경우 정상적인 
	// 변환이 되지 않는다.
	switch(this->dwOutType)
	{
	case ACM_OUT_WAVE:
		{
			if( this->dwInType == ACM_IN_G7231)
				this->pOutBufLen = header.cbSrcLength*22;
			else if( this->dwInType == ACM_IN_G711U||
				this->dwInType == ACM_IN_G711A )
				this->pOutBufLen = header.cbSrcLength*3;
		}
		break;
	case ACM_OUT_G7231:
		// G7231의 경우 Wave 파일의 -20배 정도로 
		// 압축이 이루어 진다.
		this->pOutBufLen  = header.cbSrcLength /18;
		break;
	case ACM_OUT_G711U:
	case ACM_OUT_G711A:
		// G711의 경우 Wave -2배 정도로 압축이 이루어 진다.
		this->pOutBufLen = header.cbSrcLength / 2;
		break;
	default:
		return 0;
	}
	this->pOutBuf = new unsigned char[this->pOutBufLen +2];
	memset(this->pOutBuf, 0,this->pOutBufLen +2);
		
	header.pbDst = this->pOutBuf;
	header.cbDstLength = this->pOutBufLen;
	
	// prep the header
	ret = acmStreamPrepareHeader(m_hStream, &header, 0);
	if (ret != 0)
	{
		csbuf.Format("Codec\tError in encode acmStreamPrepareHeader: error=%d\n", ret);
		AfxMessageBox(csbuf);
		return 0;
	}
	
    ret = acmStreamConvert(m_hStream, &header, 0);
    if (ret != 0)
	{
		csbuf.Format("Codec\tError in decode acmStreamConvert: error=%d\n",ret);
		AfxMessageBox(csbuf);
		return 0;
	}
	
	ret = acmStreamUnprepareHeader(m_hStream, &header, 0);
	if( ret != 0)
	{
		csbuf.Format("Codec\tError in acmStreamUnprepareHeader: error=%d\n",ret);
		AfxMessageBox(csbuf);
		return 0;
	}
	
#if 0
	FILE * fp;
	fp = fopen( "test.dat", "w+b");
	if( fp )
	{
		fwrite( pbuf, 1, iDsLength, fp);
		fclose(fp);
	}
	fp = fopen( "test1.dat", "w+b");
	if( fp )
	{
		fwrite( szDsdataBuffer, 1, iDsLength, fp);
		fclose(fp);
	}
#endif
	this->CodedLen = header.cbDstLengthUsed;
	return 1;
}

int CACMWAVE::write_raw()
{
	HFILE Inn;
	Inn=_lcreat(this->GetDESFileName(), 0);
	
	if(Inn <= 0)
	{
		AfxMessageBox("FIle Open Error.");
		return 0;
	}
	
	if( dwOutType == 32)
		_lwrite(Inn, (LPSTR)&CodedLen, 4);
	//웨이브 데이터 저장
	
	if( CodedLen != _lwrite(Inn, (LPSTR)pOutBuf, CodedLen) )
	{
		AfxMessageBox("Not enough disk space.");
		return 0;
	}
	_lclose(Inn);
	return 1;
}

int CACMWAVE::write_wave()
{
	return 0;
}

int CACMWAVE::convert()
{
	if( this->acm_convert() )
	{
		if( this->dwOutType == ACM_OUT_WAVE )
			return this->write_wave();
		else
			return this->write_raw();
	}
	return 0;
}

void CACMWAVE::close()
{
	CWave::close();
	acm_close();
	if( this->pOutBuf )
		delete this->pOutBuf;
	this->pOutBufLen = 0;
}

void CACMWAVE::acm_close()
{
	if(m_hStream)
	{
		acmStreamClose(m_hStream, 0);
		m_hStream = 0;
	}
	
	if(m_hDriver)
	{
		acmDriverClose(m_hDriver,TRUE);
	}
	
	if( driverInfo.pWf )
	{
		VirtualFree(driverInfo.pWf ,sizeof(WAVEFORMATEX),MEM_RELEASE);
		driverInfo.pWf = NULL;
	}
}

int CACMWAVE::acm_open()
{
	BOOL ret;
	WORD wTag=NULL;
	
	WAVEFORMATEX *destfmt, *srcfmt;
	
	G7231FORMAT g7231format = { {66, 1, 8000, 800, 24, 0, 10 }, {2, 0, 0xce, 0x9a, 0x32, 0xf7, 0xa2, 0xae, 0xde, 0xac } };
	BuildPCMWAVE(this->pcmfmt);
	switch(this->dwOutType)
	{
	case ACM_OUT_WAVE:
		wTag = WAVE_FORMAT_PCM;
		break;
	case ACM_OUT_G7231:
		wTag = WAVE_FORMAT_ON_G723;
		break;
	case ACM_OUT_G711U:
		wTag = WAVE_FORMAT_MULAW;
		break;
	case ACM_OUT_G711A:
		wTag = WAVE_FORMAT_ALAW;
		break;
	case ACM_OUT_GSM:
		wTag = WAVE_FORMAT_GSM610;
		break;
	default:
		return 0;
	}
	
	this->DeviceId = AcmFindCodecDriver(wTag);
	if( DeviceId == NULL )
	{
		return FALSE;
	}
	ret = acmDriverOpen(&m_hDriver, DeviceId, 0);
	if(ret)
	{
		acm_close();
		AfxMessageBox("error : acmDriverOpen!!");
		return FALSE;
	}
	
	switch(this->dwInType)
	{
	case ACM_IN_WAVE:
		srcfmt = (WAVEFORMATEX*)&pcmfmt;
		break;
	case ACM_IN_G7231:
		srcfmt = (WAVEFORMATEX*)&g7231format;
		break;
	case ACM_IN_G711U:
	case ACM_IN_G711A:
	case ACM_IN_GSM:
	default:
		return 0;
	}
	
	switch(this->dwOutType)
	{
	case ACM_OUT_WAVE:
		destfmt = (WAVEFORMATEX*)&pcmfmt;
		break;
	case ACM_OUT_G7231:
		destfmt = (WAVEFORMATEX*)&g7231format;
		break;
	case ACM_OUT_G711U:
	case ACM_OUT_G711A:
	case ACM_OUT_GSM:
		destfmt = (WAVEFORMATEX*)&driverInfo.pWf[0];
		break;
	default:
		return 0;
	}
	m_hDriver = NULL;
	ret = acmStreamOpen(&m_hStream,
						m_hDriver, // driver
						srcfmt, // destination format
						destfmt, // source format
						NULL, // no filter
						NULL, // no callback
						0, // instance data (not used)
						ACM_STREAMOPENF_NONREALTIME); // flags
	if(ret)
	{
		this->acm_close();
		AfxMessageBox("error : acmStreamOpen!!");
	}
	return ret;
}

HACMDRIVERID CACMWAVE::AcmFindCodecDriver(WORD wFormatTag)
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

BOOL CACMWAVE::DriverEnumProc(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
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

BOOL CACMWAVE::FindFormatEnum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport)
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

void CACMWAVE::EnableOutG711U()
{
	this->dwOutType = ACM_OUT_G711U;
}

void CACMWAVE::EnableOutG711A()
{
	this->dwOutType = ACM_OUT_G711A;
}

void CACMWAVE::EnableOutG7231()
{
	this->dwOutType = ACM_OUT_G7231;
}

void CACMWAVE::EnableOutGSM()
{
	this->dwOutType = ACM_OUT_GSM;
}

void CACMWAVE::EnableOutWave()
{
	this->dwOutType = ACM_OUT_WAVE;
}

void CACMWAVE::EnableInG711U()
{
	this->dwInType = ACM_IN_G711U;
}

void CACMWAVE::EnableInG711A()
{
	this->dwInType = ACM_IN_G711U;
}

void CACMWAVE::EnableInG7231()
{
	this->dwInType = ACM_IN_G7231;
}

void CACMWAVE::EnableInGSM()
{
	this->dwInType = ACM_IN_GSM;
}

void CACMWAVE::EnableInWave()
{
	this->dwInType = ACM_IN_WAVE;
}


