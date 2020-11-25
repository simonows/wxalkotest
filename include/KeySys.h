#pragma once

class CKeySys
{
	enum { len = 16,};
	BYTE	m_Buf[len];
public:

	CKeySys();
	~CKeySys(void);

	void		Clear();
	void		Create(const TCHAR* aKey);
	void		Set(const TCHAR* aKey);
	void		Set(const BYTE* aKey);
	void		SetDefault();
	CAtlString	GetString();
	bool		IsEmpty();
	operator	const	BYTE *() const	{ return(m_Buf);};
	operator	const	void *() const	{ return(m_Buf);};
	bool		operator == (const CKeySys& key) const;
	bool		operator != (const CKeySys& key) const;
};
