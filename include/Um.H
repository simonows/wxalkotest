#pragma once

#include <Mes.H>
#include <slot.h>
#include <Param.h>

/////////////////////////////////////////////////////////////////////////////
// класс УМ, вместе с подключенными к нему КЛШ
//

class CPult;

class CUM
{
public:
	bool		m_bLink;		// признак наличия пути до УМ
	bool		m_bNoAdmin;		// признак отсутсвия администрирования
	int			m_NumUM;		// номер УМ
	CString		m_Note;			// примечание
	CPult*		m_pPult;		// указатель на пульт, к которому подключен узловой модуль
	CSlotTab	m_TabSlot;		// описание слотов УМ
	CParam*		m_pParam;		// дополнитльеные параметры

	CComPtr<IXMLDOMNode>	m_XMLNode;
	bool					m_FlagModify;

	CUM(int aNumUM, CParam* aParam) 
		: m_NumUM(aNumUM)
		, m_pPult(0)
		, m_bLink(false)
		, m_bNoAdmin(false)
		, m_FlagModify(false)
		, m_pParam(aParam) {};
	CUM() : m_pParam(NULL)	{ Clear();};
	CUM( CUM &aUM);
	~CUM();

	void	Clear();

	CSlot*	DispSlot();							// поиск слота УМ, к которому подключен компьютер
	CSlot*	FindInpLink();						// поиск входящей связи на УМ
	bool	ExistOutLink();						// имеются ли исходящие связи на УМ
	void	MarkLink();							// маркировка при наличии связи
	void	ReLink();							// изменение направления связей
	void	SetAddr();							// запись в пакеты КЛШ адреса диспетчерской и собственного
	void	GetSost()	{m_TabSlot.GetSost();};	// читать состояние КЛШ
	void	GetSostL()	{m_TabSlot.GetSostL();};// читать состояние локального КЛШ
	void	GetSostR()	{m_TabSlot.GetSostR();};// читать состояние удаленного КЛШ
	void	SetNoInp()	{m_TabSlot.SetNoInp();};// установить признак "не было данных от слота"
	void	TestOffL()	{m_TabSlot.TestOffL();};// проверка потери связи с локальным КЛШ
	void	TestOffR()	{m_TabSlot.TestOffR();};// проверка потери связи с удаленным КЛШ
	void	SetTimeConf();						// установка времени запроса конфигурации
	int		TotlDisp();							// количество слотов с терминалами
	int		TotlCLL();							// количество слотов с КСЛ
	void	SetAdmPult();						// назначить адрес диспетчерского пульта
	bool	IsLocal();							// локальный УМ
	void	ReOrder()	{m_TabSlot.ReOrder();};	// сортировка

	void	XMLRemove();
	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLPostLoad();

	static	const	TCHAR* s_Um;
	static	const	TCHAR* s_Number;
	static	const	TCHAR* s_NoAdmin;
	static	const	TCHAR* s_Note;
};

/////////////////////////////////////////////////////////////////////////////
// класс перечня УМ в системе

class CUMTab : public list<CUM*>
{
	HANDLE	m_Lock;						// блокировочное событие
public:
	CUMTab();
	~CUMTab();

	CPult*		m_pPult;										// указатель на пульт, к которому подключены УМ

	CSlot*		DispSlot();										// поиск слота УМ, к которому подключен компьютер
	CSlot*		FindBlok(int aNumUM, int aNumSLOT);				// поиск КЛШ
	CSlot*		FindBlokIns(int aNumUM, int aNumSLOT);			// поиск КЛШ, вставка, если не найден
	CUM*		FindUM(int aNumUM);								// поиск УМ
	CDev*		FindDev(int aNumUM, int aNumSLOT, int aAdrBus);	// поиск устройства
	CSlot*		RootSlot(int aNumUM);							// поиск корневого слота
	int			FindLostUM();									// поиск потерянных УМ
	void		SetAddr();										// запись в пакеты КЛШ адреса диспетчерской и собственного
	bool		Ggs(CMes::askFromMain aAsk, int aNumUM, int aNumSLOT, int dev, DWORD aIP, DWORD aPort, int aNumWavIn = -1,  int aNumWavOut = -1);
	void		GetSost();										// читать состояние КЛШ
	int			TotlDisp();										// количество слотов с терминалами
	int			SetLink();
	void		SetLink(CSlot* ItSlot, CSlot::typSlot aTypInp);
	void		SetOne(CSlot* ItSlot, CSlot::typSlot aTypInp);
	void		RenameUM(int aOldUM, int aNewUM);
	void		Wait();
	void		Release();
	void		SetAdmPult();									// назначить адрес диспетчерского пульта
	bool		Insert(CUM* aUm);
	void		ReOrder();										// сортировка по номеру ММИ
	void		Clear();
	bool		IsNoAdmin(int aNumUM);							// УМ не администрируется
};
