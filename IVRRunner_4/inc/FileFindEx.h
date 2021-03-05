// FileFindEx.h: interface for the CFileFindEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEFINDEX_H__BEB4997F_2015_4B25_904D_53FFF6504639__INCLUDED_)
#define AFX_FILEFINDEX_H__BEB4997F_2015_4B25_904D_53FFF6504639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileFindEx  
{
public:
	BOOL IsFile();
	CString GetFileTitle();
	CString GetFilePath();
	BOOL IsDirectory();
	BOOL FindNextFile();
	void Close();
	BOOL FindFile(char * newPath, char * fileExt,int iOption);
	CFileFindEx(char * newPath);
	void init();
	char szNewPath[MAX_PATH];
	char szOldPath[MAX_PATH];
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CFileFindEx();
	virtual ~CFileFindEx();

};

#endif // !defined(AFX_FILEFINDEX_H__BEB4997F_2015_4B25_904D_53FFF6504639__INCLUDED_)
