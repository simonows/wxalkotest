#pragma once

class CSendNet : public CSendV7
{
public:
    CSendNet(void);
    ~CSendNet(void);

    virtual void LogPut(const CIdentV7& aID, DWORD aAddr, int aPort, const BYTE* aMes, int aLen);
    virtual void BadPut(const TCHAR* aMes);
    virtual void BadPutErr(const TCHAR* aMes = NULL, int aErr = -1);
    virtual void BadPutMes(const TCHAR* aCnt, const BYTE* aPack, int aLen);
    virtual void IsNoLink();
    virtual CReceivV7 *GetReceiv();
};

