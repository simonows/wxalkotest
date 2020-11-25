#include "../stdafx.h"
#include <mtest/receivnet.h>
#include <mtest/mtestpro.h>
#include <mtest/mtestpro_dlg.h>

CReceivNet::CReceivNet(void)
{
}

CReceivNet::~CReceivNet(void)
{
}

bool CReceivNet::ConnReset(const TCHAR* aIpPort, int aErr)
{
    theApp.m_Log.Put(CString(aIpPort)+_T("Порт недоступен"));

    return true;
}

void CReceivNet::LogPut(const CSendV7* pSender, DWORD aAddr, int aPort, const BYTE* aMes, int aLen)
{
    if(theApp.m_IsLog) theApp.m_Log.PutMes(_T("<"), aMes, aLen);
}

void CReceivNet::BadPut(const TCHAR* aMes, bool isNeedInform)
{
    theApp.m_Log.Put(aMes);
}

void CReceivNet::BadPutErr(const TCHAR* aMes, int aErr)
{
    theApp.m_Log.PutErr(aMes, aErr, false);
    theApp.m_pDlg->PostMessage(ERR_SOCKET);
}

void CReceivNet::BadPutMes(const TCHAR* aCnt, const BYTE* aPack, int aLen, bool isNeedInform)
{
    theApp.m_Log.PutMes(aCnt, aPack, aLen);
}

void CReceivNet::HandlingData(const CPackV7& aPack, const CSendV7* pSender)
{
    theApp.m_pDlg->m_InpQueue.Put(aPack);
    theApp.m_pDlg->PostMessage(ID_EXISTS_PACK);
}

CSendV7* CReceivNet::GetSend(const CIdentV7& aID)
{
    return (theApp.m_Send.m_Ident.GetNode() == aID.GetNode() || aID.IsEmpty()) ? &(theApp.m_Send) : 0;
}

