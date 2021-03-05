#ifndef _OPERATE_H
#define _OPERATE_H
#include "SStack.h"

#define LOG_TTL_TID				73777
#define LOG_TTL_TVAL			500

extern HAPP hApp;
BOOL bReleaseGlobalMemory();
BOOL bInitGlobalMemory();
BOOL ParseConfigure(HWND hWnd);
BOOL init_call_stack(int iMaxValue);
int GetKeyLock();
void EndOfOperate();
BOOL registerGlobalMessage();
int	GetScenarioItem(int iPage, int iNum,SCENARIO_STEP &s);
int	GetScenarioCount(int iPage);
int CompareSceName(int iPage, char *fName);
SStack * GetStackPoint(int iPage);
int GetItemId(int iPage, int iCount);

void WriteLogL(IN char *sFileName,IN int a,IN char *c,IN char *b);
void InitLogVar();
void DeleteLogVar();


void mentfileList_push_back(char *file);
void mentfileList_erase(char *file);
int mentfileList_get_name(int count,char *file);
int mentfileList_size();
void mentfileList_sort();
#endif // _OPERATE_H

