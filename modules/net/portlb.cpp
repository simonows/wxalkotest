#include "../stdafx.h"
#include <mtest/portlb.h>
#include <mtest/mtestpro.h>
#include <mtest/mtestpro_dlg.h>

CPortLB::CPortLB(void)
    :m_Ident(0)
    ,m_Port(0)
    ,m_PortGGS(0)
    ,m_IP(0)
    ,m_TypConnect(0)
{
}

CPortLB::~CPortLB(void)
{
}

void CPortLB::PutErr(const TCHAR* aMes, int aErr)
{
    theApp.m_pDlg->PostMessage(ID_CLR_PORT_LB);
    theApp.m_Log.PutErr(aMes, aErr, false);
}

void CPortLB::PutLogInp(const BYTE* aMes, int aLen)
{
    if (theApp.m_IsLog)
    {
        theApp.m_Log.PutMes(_T("<LB "), aMes, aLen);
    }
}

void CPortLB::PutLogOut(const BYTE* aMes, int aLen)
{
    if (theApp.m_IsLog)
    {
        theApp.m_Log.PutMes(_T(" LB>"), aMes, aLen);
    }
}

void CPortLB::MngPack(CPackV7& aPack)
{
    switch(aPack.m_ClassPack)
    {
    case CPackV7::cpData:
        {
            if (aPack.m_TypPack == CPackV7::tpDirect &&
                aPack.m_Direct == CPackV7::tdAnswer)
            {
                switch(aPack.m_TypAsk)
                {
                case CPackV7::taWho:
                    {
                        CIdentV7 Ident(aPack.Data()+3);
                        m_Ident = Ident.GetNode();
                        theApp.m_pDlg->PostMessage(ID_GET_IDENT);
                        break;
                    }
                case CPackV7::taNVRAM:
                    {
                        int Addr = (aPack.Data()[1] & 0xFF) | ((aPack.Data()[2] << 8) & 0xFF00);
                        int Len = aPack.Data()[3];

                        if (Addr == 1024)
                        {
                            m_MAC.Format(_T("%02X%02X%02X%02X%02X%02X"), aPack.Data()[5+5], aPack.Data()[5+6], aPack.Data()[5+7], aPack.Data()[5+8], aPack.Data()[5+9], aPack.Data()[5+10]);

                            if(m_MAC == _T("FFFFFFFFFFFF"))
                            {
                                m_MAC.Format(_T("%02X%02X%02X%02X%02X%02X"), aPack.Data()[5+58], aPack.Data()[5+59], aPack.Data()[5+60], aPack.Data()[5+61], aPack.Data()[5+62], aPack.Data()[5+63]);
                            }
                            m_KeyInLb.Empty();
                            
                            for(int i = 0; i < 16; i++)
                            {
                                m_KeyInLb.AppendFormat(_T("%02X"), aPack.Data()[5+11+i]);
                            }
                        }
                        else if (Addr == 1024 + 256)
                        {
                            unsigned short Port;
                            unsigned long Addr;

                            m_TypConnect = aPack.Data()[5];

                            memcpy(&Port, aPack.Data() + 5 + 1, 2);
                            m_Port = ntohs(Port);

                            memcpy(&Port, aPack.Data() + 5 + 3, 2);
                            m_PortGGS = ntohs(Port);

                            memcpy(&Addr, aPack.Data() + 5 + 5, 4);
                            m_IP = ntohl(Addr);
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
}

bool CPortLB::Put(CPackV7& aPack)
{
    aPack.SetID(CIdentV7(0,0), CIdentV7(0,0));

    return PutPort(aPack);
}

void CPortLB::SendAsk()
{
    m_Ident = 0;
    m_MAC.Empty();
    m_KeyInLb.Empty();
    m_Port = 0;
    m_PortGGS = 0;
    m_IP = 0;
    m_TypConnect = 0;

    CPackV7 Pack;
    BYTE bufOTK[2] = { 1, 1 };

    Pack.SetAsk(CPackV7::tpDirect, CPackV7::taServiceOTK, CPackV7::tdQuery, bufOTK, 2);
    Put(Pack);
    Sleep(100);

    BYTE buf[4] = { 0, 0, 64, 0};
    int Addr = 1024;

    memcpy(buf, &Addr, 2);

    Pack.SetAsk(CPackV7::tpDirect, CPackV7::taNVRAM, CPackV7::tdQuery, buf, 4);
    Put(Pack); // запрос системных параметров
    Sleep(100);

    Addr = 1024 + 256;

    memcpy(buf, &Addr, 2);

    Pack.SetAsk(CPackV7::tpDirect, CPackV7::taNVRAM, CPackV7::tdQuery, buf, 4);
    Put(Pack); // запрос собственных ETHERNET параметров
    Sleep(100);

    Pack.SetAsk(CPackV7::tpDirect, CPackV7::taWho, CPackV7::tdQuery);
    Put(Pack); // запрос Кто ты ? ЛБ
}

