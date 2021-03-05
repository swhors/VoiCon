// MyAudioMixer.h: interface for the CMyAudioMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYAUDIOMIXER_H__8CAEF006_71C3_4F35_801A_D8A98942C425__INCLUDED_)
#define AFX_MYAUDIOMIXER_H__8CAEF006_71C3_4F35_801A_D8A98942C425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>

class CMyAudioMixer
{
public:
	CMyAudioMixer();
	virtual ~CMyAudioMixer();
	
	virtual void Initialize();
	virtual void Open();
	virtual void Close();
		
	UINT m_nNumMixers;
	HMIXER m_hMixer;
	MIXERCAPS m_mxcaps;

	CString m_strDstLineName, m_strVolumeControlName;
	DWORD m_dwMinimum, m_dwMaximum;
	DWORD m_dwVolumeControlID;
	int m_nIndex;
	
	typedef struct {
		MIXERLINE MixerLine;
		MIXERCONTROL MixerControl;
		MIXERLINECONTROLS MixerLineControls;
		MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
		MIXERCONTROLDETAILS MixerControlDetails;
	}MY_MIXER_LINER,*PMY_MIXER_LINER;
	MY_MIXER_LINER MixerLineArray[20*4];

	BOOL GetVolume(DWORD dwComponentType,DWORD &dwVal) ;
	BOOL SetVolume(DWORD dwComponentType,DWORD dwVal) ;

	void AnalyseLineInfo(PMY_MIXER_LINER& pMixerLiner,DWORD fdwInfo);
	void AnalyseLineControls(PMY_MIXER_LINER& pMixerLiner,DWORD fdwControls);
	void AdjustSpeakers(PMY_MIXER_LINER& pml);
	void AdjustWaveIn(PMY_MIXER_LINER& pml);
	void AdjustWaveOut(PMY_MIXER_LINER& pml);

public:
	BOOL AdjustProperty(DWORD& dwVal,int nIndex);
private:
	typedef LPCSTR (*MIXER_FUNC)(MMRESULT mmResult);
	LPCSTR LoadString(MMRESULT mmResult,MIXER_FUNC f)
	{
		return f(mmResult);
	}
};

#endif // !defined(AFX_MYAUDIOMIXER_H__8CAEF006_71C3_4F35_801A_D8A98942C425__INCLUDED_)
