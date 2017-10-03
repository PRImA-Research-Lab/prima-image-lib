#include "BaseException.h"

using namespace PRImA;

CBaseException::CBaseException(CUniString msg)
{
	m_Msg = msg;
}

CBaseException::~CBaseException(void)
{
}
