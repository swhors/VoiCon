// VoiceDecoderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VoiceDecoder.h"
#include "VoiceDecoderDlg.h"

#include <mmsystem.h>
#include <Mmreg.h>
#include "msacm.h"
#include <io.h>

#define WAVE_FORMAT_ON_G723	66 // Microsoft g.723

typedef struct _WAVEDATAFORM
{
	char _riff[4];
	DWORD dwDataLength;
	char _wave[4];
	char _fmt[4];
	DWORD dwFormatSize;
	WAVEFORMATEX wf;
	DWORD  wExtraData;
	char _data[4];
} WAVEDATAFORM,*PWAVEDATAFORM;

typedef struct _G7231FORMAT
{
	WAVEFORMATEX wf;
	BYTE extra[10];
} G7231FORMAT,*PG7231FORMAT;

typedef struct _DRIVERINFO
{
	HACMDRIVERID hadid;
	WORD wFormatTag;
	WAVEFORMATEX* pWf;
}DRIVERINFO,*PDRIVERINFO;

HACMDRIVERID			DeviceId;
HACMDRIVERID			pcmID;
HACMSTREAM				m_hStream;
HACMDRIVER				m_hDriver;
DRIVERINFO				driverInfo;	
WAVEFORMATEX			pcmfmt;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceDecoderDlg dialog

CVoiceDecoderDlg::CVoiceDecoderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVoiceDecoderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVoiceDecoderDlg)
	m_strFileName = _T("");
	m_strFileName2 = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVoiceDecoderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoiceDecoderDlg)
	DDX_Control(pDX, IDC_BTN_RESETTITLE, m_btnResetTitle);
	DDX_Control(pDX, IDC_BTN_DECODE, m_btnDecode);
	DDX_Control(pDX, IDC_BTN_MIX, m_btnMix);
	DDX_Control(pDX, IDC_BTN_OPEN2, m_btnOpen2);
	DDX_Control(pDX, IDC_ED_FILENAME2, m_ctFileName2);
	DDX_Control(pDX, IDC_ED_FILENAME, m_ctFileName1);
	DDX_Text(pDX, IDC_ED_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_ED_FILENAME2, m_strFileName2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVoiceDecoderDlg, CDialog)
	//{{AFX_MSG_MAP(CVoiceDecoderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpen)
	ON_BN_CLICKED(IDC_BTN_DECODE, OnBtnDecode)
	ON_BN_CLICKED(IDC_BTN_OPEN2, OnBtnOpen2)
	ON_BN_CLICKED(IDC_BTN_MIX, OnBtnMix)
	ON_BN_CLICKED(IDC_BTN_ABOUT, OnBtnAbout)
	ON_EN_CHANGE(IDC_ED_FILENAME2, OnChangeEdFilename2)
	ON_BN_CLICKED(IDC_BTN_RESETTITLE, OnBtnResettitle)
	ON_EN_CHANGE(IDC_ED_FILENAME, OnChangeEdFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceDecoderDlg message handlers
static BOOL CALLBACK FindFormatEnum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport)
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
		else if( pafd->dwFormatTag == WAVE_FORMAT_ON_G723 )
		{
			return FALSE;
		}
		else
			return FALSE; // stop enumerating
	}
	return TRUE; // continue enumerating
}

