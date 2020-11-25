#include "StdAfx.h"

#include <PackV7.h>
#include <Idea.h>

/////////////////////////////////////////////////////////////////////////////
// пакет устройств v7

const BYTE	CPackV7::DefaultKey[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10};
const BYTE	CPackV7::ForKeySYS[16]	= {0xD4,0x43,0xDA,0xC6,0x4E,0x93,0x43,0xEA,0xAD,0xA3,0xE5,0x5E,0xFA,0xD8,0x10,0x6E};

CPackV7::CPackV7()
{
	Clear();
}

CPackV7::CPackV7(const CIdentV7& aOrig, const CIdentV7& aDest)
{
	Clear();

	m_ClassPack	= cpData;
	m_Orig		= aOrig;
	m_Dest		= aDest;

	Pack();
}

CPackV7::CPackV7(const CMes& aMes)
{
	Clear();
	SetMes(aMes);
}

CPackV7::CPackV7(int len, const BYTE* buf)
{
	Clear();

	m_Len = min(mxLen, len);

	memcpy(m_Buf, buf, m_Len);

	UnPack();
}


void	CPackV7::SetID(const CIdentV7& aOrig, const CIdentV7& aDest)
{
	m_Orig	= aOrig;
	m_Dest	= aDest;

	m_Orig.GetValue(&m_Buf[3]);
	m_Dest.GetValue(&m_Buf[7]);
}


void	CPackV7::Clear()
{
	m_Len	= 0;
	memset(m_Buf, 0, sizeof(m_Buf));

	m_ClassPack		= cpNone;
	m_TypPack		= tpNone;
	m_TypAsk		= taNone;
	m_Direct		= tdQuery;
	m_Ident			= 0;
	m_Reply			= 0;
	m_Retrans		= 0;
	m_Orig.Clear();
	m_Dest.Clear();
	m_TypMes		= CMes::tmNULL;		// тип инкапсурируемого сообщения
	m_Ask			= CMes::amNULL;
	m_NumDev		= 0;
	m_bIsValid		= true;
	m_bIsSubscrib	= false;
	m_InsTime.SetStatus(COleDateTime::null);
}

void	CPackV7::UnPack()
{
	if(LenCRC() >= minLen)
	{
		m_ClassPack	= static_cast<classPack>(m_Buf[0]);
		m_Ident		= m_Buf[1];
		m_Reply		= m_Buf[2];
		m_Orig.SetValue(&m_Buf[3]);
		m_Dest.SetValue(&m_Buf[7]);
		m_Retrans	= m_Buf[11];
		m_TypPack	= static_cast<typPack>(m_Buf[12]&0x7F);
		m_bIsValid	= true;

		switch(m_ClassPack)
		{
		case cpData:
			{
				switch(m_TypPack)
				{
				case tpMsgCLB:
					{
						if(m_Len >= 18)
						{
							m_TypMes	= static_cast<CMes::typMes>(m_Buf[13]);
							int	Ask		= 0;
							CMes::GetAdr(&m_Buf[13], m_NumDev, Ask);
							m_Ask	= static_cast<CMes::askFromMain>(Ask);

							if( m_TypMes == CMes::tmSostUnitD || m_TypMes == CMes::tmGetSostD)
							{
								if( (m_NumDev&0x80) != 0)
								{
									m_NumDev &= 0x7F;
									m_Ask = static_cast<CMes::askFromMain>(m_Ask | 0x10);
								}
							} 
						}
						else
						{
							m_bIsValid	= false;
						}
						break;
					}
				case tpDirect:
				case tpVideo:
					{
						m_TypAsk	= static_cast<typAsk>(m_Buf[13]&0x7F);
						m_Direct	= (m_Buf[13]&0x80)==0? tdAnswer: tdQuery;
						break;
					}
				}
				break;
			}
		case cpLink:
			{
				switch(m_TypPack)
				{
				case tpICMP:
					{
						break;
					}
				}
				break;
			}
		case cpVoice:
			{
				break;
			}
		}
	}
	else
	{
		m_bIsValid	= false;
	}
	return;
}

void	CPackV7::Pack()
{
	m_Buf[0]	= m_ClassPack;
	m_Buf[1]	= m_Ident;
	m_Buf[2]	= m_Reply;
	m_Orig.GetValue(&m_Buf[3]);
	m_Dest.GetValue(&m_Buf[7]);
	m_Buf[11]	= m_Retrans;
	m_Buf[12]	= m_TypPack;

	m_Len		= 13;
}

bool	CPackV7::SetMes(const CMes& mes)
{
	m_ClassPack	= cpData;
	m_TypPack	= tpMsgCLB;

	Pack();

	m_Buf[13] = mes.Len()+1;
	
	memcpy(&m_Buf[14], mes, mes.Len());

	m_Len	= 13 + 1 + mes.Len();

	return true;
}

