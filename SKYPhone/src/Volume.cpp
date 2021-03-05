#include "StdAfx.h"

#include "Volume.h"

#include "MyAudioMixer.h"

//CSliderCtrl	m_sldMic;
//CSliderCtrl	m_sldSpk;

BOOL bMicMute;
BOOL bSpeakerMute;

void GetWaveOutVolume()
{
	WAVEOUTCAPS	waveCaps;
	int iDev;
	
	if ( findAudioDevice(&waveCaps, &iDev) )
	{
		if(waveCaps.dwSupport & WAVECAPS_VOLUME)
		{
			DWORD volume;
			waveOutGetVolume((HWAVEOUT)iDev, (LPDWORD)&volume);

			DWORD left = LOWORD(volume);
			DWORD right = left*0x00010000UL;
			DWORD total = left + right;

			waveOutSetVolume((HWAVEOUT)iDev, total);

			int iVolume = (int)(left*0xFFFF/0xFFFF);

			WaveOutVolume.SetPos(0xFFFF - iVolume);

			WaveOutVolume.UpdateData(FALSE);
		}
	}
}

BOOL findAudioDevice(LPWAVEOUTCAPS t_waveCaps, int *iDev)
{
    UINT uNumDevs;
    
	uNumDevs = waveOutGetNumDevs(); // audio device 의 수를 읽어옴. 
	int iDefault = -1;

	if ( uNumDevs == 0 ) // audio 장비 없음. 리턴.
		return FALSE;

	for ( UINT i=0; i<uNumDevs; i++ ) // default audio device 찾기 (USB Audio 장치)
		if (!waveOutGetDevCaps(i, t_waveCaps, sizeof(WAVEOUTCAPS)) )
			if ( !strcmp(t_waveCaps->szPname, DEFAULT_AUDIO_DEVICE) )
				iDefault = i;

	if ( iDefault != -1 )
	{	// 찾았음. 
		if ( waveOutGetDevCaps(iDefault, t_waveCaps, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR )
		{
			*iDev = iDefault;
			return TRUE;
		}
		else 
			return FALSE;
	}
	else 
	{	// 없음. 그냥 시스템 디폴트 장비를 가져옴. 
		if ( waveOutGetDevCaps(0, t_waveCaps, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
		{
			*iDev = 0;
			return TRUE;
		}
		else 
			return FALSE;
	}
}

void SetWaveOutVolume(int iPos)
{
	int iVolume = 0xFFFF - iPos;
	WAVEOUTCAPS	waveCaps;

	int iDev;
	
	if ( findAudioDevice(&waveCaps, &iDev) )
	{
		if(waveCaps.dwSupport & WAVECAPS_VOLUME)
		{
			DWORD left = iVolume*0xFFFF/0xFFFF;
			DWORD right = left*0x00010000UL;
			DWORD total = left + right;

			waveOutSetVolume((HWAVEOUT)iDev, total);
		}
	}	
}

void GetWaveInVolume()
{
	MIXERCAPS	mixerCaps;
	HMIXER		hMixer;
	MIXERLINE	mxl; 
	int			iDev;

	if ( findAudioInDevice(&mixerCaps, &iDev) )
	{
		if ( mixerOpen(&hMixer, iDev, 0, 0L, CALLBACK_NULL) == MMSYSERR_NOERROR )
		{	// 열었다. 
			mxl.cbStruct = sizeof(MIXERLINE); 
			mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 
			mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE); 

			DWORD cConnections = mxl.cConnections; 
			for(DWORD j=0; j<cConnections; j++){ 
				mxl.dwSource = j; 
				mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE); 
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) 
					break; 
			} 

			LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(sizeof MIXERCONTROL); 
			
			MIXERLINECONTROLS mxlctrl = 
			{
				sizeof mxlctrl, mxl.dwLineID, 
				MIXERCONTROL_CONTROLTYPE_VOLUME, 1, 
				sizeof MIXERCONTROL, pmxctrl
			}; 

			if( mixerGetLineControls((HMIXEROBJ) hMixer, &mxlctrl, 
				MIXER_GETLINECONTROLSF_ONEBYTYPE ) == MMSYSERR_NOERROR )	
			{ // find.
				DWORD cChannels = mxl.cChannels; 
				if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl) 
					cChannels = 1; 

				LPMIXERCONTROLDETAILS_UNSIGNED pUnsigned = 
					(LPMIXERCONTROLDETAILS_UNSIGNED)malloc(cChannels * sizeof MIXERCONTROLDETAILS_UNSIGNED); 
				MIXERCONTROLDETAILS mxcd = {sizeof(mxcd), pmxctrl->dwControlID, 
					cChannels, (HWND)0, sizeof MIXERCONTROLDETAILS_UNSIGNED, (LPVOID) pUnsigned}; 
				mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE); 
				
				int iVolume = pUnsigned[0].dwValue = pUnsigned[cChannels - 1].dwValue;				
				WaveInVolume.SetPos(0xFFFF - iVolume);
				WaveInVolume.UpdateData(FALSE);
				
				mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, 
					MIXER_SETCONTROLDETAILSF_VALUE); 

				free(pmxctrl); 
				free(pUnsigned); 
			} 
			else free(pmxctrl); 
		}
		mixerClose(hMixer);
	}
}

