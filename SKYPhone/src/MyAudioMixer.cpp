// MyAudioMixer.cpp: implementation of the CMyAudioMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyAudioMixer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LPCSTR f_mixerOpen(MMRESULT mmResult)
{
	switch(mmResult){
	case MMSYSERR_ALLOCATED:
		return "The specified resource is already allocated by the maximum number of clients possible.";
	case MMSYSERR_BADDEVICEID:
		return "The uMxId parameter specifies an invalid device identifier. ";
	case MMSYSERR_INVALFLAG:
		return "One or more flags are invalid. ";
	case MMSYSERR_INVALHANDLE:
		return "The uMxId parameter specifies an invalid handle. ";
	case MMSYSERR_INVALPARAM:
		return "One or more parameters are invalid. ";
	case MMSYSERR_NODRIVER:
		return "No mixer device is available for the object specified by uMxId. Note that the location referenced by uMxId will also contain the value 쭯 1. ";
	case MMSYSERR_NOMEM:
		return "Unable to allocate resources. ";
	default:
		return "unknow error";
	}
}


CMyAudioMixer::~CMyAudioMixer()
{
}

CMyAudioMixer::CMyAudioMixer()
{
	Initialize();
}


BOOL CMyAudioMixer::GetVolume(DWORD dwTypeIndex,DWORD &dwVal)
{
	DWORD dwValue[20*4];
	for(int i=0; i<m_nIndex; i++)
	{
		AdjustProperty(dwValue[i],i);
	}
	return TRUE;
}

BOOL CMyAudioMixer::SetVolume(DWORD dwComponentType,DWORD dwVal) 
{
	for(int i=0; i<m_nIndex; i++)
	{
		AdjustProperty(dwVal,i|0x10000);
	}
	return TRUE;
}

BOOL CMyAudioMixer::AdjustProperty(DWORD& dwVal,int nIndex)
{
	BOOL nMode = nIndex & (1<<16);
	nIndex &= 0xFF;
	if(nIndex >= m_nIndex)
		return FALSE;
		
	MMRESULT result;
	if(nMode)
	{
		MixerLineArray[nIndex].mxcdVolume.dwValue = dwVal;
		result = ::mixerSetControlDetails(
			(HMIXEROBJ)m_hMixer,
			&MixerLineArray[nIndex].MixerControlDetails,
			MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE
		);
	}
	else
	{
		result = ::mixerGetControlDetails(
			(HMIXEROBJ)m_hMixer,
			&MixerLineArray[nIndex].MixerControlDetails,
			MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE
		);
		dwVal = MixerLineArray[nIndex].mxcdVolume.dwValue;
	}
		
	return result == MMSYSERR_NOERROR;
}

void CMyAudioMixer::AnalyseLineControls(PMY_MIXER_LINER& pml,DWORD fdwControls)
{
	MMRESULT result;
	pml->MixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
	result = ::mixerGetLineControls(
		(HMIXEROBJ)m_hMixer,
		&pml->MixerLineControls,
		fdwControls
	);
	if(result != MMSYSERR_NOERROR) 
	{
		return;
	}
	// record dwControlID
	//m_strDstLineName = pml->MixerLine.szName;
	//m_strVolumeControlName = pml->mxc.szName;
	//m_dwMinimum = pml->mxc.Bounds.dwMinimum;
	//m_dwMaximum = pml->mxc.Bounds.dwMaximum;
	//m_dwVolumeControlID = mxc.dwControlID;

	pml->MixerControlDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
	pml->MixerControlDetails.dwControlID = pml->MixerControl.dwControlID;
	pml->MixerControlDetails.cChannels = 1;//pml->MixerLine.cChannels;
	pml->MixerControlDetails.cMultipleItems = 0;
	pml->MixerControlDetails.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	pml->MixerControlDetails.paDetails = &pml->mxcdVolume;
	result = ::mixerGetControlDetails(
		(HMIXEROBJ)m_hMixer,
		&pml->MixerControlDetails,
		MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE
	);
	if(result != MMSYSERR_NOERROR) 
	{
		return;
	}

	pml++;
}

void CMyAudioMixer::AdjustSpeakers(PMY_MIXER_LINER& pml)
{
	pml->MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	MMRESULT result;
	pml->MixerLine.cbStruct = sizeof(MIXERLINE);
	result = ::mixerGetLineInfo(
		(HMIXEROBJ)m_hMixer,
		&pml->MixerLine,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE
	);
	if(result != MMSYSERR_NOERROR) 
	{
		return;
	}

	pml->MixerLineControls.dwLineID = pml->MixerLine.dwLineID;
	pml->MixerLineControls.cControls = pml->MixerLine.cControls;
	pml->MixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	pml->MixerLineControls.pamxctrl = &pml->MixerControl;
	//AnalyseLineControls(pml,MIXER_GETLINECONTROLSF_ALL);

	// get dwControlID
	//pml--;
	pml->MixerLineControls.dwLineID = pml->MixerLine.dwLineID;
	pml->MixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	pml->MixerLineControls.cControls = pml->MixerLine.cControls;
	pml->MixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	pml->MixerLineControls.pamxctrl = &pml->MixerControl;
	AnalyseLineControls(pml,MIXER_GETLINECONTROLSF_ONEBYTYPE);
}

