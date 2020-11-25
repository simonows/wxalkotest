#pragma once

#include <Mes.H>
#include <Pack.h>
#include <NvRam.H>
#include <Sost.h>
#include <SostDZ.H>
#include <Param.h>
#include <SostSens.h>
#include <InfoCLB.h>

/*
 * класс устройства на шине
*/
class CSlot;

class CDev
{
public:
    enum typDev // код типа устройства
    {
        tdNone = 0, // не определено
        tdLB30 = 1, // лифтовой блок "Обь-3.0"
        tdLB31 = 2, // лифтовой блок "Обь-3.1"
        tdLB4X = 3, // лифтовой блок "Обь-4.X"
        tdLB5X = 4, // лифтовой блок "Обь-5.X"
        tdLBE = 5, // лифтовой блок "Энергия"
        tdLBC = 6, // лифтовой блок "КДК"
        tdLBO = 7, // лифтовой блок "ОТИС"
        tdShulk = 8, // лифтовой блок "ШУЛК"
        tdUKL = 9, // лифтовой блок "УКЛ"
        tdRSUL = 10, // лифтовой блок "РСУЛ"
        tdLB6X = 11, // лифтовой блок "Обь-6.X"
        tdLB6Pro = 12, // лифтовой блок "Обь-6.Pro"
        tdLB7 = 14, // лифтовой блок "Обь-7"
        tdIT = 16, // инженерный терминал
        tdBRP = 17, // БРП
        tdIT60 = 18, // инженерный терминал v6.0
        tdIT7 = 19, // инженерный терминал v7
    };
    enum typExt // тип "Модуля расширения"
    {
        teNone = 0, // не определен

        teLCBI = 1, //LCB-I
        teLCBII = 2, //LCB-II
        teLBII = 3, //LB-II
        teRCBII = 4, //RCB-II
        teTCB = 5, //TCB
        teNCE = 17, //NCE

        te17g = 1, // ПКЛ-17 жилье
        te17a = 2, // ПКЛ-17 административный
        te17b = 3, // ПКЛ-17 больница
        te25g = 4, // ПКЛ-25 жилье
        te25a = 5, // ПКЛ-25 административный
        te25b = 6, // ПКЛ-25 больница
        te32g = 7, // ПКЛ-32 жилье
        te32a = 8, // ПКЛ-32 административный
        te32b = 9, // ПКЛ-32 больница

        teUCL = 1, // УКЛ
        tePU2 = 2, // УЛ(ПУ-2)
        tePU3 = 3, // УЛ(ПУ-3)

        teMogil = 1, // Могилевлифтмаш
        teMEL = 2, // МЭЛ
        teMPPL = 3, // МППЛ (НПО"Вектор")

