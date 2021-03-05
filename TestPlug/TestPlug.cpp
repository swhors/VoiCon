// TestPlug.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TestPlug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CTestPlugApp

BEGIN_MESSAGE_MAP(CTestPlugApp, CWinApp)
	//{{AFX_MSG_MAP(CTestPlugApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestPlugApp construction

CTestPlugApp::CTestPlugApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTestPlugApp object

CTestPlugApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTestPlugApp initialization

BOOL CTestPlugApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}

void LogCallInfo(char * sBuffer1, char * sBuffer2, int iCallNUM, int i)
{
	FILE * fp = NULL;
	char writebuf[2048];
	CTime cTime;
	CString csbuf;
	cTime = cTime.GetCurrentTime();
	csbuf = cTime.Format("%y%m%d");
	CString csFile;
	csFile.Format("log/Auth-%s.log", csbuf);

	try
	{
		fp = fopen(csFile, "a+t");
	}
	catch(...)
	{
		return;
	}
	
	if( fp )
	{
		memset(writebuf, 0, 2048);
		csbuf = cTime.Format("%H/%M/%S");
		try
		{
			if( sBuffer1 && sBuffer2 )
				fprintf(fp, "%s:%2d %s %s %d\n", csbuf, iCallNUM, sBuffer1, sBuffer2, i);
			else if( sBuffer1 && !sBuffer2)
				fprintf(fp, "%s:%2d %s [NULL] %d\n", csbuf, iCallNUM, sBuffer1, i);
			else if( !sBuffer1 && sBuffer2)
				fprintf(fp, "%s:%2d [NULL] %s %d\n", csbuf, iCallNUM, sBuffer2, i);
		}
		catch ( CMemoryException * e)
		{
			e->Delete();
		}
		catch ( CException * e)
		{
			e->Delete();
		}
		fclose(fp);
	}
}

//extern "C" __declspec(dllexport) int WriteInfo(VOID *bill_packet, 
int WriteInfo(VOID *bill_packet, 
														int * iRetVal, 
														char sArgIn[20][128], 
														int * iMentCount, 
														int * iRetrial, 
														int * iCallState)
{
	int	iCallNum = *iMentCount;
	LogCallInfo("RegisterUID0", sArgIn[0] ,  iCallNum,0);
	* iMentCount = 0;
	return 0;
}