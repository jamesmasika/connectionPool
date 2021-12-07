#include "DBConnectPool.h"

CDBConnectPool::CDBConnectPool()
	:m_InitConn(0), m_maxConn(0), m_minConn(0), m_nLoopPeriod(0)
	, m_callBackWriteLog(NULL)
	, m_pFactory(std::make_shared<CDBConnectFactory>())
{
	
}


CDBConnectPool::~CDBConnectPool()
{
	ReleaseDBPool();
}

bool CDBConnectPool::InitDBPool(const vector<DBCONNPARAM>& ParamVec, 
	unsigned int nInitConn, unsigned int nMinConn, unsigned int nMaxConn, 
	unsigned int nMaxIdle, unsigned int nLoopPeriod )
{
	std::lock_guard<std::mutex> _lock(m_mutex);
	timer.Expire();
	m_ConnContainerMap.clear();

	m_InitConn = nInitConn;
	m_minConn = nMinConn;
	m_maxConn = nMaxConn;
	m_nLoopPeriod = nLoopPeriod;
	m_MaxIdle = nMaxIdle;
	m_pFactory->InitParam(ParamVec);
	for (unsigned int i = 0; i < ParamVec.size(); i++)
	{
		if (!ParamVec[i].bEnable) {
			continue;
		}

		m_ConnContainerMap[ParamVec[i].nDBID] = std::make_shared<CDBConnContainer>();

		for (unsigned int j = 0; j < m_InitConn; j++)
		{
            DBConnBasePtr pConn = m_pFactory->MakeConnect(ParamVec[i].nDBID);
			if (pConn)
				m_ConnContainerMap[ParamVec[i].nDBID]->AddFreeConn(pConn);
		}
	}

	timer.StartTimer(nLoopPeriod * 60 * 1000, std::bind(&CDBConnectPool::doLoopCheckDB, this));
	return true;
}

bool CDBConnectPool::ReleaseDBPool()
{
	timer.Expire();
	std::lock_guard<std::mutex> _lock(m_mutex);
	m_ConnContainerMap.clear();
	return true;
}

void CDBConnectPool::doLoopCheckDB()
{
	for (auto& it :m_ConnContainerMap)
	{
		auto Expired = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		int64_t diffTime = m_MaxIdle * 60;
		Expired -= diffTime;

		unsigned int pre_count = it.second->GetDBCount();

		it.second->CheckExpire(Expired, m_minConn, diffTime);

		WriteLog("DBID =%u, pre_total:%u, total:%u, free:%u", it.first, pre_count,
			it.second->GetDBCount(), it.second->GetFreeCount());
	}
}


bool CDBConnectPool::GetDBConn(const uint32_t nDBId, DBCONN& dbConn)
{
	bool bRet(false);
	DBConnContainerPtr pConnLst = NULL;
	std::unique_lock<std::mutex> _lock(m_mutex);
	if (GetKeyResult(m_ConnContainerMap, nDBId, false))
		pConnLst = m_ConnContainerMap[nDBId];
	_lock.unlock();

	if (!pConnLst)
	{
		WriteLog("{CDBConnectPool::GetDBConn} DB Id[%u] does not exist!", nDBId);
		return false;
	}
	
	DBCONNINFO* dbinfo = pConnLst->FindFreeDBConn();
	if (dbinfo)
	{
		dbConn.nConnID = dbinfo->nConnID;
		dbConn.pDBConn = dbinfo->pDBConn->Get();

		//WriteLog("FindFreeDBConn, dbId = %u!", nDBId);
		bRet = true;
	}
	else
	{
		std::unique_lock<std::mutex>_lock(pConnLst->GetLock());

		if (pConnLst->GetDBCount() < m_maxConn)
		{
            DBConnBasePtr pConn = m_pFactory->MakeConnect(nDBId);
			if (pConn)
			{
				dbinfo = pConnLst->AddBusyConn_nolock(pConn);
				dbConn.nConnID = dbinfo->nConnID;
				dbConn.pDBConn = dbinfo->pDBConn->Get();
				bRet = true;
			}
			else
			{
				WriteLog("{CDBConnectPool::GetDBConn} make connect failed, dbId = %d!", nDBId);
			}
		}
		else
		{
			WriteLog("{CDBConnectPool::GetDBConn} Connections has reached the maxNum, cannot make connect, dbId = %d!", nDBId);
		}
	}

	return bRet;
}

void CDBConnectPool::retDBConn(const uint32_t nDBId, DBCONN& dbConn)
{
	DBConnContainerPtr pConnLst = NULL;
	std::unique_lock<std::mutex> _lock(m_mutex);
	 if (GetKeyResult(m_ConnContainerMap, nDBId, false))	
		pConnLst = m_ConnContainerMap[nDBId];
	
	_lock.unlock();

	if (!pConnLst)
	{
		WriteLog("{CDBConnectPool::retDBConn} DB Id[%d] does not exist!", nDBId);
		return;
	}
	pConnLst->RetDBConn(dbConn.nConnID);
	dbConn.pDBConn = NULL;

}



bool CDBConnectPool::SetCallBackWriteLog(OnWriteLog callBackWriteLog)
{
	m_callBackWriteLog = callBackWriteLog;
	if (m_pFactory) 
		m_pFactory->SetCallBackWriteLog(callBackWriteLog);

	return true;
}

void CDBConnectPool::WriteLog(const char * strFormat, ...)
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
