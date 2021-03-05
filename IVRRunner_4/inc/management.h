#pragma once
//
// file : management.h
//
void LoadingMentFile();
int LoadFolder(int iPage,int iCurCodecPage, int iCodec,char *sbuf);
void GetVoiceFileFolder(IN char *sbuf, OUT char * drBuffer, IN int iLen);
void UnLoadingMentFile();
//unsigned char * GetMentData(IN int iFileLanguagePage, IN int iFileCodecPage, IN int iFileNum, OUT PBYTE sOutbuffer);
int GetMentDataLength(char *sFileName, int iCodec, char *sLanguage, int &iFileLanguagePage, int &iFileCodecPage, OUT PBYTE sOutbuffer);
int	GetMentData(IN char *sFile,IN int iCodec,IN char *sLanguage,IN int &iFileLanguagePage,IN int &iFileCodecPage,OUT PBYTE &sOutBuffer);
//
// end of file.
//