        // Для ЛБ «ОБЬ6.X» тип логической платы:
        te6Load = 0, // Загрузчик.
        te6Mog = 1, // Могилевлифтмаш (УУЛ)
        te6MEL = 2, // МЭЛ (СУЛ1)
        te6MPPL = 3, // МППЛ (НПО «Вектор»)
        te6Otis = 4, // ОТИС
        te6S17 = 5, // ШУЛК17
        te6S32 = 6, // ШУЛК32
        te6UCL = 7, // УКЛ/УЛ
        te6Rele = 8, // Релейный лифт.
        te6Sodi = 9, // SODIMAS
        te6LGDI = 10, // LG_DI
        te6LGDSS = 11, // LG_DSS
        teEscal = 12, // эскалатор
        te6AXEL = 13, // AXEL
        te6ELEX = 14, // ELEX
        te6SPUL = 15, // СПУЛ
        te6THYSSEN = 16, // THYSSEN
        te6OLIMP = 17, // ОЛИМП
        te6Doppler = 18, // Doppler
        te6THYSSEN3 = 19, // THYSSEN (LS3)
        te6EXPRESS = 20, // EXPRESS
        te6BLT = 21, // BLT
        te6THYS_50 = 22, // THYSSEN (TAC50)
        te6LISA = 23, // LISA (KLEEMANN)
        te6Schind = 24, // Schindler
        te6PLK = 25, // PLK (программируемые логические контроллеры)
        te6UELR = 26, // УЭЛР
        te6THYSMCI = 27, // THYSSEN (MCI)
        te6ORONA = 28, // ORONA
        te6ARKEL = 29, // ARKEL
        te6BG15 = 30, // BG15
        te6MPPL2 = 31, // МППЛ (НПО «Вектор») версия 2
        te6VEGA = 32, // VEGA
        te6SodQI = 33, // Sodimas QI
        te6TR = 34, // ТР
        te6DMG = 35, // DMG
        te6MIK_EL = 36, // MIK_EL
        te6KONE = 37, // KONE
        te6S9 = 38, // S9
        te6HYUNDAI = 39, // HYUNDAI
        te6NICE = 40, // NICE
        te6VASSLER = 41, // VASSLER
        te6FST2 = 42, // FST2
        te6CANNY = 43, // CANNY
        te6SILVER = 44, // SILVER
        te6Invalid = 45, // ИНВАЛИДНЫЙ
        te6BETACTRL = 46, // BETACONTROL
        te6StaGeHEL = 47, // StaGeHELLAS
        te6SK6000 = 48, // ШК6000
        te6TKL = 49, // ТКЛ
        te6SUL2010 = 50, // СУЛ2010
        te6Union = 51, // Союз
        te6SH_R = 52, // ШЛ-Р
        te6FT9X0 = 53, // FT9X0
        te6LLC = 54, // LLC
        te6KOLLMORGEN = 55, // KOLLMORGEN
        te6SECURLIFT = 56,// SECURLIFT
        te6IMEM = 57, // IMEM
        te6WIPO = 58, // WIPO
        te6VESTNER = 59, // KLST (VESTNER)
        te6CARLOS_SILVA = 60, // CARLOS SILVA
        te6KOYO = 61, // KOYO
        te6BL6 = 62, // BL6
        te6WEBER = 63, // WEBER
        te6iAStar = 64, // iAStar
        te6GTE = 65, // GTE
        te6AUTINOR = 66, // AUTINOR
        te6THYSCMC = 67, // THYSCMC
        te6DAESUNG = 68, // DAESUNG
        te6AS380 = 69, // AS380
        te6ECL1 = 70, // ECL1
        te6ECLIPSE = 71, // ECLIPSE
        te6SKG = 72, // SKG
        te6ML65X = 73, // ML65X
        te6ML60X = 74, // ML60X
        te6PDAHL = 75, // PDAHL
        te6ULGM = 76, // УЛЖМ
        te6ISLv4 = 77, // ISLv4
        te6ML50 = 78, // ML50
        teViaSerie = 79, // ViaSerie
        teHidral = 86, // Hidral
        teKoneEsc = 98,
        teDigilift = 97,
        teGMV = 92,

        // Для ИТ60 тип платы расширения:
        teRS485 = 1, // RS-485
        teRS232 = 2, // RS-232
        teSPT942 = 3, // СПТ942
        teCAN = 4, // CAN
        teRS422 = 5, // RS-422
        teRS485nl = 6, // RS-485 развязанный
        teRS232nl = 7, // RS-232 развязанный
        teBufOut = 8, // буфер портов на выход
        teODM = 30,// адаптер общедомовой шины

        // Для ИТ7 тип платы расширения:
        teIT7Loader = 0, // загрузчик
        teIT70 = 1, // ИТ7.0
        teIT71Std = 2, // ИТ7.1 Стандарт
        teIT71Port = 2, // ИТ7.1 Портал
    };
    enum bitProt // код протокола удаленного терминала
    {
        tpMECS = 0x1, // протокол MECS
        tpRMEM = 0x2, // протокол RMEM
    };
    enum lvlAsk10 // уровень ответа msg26 ask10
    {
        laNone = -1, // не определен
        laClassic = 0, // стардартный
        laExtended = 1, // расширенный
        laFull = 2, // полный
    };

