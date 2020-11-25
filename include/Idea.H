#pragma once

/////////////////////////////////////////////////////////////////////////////
// шифрование IDEA

class CIdeaCtx
{
public:
	enum
	{
		IDEAKEYSIZE		= 16,
		IDEAROUNDS		= 8,
		IDEAKEYLEN		= 6*IDEAROUNDS+4,
	};
	USHORT m_Ek[IDEAKEYLEN];
	USHORT m_Dk[IDEAKEYLEN];

	static  USHORT mulInv(USHORT x);
	static  void Cipher(const BYTE *inbuf, BYTE *outbuf, USHORT *key);
	static  void ExpandKey(const BYTE* userkey, USHORT* EK);
	void InvertKey(const USHORT* EK);
public:
	CIdeaCtx() {};
	CIdeaCtx(const BYTE *key) { Key(key);};
	void Key(const BYTE *key);
	void Encrypt(const BYTE *inp, BYTE *out, int blocks);
	void Decrypt(const BYTE *inp, BYTE *out, int blocks);

};

/////////////////////////////////////////////////////////////////////////////
// класс ключа

class CIdeaKey
{
	enum
	{
		lKey	= 16,
	};
	BYTE	m_Buf[lKey];
public:
	CIdeaKey()							{ memset(m_Buf, 0,    lKey); };
	CIdeaKey(const BYTE* aKey)			{ memcpy(m_Buf, aKey, lKey); };
	CIdeaKey(const CIdeaKey& aKey)		{ memcpy(m_Buf, static_cast<const BYTE*>(aKey), lKey); };
	CIdeaKey(const TCHAR* aName, const TCHAR* aPassWD, const BYTE* aMask)	{ Set(aName, aPassWD, aMask); };
	CIdeaKey(const TCHAR* aName, const TCHAR* aProfil, const TCHAR* aPassWD, const BYTE* aMask)	{ Set2(aName, aProfil, aPassWD, aMask); };

	operator	const	BYTE *() const	{ return(m_Buf);};
	void		operator  = (const CIdeaKey& aKey)	{ memcpy(m_Buf, static_cast<const BYTE*>(aKey), lKey); };

	void Set(const TCHAR* aName, const TCHAR* aPassWD, const BYTE* aMask);
	void Set2(const TCHAR* aName, const TCHAR* aProfil, const TCHAR* aPassWD, const BYTE* aMask);

	static	unsigned int rand();

	static	unsigned int s_NumRand;
};
