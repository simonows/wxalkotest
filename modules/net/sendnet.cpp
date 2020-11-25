#include "../stdafx.h"
#include <mtest/sendnet.h>
#include <mtest/mtestpro.h>
#include <mtest/mtestpro_dlg.h>

CSendNet::CSendNet(void)
{
}

CSendNet::~CSendNet(void)
{
}

void CSendNet::LogPut(const CIdentV7& aID, DWORD aAddr, int aPort, const BYTE* aMes, int aLen)
{
    if (theApp.m_IsLog)
    {
        theApp.m_Log.PutMes(_T(">"), aMes, aLen);
    }
}

void CSendNet::BadPut(const TCHAR* aMes)
{
    theApp.m_Log.Put(aMes);
}

void CSendNet::BadPutErr(const TCHAR* aMes, int aErr)
{
    theApp.m_Log.PutErr(aMes, aErr);
}

void CSendNet::BadPutMes(const TCHAR* aCnt, const BYTE* aPack, int aLen)
{
    theApp.m_Log.PutMes(aCnt, aPack, aLen);
}

CReceivV7* CSendNet::GetReceiv()
{
    return &(theApp.m_Receiv);
}

void CSendNet::IsNoLink()
{
    theApp.m_pDlg->PostMessage(NO_LINK);
}

