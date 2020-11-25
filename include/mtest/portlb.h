#pragma once

class CPortLB : public CPortV7
{
public:
    int m_Ident;
    CString m_MAC;
    CString m_KeyInLb;
    int m_Port;
    int m_PortGGS;
    DWORD m_IP;
    int m_TypConnect;

    CPortLB(void);
    ~CPortLB(void);

    virtual void PutErr(const TCHAR* aMes = NULL, int aErr = -1);
    virtual void MngPack(CPackV7& aPack);
    virtual void PutLogInp(const BYTE* aMes, int aLen);
    virtual void PutLogOut(const BYTE* aMes, int aLen);

    bool Put(CPackV7& aPack);
    void SendAsk();
};

