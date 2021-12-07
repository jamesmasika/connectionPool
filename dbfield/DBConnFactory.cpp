#include "DBConnFactory.h"
#include "DBConnectProxy.h"

CDBConnectFactory::CDBConnectFactory()
	:m_callBackWriteLog(NULL)
{
}


CDBConnectFactory::~CDBConnectFactory()
{
}

void CDBConnectFactory::InitParam(const vector<DBCONNPARAM>& ParamVec)
{
	for (size_t i = 0; i < ParamVec.size(); i++)
	{
		if (m_setDBParam.find(ParamVec[i].nDBID) == m_setDBParam.end()) {
			m_setDBParam.insert(ParamVec[i].nDBID);
			m_mapDBParam[ParamVec[i].nDBID] = ParamVec[i];
		}
	}
}

DBConnBasePtr CDBConnectFactory::MakeConnect(unsigned int nConnId)
{
    DBConnBasePtr pConnect = nullptr;
	pDBCONNPARAM pParam = GetKeyResult(m_mapDBParam, nConnId, false);
	if (pParam)
	{
		pConnect = std::dynamic_pointer_cast<CDBConnBase>(std::make_shared<CDBConnectProxy>());
		if (pConnect)
		{
			pConnect->Init(pParam->nDBType, pParam->szDBAddr, pParam->nDBPort
				, pParam->szDBUser, pParam->szDBPwd, pParam->szDBName
				, pParam->nCharSet, pParam->nTimeOut);

			pConnect->SetCallBackWriteLog(m_callBackWriteLog);

			if (!pConnect->Connect())
			{
				WriteLog("Make DB Connect failed, dbId=%d", pParam->nDBID);
				DestoryConnect(pConnect);
			}
		}
	}

	return pConnect;
}

void CDBConnectFactory::DestoryConnect(DBConnBasePtr & pDBConn)
{
	if (pDBConn != nullptr)
	{
		pDBConn->Close();
		pDBConn = NULL;
	}
}

bool CDBConnectFactory::validateConnect(DBConnBasePtr & pDBConn)
{
	if (pDBConn == nullptr)
		return false;

	if (0 != pDBConn->IsConnect())
	{
		if (!pDBConn->Connect())
		{
			WriteLog("Vailed DB Connect failed");

			DestoryConnect(pDBConn);
			return false;
		}
	}

	return true;
}



bool CDBConnectFactory::SetCallBackWriteLog(OnWriteLog callBackWriteLog, void* pCaller /*= NULL*/)
{
	m_callBackWriteLog = callBackWriteLog;
	return true;
}

void CDBConnectFactory::WriteLog(const char * strFormat, ...)
{
	if (m_callBackWriteLog)
	{
		char strMsg[512];
		va_list ap;
		va_start(ap, strFormat);
		vsnprintf(strMsg, 512, strFormat, ap);
		va_end(ap);
		m_callBackWriteLog(strMsg);
	}

}
