#include "DBConnectProxy.h"
#include "DBConnRedis.h"
#include "DbConnSQLAPI.h"

CDBConnectProxy::CDBConnectProxy()
{
}


CDBConnectProxy::~CDBConnectProxy()
{
	Close();
	m_pConn.reset();
}

bool CDBConnectProxy::Init(unsigned int nDBType, std::string szIP, unsigned int nDBPort, std::string szUser, std::string szPsw, std::string  szDBName, unsigned int nCharSet, unsigned int nTimeOut /*= 120 */)
{
	switch (nDBType)
	{
#ifdef WITH_REDIS
	case DB_REDIS:
		m_pConn = std::make_shared<CDBConnRedis>();
		break;
#endif
        case DB_SQAPI:
            m_pConn = std::make_shared<CDBConnSQAPI>();
            break;
	default:
		break;
	}

	if (m_pConn)
		return m_pConn->Init(nDBType, szIP, nDBPort, szUser, szPsw, szDBName, nCharSet,  nTimeOut);
	return false;
}

bool CDBConnectProxy::Connect()
{
	if (m_pConn)
		return m_pConn->Connect();

	return false;

}

bool CDBConnectProxy::Close()
{
	if (m_pConn)
	{
		return m_pConn->Close();
	}

	return false;
}

bool CDBConnectProxy::IsConnect()
{
	if (m_pConn)
		return m_pConn->IsConnect();

	return false;
}

bool CDBConnectProxy::ReConnect()
{
	if (m_pConn)
		return m_pConn->ReConnect();

	return false;
}

DBConnBasePtr CDBConnectProxy::Get()
{
	return m_pConn;
}

bool CDBConnectProxy::SetCallBackWriteLog(OnWriteLog callBackWriteLog)
{
	if (m_pConn)
		m_pConn->SetCallBackWriteLog(callBackWriteLog);

	return true;
}
