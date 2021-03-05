#include "stdafx.h"
#include "voice.h"

/////////////////////////////////////////////////////////////////////////////////////
//  Voice data implementation
//
//  date : 2006/06/08
//  who  : swhors@naver.com
//  
//

#define CHK_POINT1(a) if(!a)return 0;
#define CHK_POINT2(a) if(!a)return;
#define CHK_CODEC(a,b) if(a!=b)continue;else

tag_VOICE_DATA::tag_VOICE_DATA()
{
	this->init();
}

tag_VOICE_DATA::~tag_VOICE_DATA()
{
	this->free();
}

tag_VOICE_DATA & tag_VOICE_DATA::operator =(const tag_VOICE_DATA *d)
{
	return *this;
}

tag_VOICE_DATA & tag_VOICE_DATA::operator =(const tag_VOICE_DATA &d)
{
	sprintf(this->file_name,"%s", d.file_name);
	//memset(this->data, 0, MAX_MENT_BUFF_SIZE);
	//memcpy(this->data, d.data, d.iLength+1);
	this->pData = d.pData;
	this->iLength = d.iLength;
	return *this;
}

int tag_VOICE_DATA::operator ==(const tag_VOICE_DATA &d)
{
	if( !this->equal((char*)&d.file_name) )
		return 1;
	return 0;
}

int tag_VOICE_DATA::operator ==(const char * fName)
{
	if( !this->equal((char*)fName) )
		return 1;
	return 0;
}

int tag_VOICE_DATA::operator<(const tag_VOICE_DATA &d)
{
	if( this->equal((char*)&d.file_name) <0)
		return 1;
	return 0;
}

int tag_VOICE_DATA::operator >(const tag_VOICE_DATA &d)
{
	if( this->equal((char*)&d.file_name)>0 )
		return 1;
	return 0;
}

int tag_VOICE_DATA::operator<(const char *fName)
{
	if( this->equal((char*)fName) <0)
		return 1;
	return 0;
}

int tag_VOICE_DATA::operator >(const char *fName)
{
	if( this->equal((char*)fName)>0 )
		return 1;
	return 0;
}

void tag_VOICE_DATA::free()
{
	init();
}

void tag_VOICE_DATA::init()
{
	memset(this->file_name, 0, 256);
//	memset(this->data, 0, MAX_MENT_BUFF_SIZE);
	this->iLength = 0;
	this->pData = NULL;
}

int tag_VOICE_DATA::equal(char *fName)
{
	return strncmp(this->file_name, fName, strlen(fName));
}

int tag_VOICE_DATA::build(char *fName,unsigned char *data,int dLen)
{
	if( dLen >MAX_MENT_BUFF_SIZE )
		return 0;
	sprintf(this->file_name,"%s", fName);
//	memset(this->data,0, MAX_MENT_BUFF_SIZE);
//	memcpy(this->data, data, dLen);
	if( this->pData )
		delete this->pData;
	this->pData = new unsigned char[dLen+1];
	if( this->pData )
	{
		memset(this->pData, 0, dLen + 1);
		memcpy(this->pData, data, dLen);
		this->iLength = dLen;
		return 1;
	}
	return 0;
}

tag_VOICE_CODEC_PAGE::tag_VOICE_CODEC_PAGE()
{
	init();
}

tag_VOICE_CODEC_PAGE::~tag_VOICE_CODEC_PAGE()
{
	this->DeleteAllVoice();
	if(voice )
		delete voice;
	init();
}

void tag_VOICE_CODEC_PAGE::init()
{
	iCount = 0;
	iCodec = -1;
	voice = NULL;
}

int tag_VOICE_CODEC_PAGE::AddVoice(char *fName,unsigned char *data,int dlen)
{
	tag_VOICE_DATA vData;
	if( vData.build(fName, data, dlen) > 0)
	{
		this->data_per_codec.push_back(vData);
		return 1;
	}
	return 0;
}

