#pragma once

/////////////////////////////////////////////////////////////////////////////
// идентификатор устройств v7

class CIdentV7
{
	int		m_Node;	// номер узла
	int		m_Dev;	// номер устройства в узле

public:
	enum
	{
		minNode		= 1,
		minNodeSpec	=  9999900, //+ специальные идентификаторы для нестандартных связей ЛБ
		maxNodeSpec	=  9999915,	//-
		minNodeProg	= 10000000,
		maxNode		= 0xFEFFFF-1,
		minDev		= 0,
		maxDev		= 255,
	};

	CIdentV7(void);
	CIdentV7(const BYTE* aIdent)	{ SetValue(aIdent);};
	CIdentV7(int aNode, int aDev);
	~CIdentV7(void);

	void		Clear();
	bool		SetUnique();
	bool		Set(int aNode, int aDev);
	bool		SetNode(int aNode);
	bool		SetDev(int aDev);
	int			GetNode() const;
	int			GetDev() const;
	void		GetValue(BYTE* aBuf) const;
	void		SetValue(const BYTE* aBuf);
	CAtlString	GetString() const;
	bool		SetString(const TCHAR* aStr);
	bool		IsEmpty() const	{ return GetNode()==0 && GetDev()==0; };
	int			operator == (const CIdentV7& id) const;
	int			operator != (const CIdentV7& id) const;
	bool		operator < (const CIdentV7& id) const;
	bool		FullEQ(const CIdentV7& id) const;
};
