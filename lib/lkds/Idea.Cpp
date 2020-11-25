#include "stdafx.h"

#include <Idea.h>

/////////////////////////////////////////////////////////////////////////////
// шифрование IDEA

#define MUL(x,y) \
	((t16 = (y)) ? \
		(x) ? \
			t32 = (ULONG)x*t16, \
			x = t32, \
			t16 = t32>>16, \
			x = (x-t16)+(x<t16) \
			: \
				(x = 1-t16) \
		: \
 			(x = 1-x))

#define SWAB(x) ((x >>8) | (x<<8))

void CIdeaCtx::Cipher(const BYTE *inbuf, BYTE *outbuf, USHORT *key)
{
 	register USHORT x1, x2, x3, x4, s2, s3;
	const	USHORT*	in  = reinterpret_cast<const USHORT*>(inbuf); 
	USHORT*			out = reinterpret_cast<USHORT*>(outbuf);
	register USHORT t16;
	register ULONG t32;
	int		 r = IDEAROUNDS;

	x1 = *in++; 
	x2 = *in++;
	x3 = *in++; 
	x4 = *in;

	x1 = SWAB(x1);
	x2 = SWAB(x2);
	x3 = SWAB(x3);
	x4 = SWAB(x4);

	do 
	{
		MUL(x1,*key++);
		x2 += *key++;
		x3 += *key++;
		MUL(x4,*key++);

		s3 = x3;
		x3 ^= x1;
		MUL(x3,*key++);
		s2 = x2;
		x2 ^= x4;
		x2 += x3;
		MUL(x2,*key++);
		x3 += x2;

		x1 ^= x2; 
		x4 ^= x3;
		x2 ^= s3; 
		x3 ^= s2;
	} 
	while(--r);

	MUL(x1,*key++);
	x3 += *key++;
	x2 += *key++;
	MUL(x4,*key++);

	*out++ = SWAB(x1);
	*out++ = SWAB(x3);
	*out++ = SWAB(x2);
	*out =   SWAB(x4);

}

USHORT CIdeaCtx::mulInv(USHORT x)
{
	USHORT t0, t1;
	USHORT q, y;
	
	if (x <= 1) return x;	// 0 and 1 are self-inverse

	t1 = 0x10001L /x;		// Since x>=2, this fits into 16 bits
	y  = 0x10001L % x;

	if (y == 1) return 1-t1;

	t0 = 1;

	do 
	{
		q = x / y;
		x = x % y;
		t0 += q * t1;

		if (x==1) return t0;

		q = y / x;
		y = y % x;
		t1 +=  q * t0;
	} 
	while (y != 1);

	return 1-t1;
}

void CIdeaCtx::ExpandKey(const BYTE* userkey, USHORT* EK)
{
	int i,j;

	for(j=0; j < 8; j++) 
	{
		EK[j] = (userkey[0]<<8) + userkey[1];
		userkey += 2;
	}
	for(i=0; j < IDEAKEYLEN; j++) 
	{
		i++;
		EK[i+7] = EK[i & 7] << 9 | EK[i+1 & 7] >> 7;
		EK += i & 8;
		i &= 7;
	}
}


void CIdeaCtx::InvertKey (const USHORT* EK)
{
	USHORT t1, t2, t3;
	USHORT temp[IDEAKEYLEN];
	USHORT *p = temp + IDEAKEYLEN;

	memset(temp, 0, sizeof(temp));

	t1 = mulInv(*EK++);
	t2 = -*EK++;
	t3 = -*EK++;
	*--p = mulInv(*EK++);
	*--p = t3;
	*--p = t2;
	*--p = t1;

	for (int i = 0; i < IDEAROUNDS-1; i++) 
	{
		t1 = *EK++;
		*--p = *EK++;
		*--p = t1;

		t1 = mulInv(*EK++);
		t2 = -*EK++;
		t3 = -*EK++;
		*--p = mulInv(*EK++);
		*--p = t2;
		*--p = t3;
		*--p = t1;
	}
	t1 = *EK++;
	*--p = *EK++;
	*--p = t1;

	t1 = mulInv(*EK++);
	t2 = -*EK++;
	t3 = -*EK++;
	*--p = mulInv(*EK++);
	*--p = t3;
	*--p = t2;
	*--p = t1;

	memcpy(m_Dk, temp, sizeof(temp));
	memset(temp, 0, sizeof(temp));
}

