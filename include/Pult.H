#pragma once

#define LKDS_ID_MNG_PACK	(WM_APP+200)
#define LKDS_ID_NEW_SOST	(WM_APP+201)

#include <UM.h>

/////////////////////////////////////////////////////////////////////////////
// фиксированные номера слотов и подсетей

enum numSlot
{
	minSlot		= 0,		// минимальный номер слота
	devSlot		= 7,		// максимальный номер аппаратного слота
	ldpSlot		= 8,		// номер слота локального диспечерского пункта
	gdpSlot		= 9,		// номер слота центрального диспетчерского пункта
	prgSlot		= 14,		// максимальный номер программного слота
	maxSlot		= 15,		// максимальный номер слота

	proSubNet	= 0x10000,	// подсеть, для представления блоков Pro в сети
};

/////////////////////////////////////////////////////////////////////////////
// класс диспетчерского пульта, вместе с подключенными к нему УМ
//

class	CDisp;

class CPult
{
public:
	int			m_RootUM;		// номер УМ, к которому подключен компьютер
	int			m_RootSLOT;		// номер слота, к которому подключен компьютер
	CUMTab		m_TabUM;		// таблица узловых модулей
	bool		m_bCross;		// использование маршрутизации
	CString		m_sNaim;		// наименование
	int			m_Number;		// номер
	CParam*		m_pParam;		// дополнитльеные параметры
	CDisp*		m_pDisp;		// указатель на диспетчерскую, к которой подключен пульт
	bool		m_bIsDefault;	// признак подсети по умолчанию

	CComPtr<IXMLDOMNode>		m_XMLNode;
	bool						m_FlagModify;


	CPult(int aNumPult);
	~CPult();

	void	InitEmpty(int aUMDisp = 0, int aSlotDisp = 8);
	void	ReLink();							// изменение направления связей
	void	ReOrder()	{m_TabUM.ReOrder();};	// сортировка

	void	XMLRelease();
	void	XMLRemove();
	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLPostLoad();

	bool	IsModify();
	void	ClearModify();

	static	const	TCHAR* s_Pult;
	static	const	TCHAR* s_Number;
	static	const	TCHAR* s_Name;
	static	const	TCHAR* s_Default;
};

/////////////////////////////////////////////////////////////////////////////
// все пульты (подсети),

class CPultTab : public list<CPult*>
{
public:
	~CPultTab()	{ Clear(); };
	CPult*	FindPult(int aNumber);		// поиск пульта
	bool	Insert(CPult* aPult);
	void	ReOrder();					// сортировка по номеру пульта
	void	Clear();
};
