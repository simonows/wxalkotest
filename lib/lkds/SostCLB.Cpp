#include "stdafx.h"

#include <SostCLB.h>

/////////////////////////////////////////////////////////////////////////////
// класс состояния КЛШ
//

int	CSostCLB::operator == (CSostCLB& sb) 
{
	return(m_bSost[0] == ((const BYTE*)sb)[0] && m_bSost[2] == ((const BYTE*)sb)[2]);
}

int	CSostCLB::operator != (CSostCLB& sb) 
{
	return( !(*this == sb));
}