void tag_VOICE_CODEC_PAGE::DeleteVoice(char *fName)
{
	std::vector<tag_VOICE_DATA>::iterator start,end,fdata;
	start = this->data_per_codec.begin();
	end = this->data_per_codec.end();
	if( fName )
	{
		fdata = std::find(start,end,fName);
		if( fdata != end )
		{
			if( fdata->pData )
				delete fdata->pData;
			this->data_per_codec.erase(fdata);
		}
	}
	else
	{
		for( fdata = start; fdata < end; fdata++)
		{
			if( fdata )
			{
				delete fdata->pData;
			}
		}
		this->data_per_codec.erase(start,end);
	}
}

void tag_VOICE_CODEC_PAGE::DeleteAllVoice()
{
	DeleteVoice();
}

void tag_VOICE_CODEC_PAGE::sortVoice()
{
	std::vector<tag_VOICE_DATA>::iterator start,end;
	start = this->data_per_codec.begin();
	end = this->data_per_codec.end();
	//if( start != end )
	//	std::sort(start,end);
}

int tag_VOICE_CODEC_PAGE::GetVoice(char *fName,unsigned char *data)
{
	std::vector<tag_VOICE_DATA>::iterator start,end,fdata;
	start = this->data_per_codec.begin();
	end = this->data_per_codec.end();
	fdata = std::find(start,end,fName);
	if( fdata != end )
	{
        //memcpy(data, fdata->data, fdata->iLength);
		memcpy(data, fdata->pData, fdata->iLength);
		return fdata->iLength;
	}
	return 0;
}

int tag_VOICE_CODEC_PAGE::GetVoicePtr(char *fName,PBYTE &data)
{
	std::vector<tag_VOICE_DATA>::iterator start,end,fdata;
	start = this->data_per_codec.begin();
	end = this->data_per_codec.end();
	fdata = std::find(start,end,fName);
	if( fdata != end )
	{
        //memcpy(data, fdata->data, fdata->iLength);
		//memcpy(data, fdata->pData, fdata->iLength);
		data = fdata->pData;
		return fdata->iLength;
	}
	return 0;
}

int tag_VOICE_CODEC_PAGE::GetLength(char *fName)
{
	std::vector<tag_VOICE_DATA>::iterator start,end,fdata;
	start = this->data_per_codec.begin();
	end = this->data_per_codec.end();
	fdata = std::find(start,end,fName);
	if( fdata != end )
	{
        return fdata->iLength;
	}
	return 0;
}

int tag_VOICE_CODEC_PAGE::GetVoice(int count,unsigned char *data)
{
	int length;
	std::vector<tag_VOICE_DATA>::iterator start,fdata;
	length = this->data_per_codec.size();
	if( count < length )
	{
		start = this->data_per_codec.begin();
		fdata = start + count;
		if( fdata < this->data_per_codec.end() )
		{
			//memcpy(data, fdata->data, fdata->iLength);
			memcpy(data, fdata->pData, fdata->iLength);
			return 1;
		}
	}
	return 0;
}

int tag_VOICE_CODEC_PAGE::GetVoicePtr(int count,PBYTE &data)
{
	int length;
	std::vector<tag_VOICE_DATA>::iterator start,fdata;
	length = this->data_per_codec.size();
	if( count < length )
	{
		start = this->data_per_codec.begin();
		fdata = start + count;
		if( fdata < this->data_per_codec.end() )
		{
			//memcpy(data, fdata->data, fdata->iLength);
			//memcpy(data, fdata->pData, fdata->iLength);
			data = fdata->pData;
			return 1;
		}
	}
	return 0;
}

tag_VOICE_PAGE::tag_VOICE_PAGE()
{
	init();
}

tag_VOICE_PAGE::~tag_VOICE_PAGE()
{
	free();
}