    int m_AdrBus; // адрес устройства на локальной шине
    COleDateTime m_LastMes; // дата прихода последнего состояния
    COleDateTime m_TimSost; // дата установки состояния
    COleDateTime m_TimRestart; // дата последнего рестарта
    int m_CodRestart; // код последнего рестарта
    typDev m_TypDev; // код типа устройства
    typExt m_TypExt; // тип "Модуля расширения"
    int m_BitProt; // Тип протокола терминада
    lvlAsk10 m_LvlAsk10; // уровень ответа msg26 ask10
    CSlot *m_pSlot; // указатель на блок, к которому подключено устройство
    CNvRam m_NvRam; // содержимое NVRAM
    COleDateTime m_TimKey; // время вставка нового ключа
    BYTE m_LvlKey; // уровень ключа
    BYTE m_PrefKey; // префикс ключа
    WORD m_NumKey; // номер ключа
    CSostDev m_Sost; // состояние
    CSostDev m_AskDirect; // результат прямого запроса
    CSostDZ m_SostDZ; // состояние DZ цепочки
    CSostSens m_SostSens; // состояние датчиков
    CMes::typMes m_LastTyp; // последнее сообщение
    CMes::askFromMain m_LastCod; // код запроса последнего сообщения
    bool m_AskSost; // признак необходимости опроса состояния
    bool m_SuppStat; // поддержка статистики работы приводов
    bool m_SuppPnt; // Поддержка кэша контрольных точек на момент аварии
    bool m_SuppIAP; // Поддержка сервиса IAP
    bool m_SuppIONVRAM; // Поддержка чтения/записи NVRAM
    bool m_SuppSrv18; // Поддержка сервиса 18
    bool m_IsFullWho; // поддерка полного формата зароса "Кто ты"
    USHORT m_Version;
    bool m_IsActNVRAM; // признак активности NVRAM
    CParam *m_pParam; // дополнитльеные параметры

    CComPtr<IXMLDOMNode> m_XMLNode;
    bool m_FlagModify;

    CDev(int aAdrBus, CSlot* apSlot, typDev aTypDev, CParam* aParam);
    CDev() : m_pParam(NULL) { Clear();};
    CDev( const CDev &aDev);
    virtual ~CDev();

    void Clear();
    void ClearForLost();
    void AddMes(int aPrior, CMes::typMes aTypMes, CMes::askFromMain aAsk, const BYTE* aMask = NULL, int aLen = 0, int aDuration = -1);
    void AddMes(int aPrior, CMes::typService aService, int aDuration, CMes::askFromMain aAsk, const BYTE* aData = NULL, int aLen = 0);
    void AddMes(int aPrior, CMes::typPrefix aTypPrefix);
    void AddMes(int aPrior, CMes::typPrefix aTypPrefix, CMes::askFromMain aAsk, int aLen = 0, const BYTE* aBody = NULL);
    void AddMes(int aPrior, CMes::subServ18 aSubServ18, const BYTE* aBody = 0, int aLen = 0);
    void Put();
    bool IsKnovExt();
    bool IsExtended();
    bool IsV6(); // блок версии 6 и выше
    bool SetParDev(const CMes& aMes); // извлечь параметры ЛБ из сообщения "Кто ты"
    bool GetFloor(const CMes& Mes, int& aCurFloor, int& aMaxFloor, int& aErrFloor1, int& aErrFloor2);
    static bool IsV6(int aTyp); // блок версии 6 и выше
    bool IsPro(); // блок версии 6Pro или 7
    static bool IsPro(int aTyp); // блок версии 6Pro или 7
    bool IsIT(); // инженерный терминал
    static CAtlString DesignLB(int aDesign);
    static CAtlString TypCodec(int aCodec);
    CAtlString TypLB();
    CAtlString S(UINT aID);

    void XMLRemove();
    void XMLUpdate();
    void XMLAdd();
    void XMLLoad(CComPtr<IXMLDOMNode> spNode);
    void XMLPostLoad();

    static const  TCHAR* s_Dev;
    static const  TCHAR* s_Addr;
};

/*
 * устройства, подключенные к блоку
*/
class CDevTab: public list<CDev*>
{
public:
    CDevTab(CSlot *aSlot): m_pSlot(aSlot){};
    ~CDevTab()
    {
        for(list<CDev*>::iterator i = begin(); i != end(); ++i)
        {
            delete *i;
        }
        clear();
    }
    void GetSost(); // читать состояние КЛШ
    CDev *FindDev(int aAdrBus); // поиск устройства на шине
    bool Insert(CDev* aDev);
    void ReOrder(); // сортировка по адресу на шине

    CSlot *m_pSlot;
    CComPtr<IXMLDOMNode> m_XMLNode;
    bool m_FlagModify;

    void XMLRemove();
    void XMLUpdate();
    void XMLAdd();
    void XMLLoad(CComPtr<IXMLDOMNode> spNode);

    static const TCHAR *s_TabDev;
};
