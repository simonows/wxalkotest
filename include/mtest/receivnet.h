#pragma once

class CReceivNet : public CReceivV7
{
public:
    CReceivNet(void);
    ~CReceivNet(void);

    virtual void LogPut(const CSendV7* pSender, DWORD aAddr, int aPort, const BYTE* aMes, int aLen);
    virtual void BadPut(const TCHAR* aMes, bool isNeedInform = false);
    virtual void BadPutErr(const TCHAR* aMes = NULL, int aErr = -1);
    virtual void BadPutMes(const TCHAR* aCnt, const BYTE* aPack, int aLen, bool isNeedInform = false);
    virtual void HandlingData(const CPackV7& aPack, const CSendV7* pSender);
    virtual CSendV7 *GetSend(const CIdentV7& aID);
    virtual bool ConnReset(const TCHAR* aIpPort, int aErr);
};

