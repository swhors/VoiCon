#pragma warning(disable:4786)

#include "stdafx.h"

#include <vector>
#include <algorithm>
#include <string>

std::vector<std::string> mentfileList;

void mentfileList_push_back(char *file)
{
	std::vector<std::string>::iterator start,end,f;
	start = mentfileList.begin();
	end = mentfileList.end();
	if( start != end )
	{
		f = std::find(start,end, file);
		if( f == end )
		{
			mentfileList.push_back(file);
		}
	}
	else
	{
		mentfileList.push_back(file);
	}
}

void mentfileList_erase(char *file)
{
	std::vector<std::string>::iterator start,end,f;
	start = mentfileList.begin();
	end = mentfileList.end();
	if( file )
	{
		f = std::find(start,end,file);
		if( f !=end )
			mentfileList.erase(f);
	}
	else
	{
		mentfileList.erase(start,end);
	}
}

int mentfileList_get_name(int count,char *file)
{
	int iSize;
	std::vector<std::string>::iterator start,end,c;
	char *s;
	
	start = mentfileList.begin();
	end = mentfileList.end();
	iSize = mentfileList.size();
	c = start;
	if( c == end || iSize<=count)
		return 0;
	if( (c+count) == end )
		return 0;
	s = (char*)(c+count)->c_str();
	sprintf(file, "%s", s);
	return 1;
}

int mentfileList_size()
{
	return mentfileList.size();
}

void mentfileList_sort()
{
	std::vector<std::string>::iterator start,end;
	start = mentfileList.begin();
	end = mentfileList.end();
	std::sort(start,end);
}

void LoadSystemDefaultMent()
{
	char tbuf[256];
	char *pbuf;
	FILE *fp;
	fp = fopen("sysment.ini", "r+t");
	if( fp )
	{
		memset(tbuf,0,256);
		while(fgets(tbuf, 256, fp))
		{
			if(!memcmp(tbuf, "MENT:", 5))
			{
				pbuf = strstr(tbuf,"\n");
				if( pbuf )
					*pbuf = 0;
				pbuf = strstr(tbuf,"\r");
				if( pbuf )
					*pbuf = 0;
				if( tbuf[5] > 0 )
					mentfileList_push_back(&tbuf[5]);
			}
			memset(tbuf,0,256);
		}
		fclose(fp);
	}
}