void SetWaveInVolume(int iPos)
{
	MIXERCAPS	mixerCaps;
	HMIXER		hMixer;
	MIXERLINE	mxl; 
	int			iDev;

	int iVolume = 0xFFFF - iPos;

	if ( findAudioInDevice(&mixerCaps, &iDev) )
	{
		if ( mixerOpen(&hMixer, iDev, 0, 0L, CALLBACK_NULL) == MMSYSERR_NOERROR )
		{	// 열었다. 
			mxl.cbStruct = sizeof(MIXERLINE); 
			mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 
			mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE); 

			DWORD cConnections = mxl.cConnections; 
			for(DWORD j=0; j<cConnections; j++){ 
				mxl.dwSource = j; 
				mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE); 
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) 
					break; 
			} 

			LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(sizeof MIXERCONTROL); 
			
			MIXERLINECONTROLS mxlctrl = 
			{
				sizeof mxlctrl, mxl.dwLineID, 
				MIXERCONTROL_CONTROLTYPE_VOLUME, 1, 
				sizeof MIXERCONTROL, pmxctrl
			}; 

			if( mixerGetLineControls((HMIXEROBJ) hMixer, &mxlctrl, 
				MIXER_GETLINECONTROLSF_ONEBYTYPE ) == MMSYSERR_NOERROR )	
			{ // public soundcard find.
				DWORD cChannels = mxl.cChannels; 
				if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl) 
					cChannels = 1; 

				LPMIXERCONTROLDETAILS_UNSIGNED pUnsigned = 
					(LPMIXERCONTROLDETAILS_UNSIGNED)malloc(cChannels * sizeof MIXERCONTROLDETAILS_UNSIGNED); 
				MIXERCONTROLDETAILS mxcd = {sizeof(mxcd), pmxctrl->dwControlID, 
					cChannels, (HWND)0, sizeof MIXERCONTROLDETAILS_UNSIGNED, (LPVOID) pUnsigned}; 
				mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE); 
				// Set the volume to the middle  (for both channels as needed) 
				pUnsigned[0].dwValue = pUnsigned[cChannels - 1].dwValue = iVolume;
					//(pmxctrl->Bounds.dwMinimum+pmxctrl->Bounds.dwMaximum)/2; 
				mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, 
					MIXER_SETCONTROLDETAILSF_VALUE); 

				free(pmxctrl); 
				free(pUnsigned); 
			} 
			else 
			{	// USB Phone find
				free(pmxctrl); 

				CMyAudioMixer m_AudioMixer;
				m_AudioMixer.Open();
				DWORD dwVolume = 0xFFFF - iPos;
				m_AudioMixer.AdjustProperty(dwVolume, 1|0x10000);
				m_AudioMixer.Close();	
			}
		}
		mixerClose(hMixer);
	}
}

BOOL findAudioInDevice(LPMIXERCAPS t_mixerCaps, int *iDev)
{
	UINT uNumDevs;
    
	uNumDevs = mixerGetNumDevs(); // mixer 의 수를 읽어옴. 
	int iDefault = -1;

	if ( uNumDevs == 0 ) // audio 장비 없음. 리턴.
		return FALSE;

	for ( UINT i=0; i<uNumDevs; i++ ) // default audio device 찾기 (USB Audio 장치)
		if (!mixerGetDevCaps(i, t_mixerCaps, sizeof(MIXERCAPS)) )
			if ( !strcmp(t_mixerCaps->szPname, DEFAULT_AUDIO_DEVICE) )
				iDefault = i;

	if ( iDefault != -1 )
	{	// 찾았음. 
		if ( mixerGetDevCaps(iDefault, t_mixerCaps, sizeof(MIXERCAPS)) == MMSYSERR_NOERROR )
		{
			*iDev = iDefault;
			return TRUE;
		}
		else 
			return FALSE;
	}
	else 
	{	// 없음. 그냥 시스템 디폴트 장비를 가져옴. 
		if ( mixerGetDevCaps(0, t_mixerCaps, sizeof(MIXERCAPS)) == MMSYSERR_NOERROR)
		{
			*iDev = 0;
			return TRUE;
		}
		else 
			return FALSE;
	}
}

void MicMute() 
{
	if ( !bMicMute )
	{
		bMicMute = TRUE;

		SetWaveInVolume(0xFFFF);
	}
	else 
	{
		bMicMute = FALSE;

		int iPos = WaveInVolume.GetPos();
		SetWaveInVolume(iPos);
	}
}

void SpeakerMute() 
{
	if ( !bSpeakerMute )
	{
		bSpeakerMute = TRUE;

		SetWaveOutVolume(0xFFFF);
	}
	else 
	{
		bSpeakerMute = FALSE;

		int iPos = WaveOutVolume.GetPos();
		SetWaveOutVolume(iPos);
	}	
}

void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( bSpeakerMute )
	{
		SetWaveOutVolume(0xFFFF);
	}
	else 
	{
		int iPos = WaveOutVolume.GetPos();
		SetWaveOutVolume(iPos);
	}

	if ( bMicMute )
	{
		SetWaveInVolume(0xFFFF);
	}
	else 
	{
		int iPos = WaveInVolume.GetPos();
		SetWaveInVolume(iPos);
	}
	
	OnVScroll(nSBCode, nPos, pScrollBar);
}