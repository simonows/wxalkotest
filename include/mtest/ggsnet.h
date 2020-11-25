#pragma once

class CGGSNet: public CGGSV7
{
public:
    virtual void PutErr(const TCHAR* aMes = NULL, int aErr = -1);
    virtual void GetVolume(BYTE& aVolumIn, BYTE& aVolumOut);
};