static BOOL CALLBACK	DriverEnumProc(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
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

BOOL CVoiceDecoderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hStream = NULL;
	m_hDriver = NULL;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	this->m_btnOpen2.EnableWindow(FALSE);
	this->m_ctFileName2.EnableWindow(FALSE);
	this->m_btnMix.EnableWindow(FALSE);
	this->m_btnDecode.EnableWindow(FALSE);
	this->m_btnResetTitle.EnableWindow(FALSE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVoiceDecoderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVoiceDecoderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVoiceDecoderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVoiceDecoderDlg::OnBtnOpen() 
{
	CFileDialog cFileDlg(TRUE, "g7231", "*.g7231", NULL, "Voice for g7231");
	if( cFileDlg.DoModal() == IDOK )
	{
		UpdateData(TRUE);
		this->m_strFileName = cFileDlg.GetPathName();
		UpdateData(FALSE);
		this->m_btnOpen2.EnableWindow(TRUE);
		this->m_ctFileName2.EnableWindow(TRUE);
		//this->m_btnMix.EnableWindow(TRUE);
		this->m_btnDecode.EnableWindow(TRUE);
		this->m_btnResetTitle.EnableWindow(TRUE);
	}
}

void CVoiceDecoderDlg::OnBtnDecode() 
{
	if( this->m_strFileName.GetLength() > 0 )
	{
		DecodeVoice();
	}
	else
		AfxMessageBox("Select File");
}

void CVoiceDecoderDlg::DecodeVoice()
{
	CFile cRaw;
	int dLength;
	unsigned char *dBuff;
	if( !acmOpen() )
	{
		AfxMessageBox("Can't open ACM Device");
		return;
	}
	if( cRaw.Open(m_strFileName, CFile::modeRead|CFile::typeBinary, NULL ) )
	{
		BYTE rBuf[24];
		int tOutLen = 0;
		dLength = cRaw.GetLength();
		dBuff = new unsigned char[dLength*100 + 1];
		if( dBuff)
		{
			time_t tVal;
			memset( dBuff, 0, dLength * 100 + 1);
			while(cRaw.Read(&tVal, sizeof(time_t)) > 0 )
			{
				//while(cRaw.ReadHuge(rBuf, 24)> 0 )
				if(cRaw.ReadHuge(rBuf, 24)> 0 )
				{
					if( !this->acmConvert(rBuf, 24, (dBuff+tOutLen), 480))
						break;
					tOutLen += 480;
				}
			}
			
			if( !this->WaveWrite(this->m_strFileName, dBuff, tOutLen) )
				AfxMessageBox("Can't Create Output File!");
			else
				AfxMessageBox("Succes to Decode!");
			delete dBuff;
		}
		cRaw.Close();
	}
	acmClose();
}

BOOL CVoiceDecoderDlg::acmOpen()
{
	BOOL ret;
	WORD wTag=NULL;
	WAVEFORMATEX pcmfmt;
	WAVEFORMATEX *destfmt, *srcfmt;
	
	G7231FORMAT g7231format = { {66, 1, 8000, 800, 24, 0, 10 }, {2, 0, 0xce, 0x9a, 0x32, 0xf7, 0xa2, 0xae, 0xde, 0xac } };
	pcmfmt.wFormatTag = WAVE_FORMAT_PCM;
	pcmfmt.nChannels = 1;
	pcmfmt.wBitsPerSample = 16;
	pcmfmt.cbSize = 0;
	pcmfmt.nSamplesPerSec = 8000;
	pcmfmt.nAvgBytesPerSec = pcmfmt.nSamplesPerSec*(pcmfmt.wBitsPerSample/8);;
	pcmfmt.nBlockAlign = 2;
	pcmfmt.nBlockAlign = (pcmfmt.wBitsPerSample/8)*pcmfmt.nChannels;
	wTag = WAVE_FORMAT_ON_G723;
	DeviceId = AcmFindCodecDriver(wTag);
	if( DeviceId == NULL )
	{
		return FALSE;
	}
	ret = acmDriverOpen(&m_hDriver, DeviceId, 0);
	if(ret)
	{
		AfxMessageBox("error : acmDriverOpen!!");
		return FALSE;
	}
	
	srcfmt = (WAVEFORMATEX*)&g7231format;
	destfmt = (WAVEFORMATEX*)&pcmfmt;
	
	//m_hStream = NULL;
	
	
	ret = acmStreamOpen(&m_hStream,
						m_hDriver, // driver
						srcfmt, 
						destfmt,
						NULL, // no filter
						NULL, // no callback
						0, // instance data (not used)
						0/*ACM_STREAMOPENF_NONREALTIME*/); // flags

	if(ret)
	{
		acmClose();
		switch(ret )
		{
		case ACMERR_NOTPOSSIBLE:
			AfxMessageBox("The requested operation cannot be performed.");
			break;
		case MMSYSERR_INVALFLAG:
			AfxMessageBox("At least one flag is invalid.");
			break;
		case MMSYSERR_INVALHANDLE:
			AfxMessageBox("The specified handle is invalid.");
			break;
		case MMSYSERR_INVALPARAM:
			AfxMessageBox("At least one parameter is invalid.");
			break;
		case MMSYSERR_NOMEM:
			AfxMessageBox("The system is unable to allocate resources.");
			break;
		default:
			AfxMessageBox("error : acmStreamOpen!!");
			break;
		}
		return FALSE;
	}
	return TRUE;
}

void CVoiceDecoderDlg::acmClose()
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

void CVoiceDecoderDlg::OnBtnOpen2() 
{
	CFileDialog cFileDlg(TRUE, "g7231", "*.g7231", NULL, "Voice for g7231");
	if( cFileDlg.DoModal() == IDOK )
	{
		UpdateData(TRUE);
		this->m_strFileName2 = cFileDlg.GetPathName();
		UpdateData(FALSE);
		this->m_btnMix.EnableWindow(TRUE);
		this->m_btnDecode.EnableWindow(FALSE);
	}
}

typedef struct _voice_rec_data
{
	time_t tData;
	unsigned char voice[24];
}voice_rec_data;

void CVoiceDecoderDlg::OnBtnMix() 
{
	CFile cRaw1,cRaw2,cPcmFile;
	int dLength1,dLength2;
	int	rLength1,rLength2;
	PBYTE rBuff1,rBuff2,outPcmBuf=NULL;
	voice_rec_data vData1, vData2;

	int count1,count2,minCount,maxCount,dSize,tSize,realDataCount;
	PBYTE minData, maxData;
	BYTE	pcmBuff1[480],pcmBuff2[480];
	PBYTE curPtr = NULL;

	rBuff1 = NULL;
	rBuff2 = NULL;

	if( !acmOpen() )
	{
		AfxMessageBox("Can't open ACM Device");
		return;
	}
	if( !VoiceFileOpen(this->m_strFileName, cRaw1, rBuff1, dLength1) )
	{
		AfxMessageBox("Can't open File 1");
		acmClose();
		return;
	}
	if( !VoiceFileOpen(this->m_strFileName2, cRaw2, rBuff2, dLength2) )
	{
		AfxMessageBox("Can't open File 2");
		cRaw1.Close();
		acmClose();
		return;
	}
	
	dSize = sizeof(voice_rec_data);
	tSize = sizeof(time_t);
	count1 = dLength1 / dSize;
	count2 = dLength2 / dSize;
	if( count1 > count2 )
	{
		minCount = count2;
		maxCount = count1;
		minData = rBuff2;
		maxData = rBuff1;
	}
	else
	{
		minCount = count1;
		maxCount = count2;
		minData = rBuff1;
		maxData = rBuff2;
	}
	realDataCount = 0;
	cRaw1.ReadHuge(rBuff1, dLength1);
	cRaw2.ReadHuge(rBuff2, dLength2);
	memcpy(&vData1, minData, dSize );
	memcpy(&vData2, maxData, dSize );
	outPcmBuf = new BYTE[480 *(maxCount + minCount)];
	if( outPcmBuf )
	{
		int i2,i3;
		memset(outPcmBuf, 0, 480*(maxCount+minCount));
		curPtr = outPcmBuf;
		for( i2=0,i3=0,realDataCount = 0; realDataCount < (maxCount+minCount) ; )
		{
			if( (vData1.tData < vData2.tData  && vData1.tData > 0 )
				|| ( vData2.tData<=0 && vData1.tData > 0 ) )
			{
				//
				// 작은 용량의 데이터의 타임 값이 큰 데이터의 타임 값보다 작은 경우 
				//
				if( i3 < minCount )
				{
					i3++;
					rLength1 = acmConvert(vData1.voice, 24, pcmBuff1, 480);
					//cPcmFile.Write(pcmBuff1, 480);
					memcpy(curPtr, pcmBuff1, 480);
					minData += dSize;
					memcpy(&vData1, minData, dSize);
					curPtr += 480;
					realDataCount ++;
				}
				else
				{
					memset(&vData1, 0, dSize);
				}
			}
			else if( vData1.tData == vData2.tData )
			{
				if( i2<maxCount && i3<minCount)
				{
					i2++;
					i3++;
					unsigned char convertBuff[1024];
					rLength1 = acmConvert(vData1.voice, 24, pcmBuff1, 480 );
					rLength2 = acmConvert(vData2.voice, 24, pcmBuff2, 480 );
					memset(convertBuff, 0, 1024);
					WaveMix((short*)&pcmBuff1, (short*)&pcmBuff2, (short*)&convertBuff[0], 480, 480, 0, 8000);
					memcpy(curPtr, convertBuff, 480);
					//cPcmFile.Write(convertBuff, 480);
					minData += dSize;
					memcpy(&vData1, minData, dSize);
					maxData += dSize;
					memcpy(&vData2, maxData, dSize);
					curPtr += 480;
					realDataCount ++;
				}
				else
				{
					if( i2 >= maxCount )
						memset(&vData2, 0, dSize);
					else
					{
						i2++;
						rLength1 = acmConvert(vData2.voice, 24, pcmBuff2, 480);
						//cPcmFile.Write(pcmBuff2, 480);
						memcpy(curPtr, pcmBuff2, 480);
						maxData += dSize;
						memcpy(&vData2, maxData, dSize);
						curPtr += 480;
						realDataCount ++;
					}
					if( i3 >= minCount )
						memset(&vData1, 0, dSize);
					else
					{
						i3++;
						rLength1 = acmConvert(vData1.voice, 24, pcmBuff1, 480);
						//cPcmFile.Write(pcmBuff1, 480);
						memcpy(curPtr, pcmBuff1, 480);
						minData += dSize;
						memcpy(&vData1, minData, dSize);
						curPtr += 480;
						realDataCount ++;
					}
				}
			}
			else if( (vData2.tData < vData1.tData  && vData2.tData > 0 ) 
				|| (vData1.tData <=0 && vData2.tData > 0 ) )
			{
				//
				// 큰 용량의 데이터의 타임 값이 작은 데이터의 타임 값보다 작은 경우 
				// 
				if( i2 < maxCount )
				{
					i2++;
					rLength1 = acmConvert(vData2.voice, 24, pcmBuff2, 480);
					//cPcmFile.Write(pcmBuff2, 480);
					memcpy(curPtr, pcmBuff2, 480);
					maxData += dSize;
					memcpy(&vData2, maxData, dSize);
					curPtr += 480;
					realDataCount ++;
				}
				else
				{
					memset(&vData2, 0, dSize);
				}
			}
			if( i2 == maxCount && i3 == minCount)
				break;
			if( vData1.tData <= 0 && vData2.tData <= 0 )
				break;
		}
	}
	if( realDataCount > 0 ) 
	{
		if( !this->WaveWrite(this->m_strFileName, outPcmBuf, 480 *(realDataCount+1),"-mix") )
			AfxMessageBox("Can't Create Output File!");
		else
			AfxMessageBox("Success to Mix!");
	}
	if( rBuff1 ) delete rBuff1;
	if( rBuff2 ) delete rBuff2;
	if( outPcmBuf ) delete outPcmBuf;
	cRaw1.Close();
	cRaw2.Close();
	acmClose();
}

void CVoiceDecoderDlg::OnBtnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CVoiceDecoderDlg::OnChangeEdFilename2() 
{
	CString csBuf;
	this->m_ctFileName2.GetWindowText(csBuf);
	UpdateData(TRUE);
	this->m_strFileName2 = csBuf;
	UpdateData(FALSE);
	if( this->m_strFileName2.GetLength() == 0 )
	{
		this->m_btnMix.EnableWindow(FALSE);
		this->m_btnDecode.EnableWindow(TRUE);
	}
}

int CVoiceDecoderDlg::acmConvert(PBYTE str_iData, int i_iLen, PBYTE str_oData, int i_oLen)
{
	ACMSTREAMHEADER header;
	UINT ret;
	CString csbuf;
	if( !str_iData || !str_oData || (i_iLen <= 0) || (i_oLen <= 0 ) )
		return 0;
	memset( str_oData, 0, i_oLen);
	memset(&header, 0, sizeof(ACMSTREAMHEADER));
	header.cbStruct = sizeof(ACMSTREAMHEADER);
	header.pbSrc = str_iData;
	header.cbSrcLength = i_iLen;
	header.pbDst = str_oData;
	header.cbDstLength = i_oLen;
	// prep the header
	ret = acmStreamPrepareHeader(m_hStream, &header, 0);
	if(ret != 0)
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
	return header.cbDstLengthUsed;
}

BOOL CVoiceDecoderDlg::WaveWrite(CString fileName,PBYTE data,int len,char *strOtherName)
{
	CFile cWFile;
	char tbuf[64];
	CString csFName = fileName;
	csFName.MakeLower();
	if( strOtherName )
		wsprintf(tbuf, "%s.wav", strOtherName);
	else
		wsprintf(tbuf, ".wav");
	csFName.Replace(".g7231", tbuf);
	if( cWFile.Open(csFName, CFile::modeWrite|CFile::modeCreate|CFile::typeBinary) )
	{
		WAVEDATAFORM waveForm;
		memcpy(waveForm._riff,"RIFF",4);
		waveForm.dwDataLength = len + 32;
		memcpy(waveForm._wave, "WAVE", 4);
		memcpy(waveForm._fmt , "fmt ", 4);
		waveForm.dwFormatSize = 16;
		waveForm.wf.wFormatTag = WAVE_FORMAT_PCM;
		waveForm.wf.nChannels = 1;
		waveForm.wf.wBitsPerSample = 16;
		waveForm.wf.cbSize = 16;
		waveForm.wf.nSamplesPerSec = 8000;
		waveForm.wf.nAvgBytesPerSec = waveForm.wf.nSamplesPerSec*(waveForm.wf.wBitsPerSample/8);;
		waveForm.wf.nBlockAlign = (waveForm.wf.wBitsPerSample/8)*waveForm.wf.nChannels;
		waveForm.wExtraData = 0;
		memcpy(waveForm._data, "data", 4);
		cWFile.Write(waveForm._riff, 4);
		cWFile.Write(&waveForm.dwDataLength, 4);
		cWFile.Write(waveForm._wave, 4);
		cWFile.Write(waveForm._fmt, 4);
		cWFile.Write(&waveForm.dwFormatSize, 4);
		cWFile.Write(&waveForm.wf, 16);
		cWFile.Write(waveForm._data, 4);
		cWFile.Write(&len, 4);
		cWFile.Write(data, len);
		cWFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL CVoiceDecoderDlg::VoiceFileOpen(CString csFileName, CFile &ctrFile, PBYTE &ptrOutBuf, int &dLen)
{
	if( ctrFile.Open(csFileName, CFile::modeRead|CFile::typeBinary, NULL ) )
	{
		dLen = ctrFile.GetLength();
#if 0
		ptrOutBuf = new BYTE [dLen*100 + 1];
		if( ptrOutBuf)
			memset( ptrOutBuf, 0, dLen * 100 + 1);
		else
		{
			ctrFile.Close();
			return FALSE;
		}
#else
		ptrOutBuf = new BYTE[dLen + 1];
		if( ptrOutBuf )
			memset(ptrOutBuf, 0, dLen + 1);
		else
		{
			ctrFile.Close();
			return FALSE;
		}
#endif
		return TRUE;
	}
	return FALSE;
}

//두번째 음성 데이터가 더 작은 것이어야 한다.
void CVoiceDecoderDlg::WaveMix(short *in1, short *in2, short *out, int nSize1, int nSize2, int nPreMargineSec, int nSamplesPerSec)
{
    int i;
    float tmp;
    float nTemp;
    int nVoiceMute = nSamplesPerSec * nPreMargineSec/*초*/;
    for(i = 0 ;i < nSize2/2; i++)
    {
        if(i < (nSize1/2)+nVoiceMute)
        {
            if( i >= nVoiceMute && i < nVoiceMute+20)
            {
                // 합쳐지는 부분의 순간 잡음을 없애기 위해.
                // 추후 Fade Out , Fade In 사용예정
                out[i] = 0;
            }
            else if(i < nVoiceMute)
            {
                // 앞에 x초는 배경음만
                out[i] = (short)in2[i];
            }
            else
            {
                // 합친다. (합친후 다시 나누기할때의 배율은 입맛에 맞게 조절한다.)
                nTemp = in1[i-nVoiceMute];
                nTemp += (float)(in2[i]*0.7);
                //tmp = (float)(nTemp / 2.0);
                tmp = (float)(nTemp / 1.7);
                out[i] = (short)tmp;
            }
        }
        else
        {
	       // 배경음 뒷부분은 그대로..
            out[i] = (short)in2[i];
        }
    }
}

void CVoiceDecoderDlg::OnBtnResettitle() 
{
	UpdateData(TRUE);
	this->m_strFileName2.Empty();
	this->m_strFileName.Empty();
	UpdateData(FALSE);
	this->m_btnMix.EnableWindow(FALSE);
	this->m_btnDecode.EnableWindow(FALSE);
	this->m_btnOpen2.EnableWindow(FALSE);
	this->m_btnResetTitle.EnableWindow(FALSE);
	this->m_ctFileName2.EnableWindow(FALSE);
}

void CVoiceDecoderDlg::OnChangeEdFilename() 
{
	CString csBuf;
	this->m_ctFileName1.GetWindowText(csBuf);
	UpdateData(TRUE);
	this->m_strFileName = csBuf;
	UpdateData(FALSE);
	if( this->m_strFileName.GetLength() == 0 )
	{
		this->m_btnMix.EnableWindow(FALSE);
		this->m_btnDecode.EnableWindow(FALSE);
		this->m_btnOpen2.EnableWindow(FALSE);
		this->m_btnResetTitle.EnableWindow(FALSE);
		UpdateData(TRUE);
		this->m_strFileName2.Empty();
		this->m_ctFileName2.EnableWindow(FALSE);
		UpdateData(FALSE);
	}
}
