#pragma once

#include <Pult.h>

/////////////////////////////////////////////////////////////////////////////
// класс всех пультов (подсетей)

class CDisp
{
public:
	CPultTab	m_TabPult;		// таблица пультов (подсетей)
	CString		m_sNaim;		// наименование
	int			m_Number;		// номер
	CParam*		m_pParam;		// дополнитльеные параметры

	CComPtr<IXMLDOMDocument>	m_XMLDOM;
	CComPtr<IXMLDOMNode>		m_XMLNode;
	bool						m_FlagModify;

	CDisp();
	~CDisp();

	virtual	void	Put(CPult* aPult, CPackMes& aPack) = 0;
	virtual	void	PutNoIdent(CPult* aPult, CPackMes& aPack) {};
	virtual	void	PutDirect(CPult* aPult, CPackMes& aPack) {};
	virtual	void	PutDirectNoIdent(CPult* aPult, CPackMes& aPack) {};
	virtual	bool	Get(CPult* aPult, CPackMes& aPack) { return false; };
	virtual	bool	Mng(CPult* aPult, CPackMes& aPack) { return true; };
	virtual	void	SendAlert(CPult* aPult, int aUmOrig, int aSlOrig, CMes::typAlert aTypAlert, const BYTE* aBody = NULL, int aLen = 0) {};
	virtual	CSlot*	RootSlot(CPult* aPult, int aNumUM);
	virtual	void	PutLog(const TCHAR* aMsg, bool aMessageBox = false) {};

	void	XMLInit();
	void	XMLRelease();
	void	XMLSaveStr(CComBSTR& aXmlStr);
	void	XMLSaveFile(const TCHAR* aFileName, bool aIsUnicode = false);
	void	XMLLoad();
	void	XMLLoadStr(const TCHAR* aXmlStr);
	void	XMLLoadFile(const TCHAR* aFileName);

	void	XMLUpdate();
	void	XMLAdd();
	void	XMLLoad(CComPtr<IXMLDOMNode> spNode);
	void	XMLPostLoad();

	void	Clear();
	bool	IsModify();
	void	ClearModify();
	void	ReOrder()	{m_TabPult.ReOrder();};	// сортировка
	int		SetLink();
	CPult*	FindPult(int aSubNet);											// поиск подсети
	CUM*	FindUM(int aSubNet, int aNumUM);								// поиск УМ
	CSlot*	FindBlok(int aSubNet, int aNumUM, int aNumSLOT);				// поиск КЛШ
	CDev*	FindDev(int aSubNet, int aNumUM, int aNumSLOT, int aAdrBus);	// поиск устройства

	static	const	TCHAR* s_Disp;
	static	const	TCHAR* s_Number;
	static	const	TCHAR* s_Name;
};
