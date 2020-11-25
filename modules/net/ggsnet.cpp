#pragma once

#include "../stdafx.h"
#include <mtest/ggsnet.h>
#include <mtest/mtestpro.h>
#include <mtest/mtestpro_dlg.h>

void CGGSNet::PutErr(const TCHAR* aMes, int aErr)
{
    theApp.m_pDlg->PostMessage(ID_CLR_PORT_GGS);
    theApp.m_Log.PutErr(aMes, aErr, false);
}

void CGGSNet::GetVolume(BYTE& aVolumIn, BYTE& aVolumOut)
{
    aVolumIn = 7;
    aVolumOut = 7;
}