void CMyAudioMixer::AdjustWaveIn(PMY_MIXER_LINER& pml)
{
	pml->MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	MMRESULT result;
	pml->MixerLine.cbStruct = sizeof(MIXERLINE);
	result = ::mixerGetLineInfo(
		(HMIXEROBJ)m_hMixer,
		&pml->MixerLine,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE
	);
	if(result != MMSYSERR_NOERROR) 
	{
		return;
	}

	pml->MixerLineControls.dwLineID = pml->MixerLine.dwLineID;
	pml->MixerLineControls.cControls = pml->MixerLine.cControls;
	pml->MixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	pml->MixerLineControls.pamxctrl = &pml->MixerControl;
	//AnalyseLineControls(pml,MIXER_GETLINECONTROLSF_ALL);

	// get dwControlID
	//pml--;
	pml->MixerLineControls.dwLineID = pml->MixerLine.dwLineID;
	pml->MixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	pml->MixerLineControls.cControls = pml->MixerLine.cControls;
	pml->MixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	pml->MixerLineControls.pamxctrl = &pml->MixerControl;
	AnalyseLineControls(pml,MIXER_GETLINECONTROLSF_ONEBYTYPE);
}

void CMyAudioMixer::AdjustWaveOut(PMY_MIXER_LINER& pml)
{
	pml->MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	MMRESULT result;
	pml->MixerLine.cbStruct = sizeof(MIXERLINE);
	result = ::mixerGetLineInfo(
		(HMIXEROBJ)m_hMixer,
		&pml->MixerLine,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE
	);
	if(result != MMSYSERR_NOERROR) 
	{
		return;
	}

	pml->MixerLineControls.dwLineID = pml->MixerLine.dwLineID;
	pml->MixerLineControls.cControls = pml->MixerLine.cControls;
	pml->MixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	pml->MixerLineControls.pamxctrl = &pml->MixerControl;
	AnalyseLineControls(pml,MIXER_GETLINECONTROLSF_ALL);
	
		// get dwControlID
	//*pml = *(pml-1);
	//pml->MixerLineControls.dwLineID = pml->MixerLine.dwLineID;
	//pml->MixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	//pml->MixerLineControls.cControls = pml->MixerLine.cControls;
	//pml->MixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	//pml->MixerLineControls.pamxctrl = &pml->MixerControl;
	//AnalyseLineControls(pml,MIXER_GETLINECONTROLSF_ONEBYTYPE);
}

void CMyAudioMixer::Initialize()
{
	m_nNumMixers = 0;
	m_hMixer = NULL;
	memset(&m_mxcaps,0,sizeof(m_mxcaps));

	m_dwMinimum = 0;
	m_dwMaximum = 0;
	m_dwVolumeControlID = 0;
	m_nIndex = 0;
	
	memset(MixerLineArray,0,sizeof(MixerLineArray));
}

void CMyAudioMixer::Open()
{
	// get the number of mixer devices present in the system
	m_nNumMixers = ::mixerGetNumDevs();

	MMRESULT result;
	CStringArray AudioName;
	for(UINT uMxId = 0; uMxId<m_nNumMixers; uMxId++)
	{
		result = ::mixerGetDevCaps(uMxId,&m_mxcaps, sizeof(MIXERCAPS));
		if ( result == MMSYSERR_NOERROR)
		{
			AudioName.Add(m_mxcaps.szPname);
			//if(!strcmp(m_mxcaps.szPname,"Yealink USB Audio Device")) break;
			//if(!strcmp(m_mxcaps.szPname,"USB Audio Device")) break;
			//if(strstr(m_mxcaps.szPname,"USB") != NULL) break;
		}
	}
	BOOL fFind = FALSE;
	static char *sFindName[] = {
		"Yealink USB Audio Device",
		"USB Audio Device",
		"USB"
	};
	for(int i=0; i<3 && !fFind; i++)
	{	// USB 장비 검색
		for(int j=0; j<AudioName.GetSize(); j++)
		{
			if(AudioName[j].Find(sFindName[i]) > -1)
			{
				fFind = TRUE;
				uMxId = j;
				break;
			}
		}
	}
	if(!fFind)
	{	// 없으면 그냥 디폴트 장비로. 
		for(int i=0; i<3 && !fFind; i++)
		{
			for(int j=0; j<AudioName.GetSize(); j++)
			{
				fFind = TRUE;
				uMxId = j;
				break;
			}
		}
	}

	result = ::mixerOpen(&m_hMixer,
						uMxId,
						(DWORD)0,
						NULL,
						MIXER_OBJECTF_MIXER | CALLBACK_WINDOW);
	if (result != MMSYSERR_NOERROR)
	{
//		myCallBack.SetLastError(ERROR_BAD_UNIT,YL_CALLBACK_MSG_ERROR);
	}
			
	PMY_MIXER_LINER pml = MixerLineArray;
	
	AdjustSpeakers(pml);
	AdjustWaveIn(pml);
	AdjustWaveOut(pml);

	m_nIndex = ((DWORD)pml - (DWORD)MixerLineArray)/sizeof(MY_MIXER_LINER);
	if(m_nIndex == 0)
	{
//		myCallBack.SetLastError(ERROR_BAD_UNIT,YL_CALLBACK_MSG_ERROR);
	}
}

void CMyAudioMixer::Close()
{
	if(m_hMixer)
	{
		mixerClose(m_hMixer);
	}
	Initialize();
}