void tag_VOICE_PAGE::init()
{
	this->iPage = -1;
	this->iCodecCount = -1;
	memset(this->language,0, 12);
	memset(this->iCodec, 0, sizeof(int)*MAX_SUPP_CODEC);
	this->codec_page = NULL;
}

void tag_VOICE_PAGE::free()
{
	if(this->codec_page)
		delete []codec_page;
	init();
}

int tag_VOICE_PAGE::allocPage(char *lAnguage,int codec[],int page,int codeccount)
{
	if( codeccount <= 0 )
		return 0;
	this->codec_page = new tag_VOICE_CODEC_PAGE[codeccount];
	if( !this->codec_page )
		return 0;
	this->iCodecCount = codeccount;
	this->iPage = page;
	sprintf(this->language, "%s", lAnguage);
	memcpy(this->iCodec, codec, sizeof(int)*MAX_SUPP_CODEC);
	for(int i = 0; i< codeccount; i++)
	{
		this->codec_page[i].iCodec = codec[i];
	}
	return 1;
}

int	tag_VOICE_PAGE::insertData(int codec,char *fName,unsigned char *data,int dLen)
{
	CHK_POINT1(this->codec_page)
	for(int i = 0; i < this->iCodecCount; i++)
	{
		// examine codec
		CHK_CODEC(this->codec_page[i].iCodec,codec)
		return this->codec_page[i].AddVoice(fName,data,dLen);
	}
	return 0;
}

int tag_VOICE_PAGE::getDataPtr(int codec,char *fName,PBYTE &data, int count)
{
	CHK_POINT1(this->codec_page)
	for(int i = 0; i < this->iCodecCount; i++)
	{
		// examine codec
		CHK_CODEC(this->codec_page[i].iCodec,codec)
		if(count < 0 )
			return this->codec_page[i].GetVoicePtr(fName,data);
		else
			return this->codec_page[i].GetVoicePtr(count,data);
		
	}
	return 0;
}

int tag_VOICE_PAGE::getData(int codec,char *fName,unsigned char *data, int count)
{
	CHK_POINT1(this->codec_page)
	for(int i = 0; i < this->iCodecCount; i++)
	{
		// examine codec
		CHK_CODEC(this->codec_page[i].iCodec,codec)
		if(count < 0 )
			return this->codec_page[i].GetVoice(fName,data);
		else
			return this->codec_page[i].GetVoice(count,data);
		
	}
	return 0;
}

int tag_VOICE_PAGE::getDataLength(int codec,char *fName,int &count)
{
	CHK_POINT1(this->codec_page)
	for(int i = 0; i < this->iCodecCount; i++)
	{
		// examine codec
		CHK_CODEC(this->codec_page[i].iCodec,codec)
		return this->codec_page[i].GetLength(fName);
	}
	return 0;
}

void tag_VOICE_PAGE::deleteData(int codec,char *fName)
{
	CHK_POINT2(this->codec_page)
	for(int i = 0; i < this->iCodecCount; i++)
	{
		if( codec < 0 )
		{
			this->codec_page[i].DeleteVoice(fName);
		}
		else
		{
			//if( codec == 
			// examine codec
			CHK_CODEC(this->codec_page[i].iCodec,codec)
			this->codec_page[i].DeleteVoice(fName);
		}
	}
}

void tag_VOICE_PAGE::deleteAllData()
{
	deleteData();
}

void tag_VOICE_PAGE::sortData(int codec)
{
	CHK_POINT2(this->codec_page)
	for(int i = 0; i < this->iCodecCount; i++)
	{
		if( codec < 0 )
		{
			this->codec_page[i].sortVoice();
		}
		else
		{
			// examine codec
			CHK_CODEC(this->codec_page[i].iCodec,codec)
			this->codec_page[i].sortVoice();
		}
	}
}

void tag_VOICE_PAGE::sortAllData()
{
	this->sortData(-1);
}

//////////////////////////////////////////////////////////////////////////////////////
// end of file..
//
