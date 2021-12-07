#include "DBConnBase.h"

CDBConnBase::CDBConnBase()
	:m_nDBType(0), m_nPort(0), m_nCharSet(0), m_callBackWriteLog(NULL)
{

}


CDBConnBase::~CDBConnBase()
{
}


bool CDBConnBase::Init(unsigned int nDBType, std::string szIP, unsigned int nDBPort, std::string szUser, std::string szPsw, std::string szDBName, unsigned int nCharSet, unsigned int nTimeOut /*= 120 */)
{
	m_nDBType = nDBType;
	m_strIP = szIP;
	m_nPort = nDBPort;
	m_strUser = szUser;
	m_strPsw = szPsw;
	m_strDBName = szDBName;
	m_nCharSet = nCharSet;
	m_nTimeOut = nTimeOut;
	return true;
}


bool CDBConnBase::SetCallBackWriteLog(OnWriteLog callBackWriteLog)
{
	m_callBackWriteLog = callBackWriteLog;
	return true;
}

DBConnBasePtr CDBConnBase::Get()
{
	return shared_from_this();
}

void CDBConnBase::WriteLog(const char * strFormat, ...)
{
	if (m_callBackWriteLog)
	{
		char strMsg[256];
		va_list ap;
		va_start(ap, strFormat);
		vsnprintf(strMsg, sizeof(strMsg), strFormat, ap);
		va_end(ap);
		m_callBackWriteLog(strMsg);
	}
}
