#pragma once

/////////////////////////////////////////////////////////////////////////////
// класс дополнительных параметров 
//

class CParam
{
public:
	CParam()	{};
	virtual		~CParam() {};

	virtual	void	XMLUpdate()	{};
	virtual	void	XMLLoad() {};
};
