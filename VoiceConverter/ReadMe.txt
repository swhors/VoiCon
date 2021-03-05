main function -----------------------------------------------
	g723_con conv;
	FormWaveData form_wdata;

	if( conv.bReadSrcFile(&form_wdata) == FALSE){
		AfxMessageBox("error : read source wave file");
		return;
	}
	
	conv.AcmOpen(FALSE);
	
	if( conv.AcmConvertData(FALSE) == FALSE){
		conv.AcmClose();
		return;
	}
	
	conv.AcmClose();

-------------------------------------------------------------
-------------------------------------------------------------
Sub Function-------------------------------------------------
-------------------------------------------------------------
-------------------------------------------------------------
class g723_con
{
public:
	g723_con();
	~g723_con();
	HACMSTREAM m_hStream;
	HACMDRIVER m_hDriver;
	BYTE * szSrcdataBuffer;
	HACMDRIVERID DeviceId;
	
	BOOL bReadSrcFile(FormWaveData * wDataInfo);
	BOOL AcmOpen(BOOL bOpt);
	void AcmClose();
	BOOL AcmConvertData();
	void GetWaveFormat(WORD wFormatTag, 
				   WORD wChannels,
				   DWORD dwSamplesPerSec,
				   WORD wbitsPerSec,
				   WAVEFORMATEX & wf);

};

g723_con::g723_con()
{
	szSrcdataBuffer=NULL;
	m_hStream = NULL;
	m_hDriver=NULL;
	DeviceId=NULL;
}

#define WAVE_FORMAT_ON_G723 66 // Microsoft g.723
typedef const struct{
	WAVEFORMATEX wf;
	BYTE extra[10];
} G7231Form;
BOOL g723_con::AcmOpen(BOOL bOpt)
{
	BOOL ret;
	WORD wTag=NULL;
	G7231Form g7231format= {
		{66, 1, 8000, 800, 24, 0, 10 },
		{2, 0, 0xce, 0x9a, 0x32, 0xf7, 0xa2, 0xae, 0xde, 0xac }
	};
	
	WAVEFORMATEX pcmfmt;
	GetWaveFormat(WAVE_FORMAT_PCM, 1, 8000, 16, pcmfmt);
	wTag = WAVE_FORMAT_ON_G723;
	DeviceId = AcmFindCodecDriver(wTag);
	if( DeviceId == NULL ){
		return FALSE;
	}
	ret = acmDriverOpen(&m_hDriver, DeviceId, 0);
	
	if(ret)
	{
		AfxMessageBox("error : acmDriverOpen!!");
		return FALSE;
	}
		
	ret = acmStreamOpen(&m_hStream,
		m_hDriver, // driver
		&pcmfmt, // destination format
		&g7231format, // source format
		NULL, // no filter
		NULL, // no callback
		0, // instance data (not used)
		ACM_STREAMOPENF_NONREALTIME); // flags
	if(ret)
	{
		AfxMessageBox("error : acmStreamOpen!!");
	}
	return ret;
}

void g723_con::GetWaveFormat(WORD wFormatTag, 
				   WORD wChannels,
				   DWORD dwSamplesPerSec,
				   WORD wbitsPerSec,
				   WAVEFORMATEX & wf){
	wf.cbSize = 0;
	wf.wFormatTag = wFormatTag;
	wf.nChannels = wChannels;
	wf.nSamplesPerSec = dwSamplesPerSec;
	wf.wBitsPerSample = wbitsPerSec;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
}


void g723_con::AcmClose()
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
}

BOOL g723_con::AcmConvertData()
{
	CString csbuf;
	int ret;
	BOOL bRet=FALSE;
	ACMSTREAMHEADER header;
	PBYTE pbuf=NULL;
	memset(&header, 0, sizeof(header));
	header.cbStruct = sizeof(header);
	header.pbSrc = szSrcdataBuffer;
	header.cbSrcLength = iSrcLength;
	
	iDsLength = header.cbSrcLength;
	pbuf = (PBYTE)malloc(iDsLength +2);
	
	if(!pbuf)
		return FALSE;
	
	memset( pbuf, 0, iDsLength +2);
	
	header.pbDst = pbuf;
	header.cbDstLength = iDsLength;
	
	// prep the header
	ret = acmStreamPrepareHeader(m_hStream, &header, 0);
	if (ret != 0)
	{
		csbuf.Format("Codec\tError in encode acmStreamPrepareHeader: error=%d\n", ret);
		AfxMessageBox(csbuf);
		free(pbuf);
		return FALSE;
	}
	
	ret = acmStreamConvert(m_hStream, &header, 0);
	if (ret != 0)
	{
		csbuf.Format("Codec\tError in decode acmStreamConvert: error=%d\n",ret);
		AfxMessageBox(csbuf);
		free(pbuf);
		return FALSE;
	}
	
	ret = acmStreamUnprepareHeader(m_hStream, &header, 0);
	if( ret != 0){
		csbuf.Format("Codec\tError in acmStreamUnprepareHeader: error=%d\n",ret);
		AfxMessageBox(csbuf);
		free(pbuf);
		return FALSE;
	}
	memset( szSrcdataBuffer, 0, iSrcLength);
	memcpy( szSrcdataBuffer, pbuf, header.cbDstLengthUsed);
	iSrcLength = header.cbDstLengthUsed;
	return TRUE;
}

BOOL g723_con::bReadSrcFile(FormWaveData * wDataInfo)
{
	//CFile srcFile;
	CFile pFile;
	HFILE oid;
	oid = _lopen( "test.wav", OF_READ);
	if( oid == HFILE_ERROR)
		return FALSE;
	_lread(oid, (LPSTR)wDataInfo->_riff, 4);
	//전체 파일 크기에서 8Byte를 제외한 크기
	//len = (48 - 8) + dwSaveBufLength;
	_lread(oid, (LPSTR)&wDataInfo->dwDataLength, 4);
	iSrcLength = wDataInfo->dwDataLength - 40;
	_lread(oid, (LPSTR)wDataInfo->_wave, 4);
	_lread(oid, (LPSTR)wDataInfo->_fmt, 4);
	_lread(oid, (LPSTR)&wDataInfo->dwDataLength,4);
	_lread(oid, (LPSTR)&wDataInfo->wf.wFormatTag,2);
	_lread(oid, (LPSTR)&wDataInfo->wf.nChannels,2);
	_lread(oid, (LPSTR)&wDataInfo->wf.nSamplesPerSec,4);
	_lread(oid, (LPSTR)&wDataInfo->wf.nAvgBytesPerSec,4);
	_lread(oid, (LPSTR)&wDataInfo->wf.nBlockAlign,2);
	_lread(oid, (LPSTR)&wDataInfo->wf.wBitsPerSample,2);
	_lread(oid, (LPSTR)&wDataInfo->wf.cbSize,2);
	_lread(oid, (LPSTR)&wDataInfo->wExtraData,2);
	_lread(oid, wDataInfo->_data, 4);
	szSrcdataBuffer =(PBYTE) malloc( iSrcLength +2 );
	memset( szSrcdataBuffer, 0, iSrcLength +2 );
	_lread(oid, szSrcdataBuffer, iSrcLength);
	_lclose(oid);
	return TRUE;
}