void CIdeaCtx::Key(const BYTE *key)
{
	ExpandKey(key, m_Ek);
	InvertKey(m_Ek);
}

void CIdeaCtx::Encrypt(const BYTE *inp, BYTE *out, int blocks)
{
	const BYTE* in = inp;
	BYTE*		ou = out;

	for(int i=0; i < blocks; i++, in+=8,ou+=8)
	{
		Cipher(in, ou, m_Ek);
	}
}

void CIdeaCtx::Decrypt(const BYTE *inp, BYTE *out, int blocks)
{
	const BYTE* in = inp;
	BYTE*		ou = out;

	for(int i=0; i < blocks; i++, in+=8,ou+=8)
	{
		Cipher(in, ou, m_Dk);
	}
}

/////////////////////////////////////////////////////////////////////////////
// класс ключа

unsigned int CIdeaKey::s_NumRand;

void CIdeaKey::Set(const TCHAR* aName, const TCHAR* aPassWD, const BYTE* aMask)
{
	CStringA	NameA(aName);
	CStringA	PassWDA(aPassWD);

	memcpy(m_Buf, aMask, lKey);

	{
		int	Len	= min(sizeof(m_Buf), _tcsclen(aName));
		int	Ful	= Len/lKey;
		int	Ost	= Len%lKey;
		int Index(0);

		for(int i = 0; i < Ful; i++)
			for(int j = 0; j < lKey; j++)
				m_Buf[j]	^= NameA[Index++];

		for(int j = 0; j < Ost; j++)
			m_Buf[j]	^= NameA[Index++];
	}
	{
		int	Len	= min(sizeof(m_Buf), _tcsclen(aPassWD));
		int	Ful	= Len/lKey;
		int	Ost	= Len%lKey;
		int Index(Len);

		for(int i = 0; i < Ful; i++)
			for(int j = lKey-1; j >= 0; j--)
				m_Buf[j]	^= PassWDA[--Index];

		for(int j = lKey-1; j >= lKey-Ost; j--)
			m_Buf[j]	^= PassWDA[--Index];
	}
}

void CIdeaKey::Set2(const TCHAR* aName, const TCHAR* aProfil, const TCHAR* aPassWD, const BYTE* aMask)
{
	CStringA	NameA(aName);
	CStringA	PassWDA(aPassWD);

	NameA	+= CStringA(aProfil);

	memcpy(m_Buf, aMask, lKey);

	{
		int	Len	= NameA.GetLength();
		int	Ful	= Len/lKey;
		int	Ost	= Len%lKey;
		int Index(0);

		for(int i = 0; i < Ful; i++)
			for(int j = 0; j < lKey; j++)
				m_Buf[j]	^= NameA[Index++];

		for(int j = 0; j < Ost; j++)
			m_Buf[j]	^= NameA[Index++];
	}
	{
		int	Len	= min(sizeof(m_Buf), _tcsclen(aPassWD));
		int	Ful	= Len/lKey;
		int	Ost	= Len%lKey;
		int Index(Len);

		for(int i = 0; i < Ful; i++)
			for(int j = lKey-1; j >= 0; j--)
				m_Buf[j]	^= PassWDA[--Index];

		for(int j = lKey-1; j >= lKey-Ost; j--)
			m_Buf[j]	^= PassWDA[--Index];
	}
}

unsigned int CIdeaKey::rand()
{
	return ::rand();
}

