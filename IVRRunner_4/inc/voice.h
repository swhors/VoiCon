#pragma once

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <string>
#include <algorithm>
#include <iomanip>
#include <climits>
#include <sstream>
#include <vector>

#define MAX_SUPP_CODEC	5
//#define MAX_MENT_BUFF_SIZE	65536*10
#define MAX_MENT_BUFF_SIZE	50000*10

typedef struct tag_VOICE_DATA
{
	tag_VOICE_DATA();
	~tag_VOICE_DATA();
	tag_VOICE_DATA &operator=(const tag_VOICE_DATA &d);
	tag_VOICE_DATA &operator=(const tag_VOICE_DATA *d);
	int				operator==(const tag_VOICE_DATA &d);
	int				operator>(const tag_VOICE_DATA &d);
	int				operator<(const tag_VOICE_DATA &d);
	int				operator==(const char * fName);
	int				operator>(const char * fName);
	int				operator<(const char * fName);
	int				build(char *fName,unsigned char *data,int dLen);
	int				equal(char *fName);
	void			init();
	void			free();
	char			file_name[256];
	int				iLength;
	//unsigned char	data[MAX_MENT_BUFF_SIZE];
	unsigned char   *pData;
} VOICE_DATA, * PVOICE_DATA;

typedef struct tag_VOICE_CODEC_PAGE
{
	tag_VOICE_CODEC_PAGE();
	~tag_VOICE_CODEC_PAGE();
	int			AddVoice(char *fName,unsigned char *data,int len);
	int			GetVoice(char *fName,unsigned char *data);
	int			GetVoice(int count,unsigned char *data);
	int			GetVoicePtr(char *fName,PBYTE &data);
	int			GetVoicePtr(int count,PBYTE &data);
	int			GetLength(char *fName);
	void		DeleteVoice(char *fName=NULL);
	void		DeleteAllVoice();
	void		sortVoice();
	void		init();
	void		free();
	int			iCount;
	int			iCodec; // 0 g7231, 1 g711alaw, 2 g711mulaw
	VOICE_DATA	*voice;
private:
	std::vector<VOICE_DATA> data_per_codec;
//	std::vector<VOICE_DATA_SMALL> data_per_codec_sm;
} VOICE_CODEC_PAGE, *PVOICE_CODEC_PAGE;

typedef struct tag_VOICE_PAGE
{
	tag_VOICE_PAGE();
	~tag_VOICE_PAGE();
	void		init();
	void		free();
	int			allocPage(char *lAnguage,int iCodec[],int page,int codeccount);
	int			insertData(int codec,char *fName,unsigned char *data,int dLen);
	int			getData(int codec,char *fName,unsigned char *data,int count=-1);
	int			getDataPtr(int codec,char *fName,PBYTE &data, int count=-1);
	int			getDataLength(int codec,char *fName,int &count);
	void		deleteData(int codec=-1,char *fName=NULL);
	void		deleteAllData();
	void		sortData(int codec = -1);
	void		sortAllData();
	int			iPage;
	int			iCodecCount;
	char		language[12];
	int			iCodec[MAX_SUPP_CODEC]; 
				// 0 g7231, 1 g711alaw, 
				// 2 g711mulaw,3 g729, 4 g729a default -1 [none]
	VOICE_CODEC_PAGE	*codec_page;
} VOICE_PAGE, *PVOICE_PAGE;