bool	CPackV7::SetLink(typPack aTypPack, const BYTE* aBuf, int aLen)
{
	if(aLen + 14 < mxLen)
	{
		m_ClassPack	= cpLink;
		m_TypPack	= aTypPack;

		Pack();

		memcpy(&m_Buf[13], aBuf, aLen);

		m_Len	= 13 + aLen;

		return true;
	}
	else
	{
		return false;
	}
}
bool	CPackV7::SetVoice(typPack aTypPack, const BYTE* aBuf, int aLen)
{
	if(aLen + 14 < mxLen)
	{
		m_ClassPack	= cpVoice;
		m_TypPack	= aTypPack;

		Pack();

		memcpy(&m_Buf[13], aBuf, aLen);

		m_Len	= 13 + aLen;

		return true;
	}
	else
	{
		return false;
	}
}

void	CPackV7::SetVoiceToLU(const BYTE* aBuf)
{
	m_ClassPack	= cpVoice;
	m_TypPack	= tpG723;

	Pack();

	memcpy(&m_Buf[13], aBuf, lenVoice);
	m_Buf[13+lenVoice]	= sdToLift;

	m_Len	= 13 + lenVoice + 1;
}

bool	CPackV7::SetAsk(typPack aTypPack, typAsk aTypAsk, typDirAsk aDirect, const BYTE* aBuf, int aLen)
{
	if(aLen + 15 < mxLen)
	{
		m_ClassPack	= cpData;
		m_TypPack	= aTypPack;
		m_TypAsk	= aTypAsk;
		m_Direct	= aDirect;

		Pack();

		m_Buf[13]	= m_TypAsk|(m_Direct==tdAnswer?0: 0x80);
		if(aBuf) memcpy(&m_Buf[14], aBuf, aLen);

		m_Len	= 14 + aLen;

		return true;
	}
	else
	{
		return false;
	}
}

