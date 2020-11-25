#include "StdAfx.h"
#include <IdentV7.h>

/////////////////////////////////////////////////////////////////////////////
// идентификатор устройств v7

CIdentV7::CIdentV7(void)
{
	Clear();
}

CIdentV7::CIdentV7(int aNode, int aDev)
{
	m_Node	= aNode;
	m_Dev	= aDev;
}

CIdentV7::~CIdentV7(void)
{
}

void	CIdentV7::Clear()
{
	m_Node	= 0;
	m_Dev	= 0;
}

bool	CIdentV7::SetUnique()
{
	union
	{
		GUID			guid;
		unsigned short	Short[8];
	} G;

	Clear();

	HRESULT	hr	= ::CoCreateGuid(&(G.guid));

	if(hr == S_OK)
	{
		for(int i = 0; i < 8; i++)
		{
			m_Node	+= G.Short[i];
		}
		m_Node	+= minNodeProg;
		return true;
	}
	return false;
}

bool	CIdentV7::SetNode(int aNode)
{
	if(aNode >= 0 && aNode <= maxNode)
	{
		m_Node	= aNode;
		return true;
	}
	else
	{
		return false;
	}
}

bool	CIdentV7::SetDev(int aDev)
{
	if(aDev >= minDev && aDev <= maxDev)
	{
		m_Dev	= aDev;
		return true;
	}
	else
	{
		return false;
	}
}

bool	CIdentV7::Set(int aNode, int aDev)
{
	if(SetNode(aNode)) return SetDev(aDev);

	return false;
}

int		CIdentV7::GetNode() const
{
	return m_Node;
}

int		CIdentV7::GetDev() const
{
	return m_Dev;
}

void	CIdentV7::GetValue(BYTE* aBuf) const
{
	if(aBuf)
	{
		aBuf[0] = m_Dev;
		memcpy(&aBuf[1], &m_Node, 3);
	}
}

void	CIdentV7::SetValue(const BYTE* aBuf)
{
	Clear();

	if(aBuf)
	{
		m_Dev	= aBuf[0];
		memcpy(&m_Node, &aBuf[1], 3);
	}
}

CAtlString	CIdentV7::GetString() const
{
	CString	buf;

	buf.Format(_T("%d.%d"), m_Node, m_Dev);

	return buf;
}

bool CIdentV7::SetString(const TCHAR* aStr)
{
	bool	rc	= true;

	Clear();

	int	Node=0,Dev=0;

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	_stscanf(aStr, _T("%d.%d"), &Node, &Dev);
#else
	_stscanf_s(aStr, _T("%d.%d"), &Node, &Dev);
#endif

	if(Node >= minNode && Node <= maxNode)
	{
		m_Node	= Node;
	}
	else
	{
		rc	= false;
	}
	if(Dev >= minDev && Dev <= maxDev)
	{
		m_Dev	= Dev;
	}
	else
	{
		rc	= false;
	}

	return rc;
}

int	CIdentV7::operator == (const CIdentV7& id) const
{
	//return(	m_Node == id.m_Node && 24.08.2016
	//		m_Dev  == id.m_Dev );
	return	m_Node == id.m_Node;
}

int	CIdentV7::operator != (const CIdentV7& id) const
{
	//return(	m_Node != id.m_Node || 24.08.2016
	//		m_Dev  != id.m_Dev );
	return	m_Node != id.m_Node;
}

bool	CIdentV7::operator < (const CIdentV7& id) const
{
	return	m_Node < id.m_Node;
}

bool	CIdentV7::FullEQ(const CIdentV7& id) const
{
	return(	m_Node == id.m_Node && 
			m_Dev  == id.m_Dev );
}
