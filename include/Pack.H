#pragma once

#include <TIME.H>

#include <Mes.H>

/////////////////////////////////////////////////////////////////////////////
// класс пакета обмена
//

class CPackMes
{
	int		m_iIndex;
public:
	enum maxLen
	{
		mxLen = 63
	};
	int				m_iLen;
	int				m_iReply;
	int				m_iIdent;
	bool			m_bIdent;
	BYTE			m_bBuf[mxLen+1];
	BYTE			m_bReply[mxLen+1];
	COleDateTime	m_InsTime;		// время вставки пакета в очередь
	BYTE			m_Prior;		// приоритет пакета
	int				m_SubNet;		// номер подсети

	CPackMes();
	CPackMes(int len, const BYTE* buf, int aPrior = 255);
	//CPackMes(CPackMes& pk) {*this = pk;};
	CPackMes(const CPackMes& pk) {*this = pk;};
	CPackMes(int aOrig, int aDest, int aPrior = 255);

	int			GetOrig();
	int			GetDest();
	void		SetDest(int aDest);
	bool		IsValid();
	bool		IsEmpty();
	bool		IsNoMes();							// в пакете отсутствуют сообщения
	void		UnPack();							// разобрать пакет
	void		Clear();							// очистка пакета
	void		BegGet();							// передвинуть указатель чтения в начало
	void		SetAddr(int aOrig, int aDest);		// запись адреса диспетчерской и собственного
	void		SetIdent(int aIdent);				// установить идентификатор пакета							
	void		DelIdent();							// удалить идентификатор пакета	
	void		SetPrior(int aPrior);				// установить приоритет пакета
	bool		GetMes(CMes& aMes);
	void		SetOrigMes(int aUM, int aSlot);		// запись в сообщения пакета вместа FF:F отправителя (УМ и слот диспетчерской)
	bool		AddMes(const CMes& aMes);
	void		AddIdent(int aIdent);				// добавление идентификатора пакета
	bool		AddRepl(BYTE aRepl);				// добавление подтверждений на пакеты
	void		CalcCrc();
	bool		Link(CPackMes& pk);					// проверка и при возможности слияние пакетов
	int			Len()	{ return(m_iLen);};
	CAtlString	Str(const TCHAR* aTitle = 0);

	operator const	BYTE *() const	{ return(m_bBuf);};
	void operator  = (const CPackMes& pk);

	static BYTE CalcCrc (const BYTE* mes, int len);
};