bool	CPackV7::SetAskData(const BYTE* aBuf, int aLen)
{
	if(aLen + 15 < mxLen)
	{
		if(aLen > 0 && aBuf)
		{
			memcpy(&m_Buf[14], aBuf, aLen);
			m_Len	= 14 + aLen;
		}
		else
		{
			m_Len	= 14;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool	CPackV7::GetMes(CMes& mes) const
{
	mes.Clear();

	if(LenCRC() > minLen)
	{
		int	LenMes = m_Buf[13];

		if( LenMes+minLen == LenCRC())
		{
			mes.SetDann(LenMes-1, &m_Buf[14]);
			return true;
		}
	}
	return false;
}

bool	CPackV7::IsOnOffGGS() const
{
	if(m_ClassPack == cpData && m_TypPack == tpMsgCLB)
	{
		CMes	Mes;

		if(GetMes(Mes))	return Mes.m_iTypMes == CMes::tmSetSost;
	}

	return false;
}

void	CPackV7::SetIdent(int aIdent)
{
	m_Ident		= aIdent;
	m_Buf[1]	= aIdent;
}

void	CPackV7::DelIdent()
{
	SetIdent(0);
}

void CPackV7::operator  = (const CPackV7& pk)
{
	memcpy(m_Buf, pk.m_Buf, sizeof(m_Buf));
	m_Len			= pk.m_Len;		

	m_ClassPack		= pk.m_ClassPack;
	m_TypPack		= pk.m_TypPack;
	m_TypAsk		= pk.m_TypAsk;
	m_Direct		= pk.m_Direct;
	m_Ident			= pk.m_Ident;
	m_Reply			= pk.m_Reply;
	m_Orig			= pk.m_Orig;
	m_Dest			= pk.m_Dest;
	m_Retrans		= pk.m_Retrans;
	m_TypMes		= pk.m_TypMes;
	m_Ask			= pk.m_Ask;
	m_NumDev		= pk.m_NumDev;
	m_bIsValid		= pk.m_bIsValid;
	m_bIsSubscrib	= pk.m_bIsSubscrib;
}

static const BYTE Step3[256]= {
   0x00, 0x43, 0x86, 0xC5, 0x4F, 0x0C, 0xC9, 0x8A,
   0x9E, 0xDD, 0x18, 0x5B, 0xD1, 0x92, 0x57, 0x14,
   0x7F, 0x3C, 0xF9, 0xBA, 0x30, 0x73, 0xB6, 0xF5,
   0xE1, 0xA2, 0x67, 0x24, 0xAE, 0xED, 0x28, 0x6B,
   0xFE, 0xBD, 0x78, 0x3B, 0xB1, 0xF2, 0x37, 0x74,
   0x60, 0x23, 0xE6, 0xA5, 0x2F, 0x6C, 0xA9, 0xEA,
   0x81, 0xC2, 0x07, 0x44, 0xCE, 0x8D, 0x48, 0x0B,
   0x1F, 0x5C, 0x99, 0xDA, 0x50, 0x13, 0xD6, 0x95,
   0xBF, 0xFC, 0x39, 0x7A, 0xF0, 0xB3, 0x76, 0x35,
   0x21, 0x62, 0xA7, 0xE4, 0x6E, 0x2D, 0xE8, 0xAB,
   0xC0, 0x83, 0x46, 0x05, 0x8F, 0xCC, 0x09, 0x4A,
   0x5E, 0x1D, 0xD8, 0x9B, 0x11, 0x52, 0x97, 0xD4,
   0x41, 0x02, 0xC7, 0x84, 0x0E, 0x4D, 0x88, 0xCB,
   0xDF, 0x9C, 0x59, 0x1A, 0x90, 0xD3, 0x16, 0x55,
   0x3E, 0x7D, 0xB8, 0xFB, 0x71, 0x32, 0xF7, 0xB4,
   0xA0, 0xE3, 0x26, 0x65, 0xEF, 0xAC, 0x69, 0x2A,
   0x3D, 0x7E, 0xBB, 0xF8, 0x72, 0x31, 0xF4, 0xB7,
   0xA3, 0xE0, 0x25, 0x66, 0xEC, 0xAF, 0x6A, 0x29,
   0x42, 0x01, 0xC4, 0x87, 0x0D, 0x4E, 0x8B, 0xC8,
   0xDC, 0x9F, 0x5A, 0x19, 0x93, 0xD0, 0x15, 0x56,
   0xC3, 0x80, 0x45, 0x06, 0x8C, 0xCF, 0x0A, 0x49,
   0x5D, 0x1E, 0xDB, 0x98, 0x12, 0x51, 0x94, 0xD7,
   0xBC, 0xFF, 0x3A, 0x79, 0xF3, 0xB0, 0x75, 0x36,
   0x22, 0x61, 0xA4, 0xE7, 0x6D, 0x2E, 0xEB, 0xA8,
   0x82, 0xC1, 0x04, 0x47, 0xCD, 0x8E, 0x4B, 0x08,
   0x1C, 0x5F, 0x9A, 0xD9, 0x53, 0x10, 0xD5, 0x96,
   0xFD, 0xBE, 0x7B, 0x38, 0xB2, 0xF1, 0x34, 0x77,
   0x63, 0x20, 0xE5, 0xA6, 0x2C, 0x6F, 0xAA, 0xE9,
   0x7C, 0x3F, 0xFA, 0xB9, 0x33, 0x70, 0xB5, 0xF6,
   0xE2, 0xA1, 0x64, 0x27, 0xAD, 0xEE, 0x2B, 0x68,
   0x03, 0x40, 0x85, 0xC6, 0x4C, 0x0F, 0xCA, 0x89,
   0x9D, 0xDE, 0x1B, 0x58, 0xD2, 0x91, 0x54, 0x17   };

BYTE	CPackV7::CalcCrc(const BYTE* aBody, int aLen)
{
	BYTE	CRC = 0;

	for(int i = 0; i < aLen; i++)
		CRC = Step3[(CRC ^ aBody[i]) & 0xFF];

	return CRC;
}

unsigned short CPackV7::CalcCrc16(const BYTE* aBody, int aLen)
{
    unsigned short crc = 0;

    for( int j = 0; j < aLen; j++ )
    {
        crc = crc^static_cast<unsigned short>(aBody[j]) << 8;

        for(int i = 0; i < 8; i++)
        {
            crc = (crc&0x8000)? crc << 1^0x1021: crc << 1;
        }
    }
	return crc;
}

void CPackV7::CalcCrc()
{
	unsigned short CRC = CalcCrc16(m_Buf, m_Len);

	memcpy(&m_Buf[m_Len], &CRC, 2);
}

int	CPackV7::ChanCod(BYTE* aBuf, int aMaxLen, const BYTE* KeySys)
{
	//memcpy(aBuf, m_Buf, m_Len+3);
	//return m_Len+3;

	BYTE	KeyPACK		= ::rand()&0xFF;
	BYTE	SysKey1		= 0;
	BYTE	SysKey2		= KeyPACK;
	int		IndxKeySys	= SysKey2;

	CalcCrc();

	for(int i = 0; i < 2; i++)
	{
		bool	High	= (IndxKeySys&0x80) != 0;

		IndxKeySys	<<= 1;
		if(High)	IndxKeySys	|= 0x01;
	}
	IndxKeySys	&= 0x0F;

	int	Len = min(LenCRC(), aMaxLen);

	for(int i = 0; i < Len; i++)
	{
		SysKey1	= KeySys[IndxKeySys];

		aBuf[i]	= m_Buf[i]^SysKey2;

		SysKey2	= aBuf[i];

		aBuf[i]	= aBuf[i]^SysKey1;

		IndxKeySys	= (aBuf[i]+1)&0x0F;
	}

	SysKey2		= 1;
	IndxKeySys	= 4;

	if(aMaxLen >= LenCRC()+1)
	{
		aBuf[Len] = CalcCrc(aBuf, Len)^KeyPACK;
		Len++;

		for(int i = Len-1; i >= 0; i--)
		{
			SysKey1	= Step3[KeySys[IndxKeySys]];

			aBuf[i]	= aBuf[i]^SysKey2;

			SysKey2	= aBuf[i];

			aBuf[i]	= aBuf[i]^SysKey1;

			IndxKeySys	= (IndxKeySys+1)&0x0F;
		}
	}
	return Len;
}


bool CPackV7::ChanDecod(const BYTE* aBuf, int aLen, const BYTE* KeySys)
{
	//Clear();
	//m_Len = aLen - 3;
	//memcpy(m_Buf, aBuf, m_Len);
	//UnPack();
	//return true;

	BYTE	SysKey1		= 0;
	BYTE	SysKey2		= 1;
	int		IndxKeySys	= 4;

	Clear();

	m_Len = min(mxLen, aLen);

	memcpy(m_Buf, aBuf, m_Len);

	m_Len--;

	for(int i = m_Len; i >= 0; i--)
	{
		SysKey1		= Step3[KeySys[IndxKeySys]];

		m_Buf[i]	= m_Buf[i]^SysKey1;

		BYTE	SaveDATA	= m_Buf[i];

		m_Buf[i]	= m_Buf[i]^SysKey2;

		SysKey2		= SaveDATA;

		IndxKeySys	= (IndxKeySys+1)&0x0F;
	}

	SysKey2		= CalcCrc(m_Buf, m_Len)^m_Buf[m_Len];
	IndxKeySys	= SysKey2;

	for(int i = 0; i < 2; i++)
	{
		bool	High	= (IndxKeySys&0x80) != 0;

		IndxKeySys	<<= 1;
		if(High)	IndxKeySys	|= 0x01;
	}
	IndxKeySys	&= 0x0F;

	for(int i = 0; i < m_Len; i++)
	{
		SysKey1	= KeySys[IndxKeySys];

		IndxKeySys	= (m_Buf[i]+1)&0x0F;

		m_Buf[i]	= m_Buf[i]^SysKey1;

		BYTE	SaveDATA	= m_Buf[i];

		m_Buf[i]	= m_Buf[i]^SysKey2;

		SysKey2		= SaveDATA;
	}

	m_Len -= 2;

	UnPack();

	return TestCRC();
}

bool	CPackV7::TestCRC() const
{
//	return (CalcCrc(m_Buf, m_Len) == m_Buf[m_Len]);

	unsigned short CRC = CalcCrc16(m_Buf, m_Len);

	return memcmp(&m_Buf[m_Len], &CRC, 2) == 0;
}

bool	CPackV7::AddRepl(BYTE aReply)
{
	if(m_Reply == 0)
	{
		m_Reply		= aReply;
		m_Buf[2]	= aReply;

		return true;
	}
	else
	{
		return false;
	}
}

bool	CPackV7::IsInIdent() const
{
	return m_ClassPack == cpData && m_TypPack != tpNone;
}

bool	CPackV7::IsCloseLink() const
{
	return m_ClassPack == cpLink && m_TypPack == tpClosLink;
}

bool	CPackV7::IsRestart() const
{
	return 	m_ClassPack	== CPackV7::cpData		&&
			m_TypPack	== CPackV7::tpDirect	&&
			m_TypAsk	== CPackV7::taRestart	&&
			m_Direct	== CPackV7::tdQuery;
}

/////////////////////////////////////////////////////////////////////////////
// класс входной очереди пакетов устройств v7

InpQueuePackV7::~InpQueuePackV7() 
{
	WaitForSingleObject(m_Lock, INFINITE);

	CloseHandle(m_Lock);
}

bool InpQueuePackV7::Put(const CPackV7& aPack)
{
	bool rc = false;

	WaitForSingleObject(m_Lock, INFINITE);

	push( aPack );
	
	rc = true;

	SetEvent(m_Lock);
	return(rc);
}

bool InpQueuePackV7::Read(CPackV7& aPack)
{
	bool rc = false;

	WaitForSingleObject(m_Lock, INFINITE);

	if(!empty())
	{
		aPack = front();
		pop();

		rc = true;
	}
	SetEvent(m_Lock);
	return(rc);
}

