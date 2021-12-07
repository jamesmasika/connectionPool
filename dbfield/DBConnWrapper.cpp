#include "DBConnWrapper.h"
#include "common.h"
#include <random>  
#include <time.h>  

#define BASE_TIMES 10000
CDBConnectionWrapper::CDBConnectionWrapper(CDBConnectPool* pDBPool)
	:m_pDBPool(pDBPool)
{
    m_callBackWriteLog = nullptr;
    m_nInnerId = -1;
}


CDBConnectionWrapper::~CDBConnectionWrapper()
{
	releaseDbConn(m_nInnerId);
}

DBConnBasePtr CDBConnectionWrapper::GetDBConn(unsigned int nDBId, uint32_t& innerId)
{
	DBCONN dbCon;
	if (m_pDBPool->GetDBConn(nDBId, dbCon))
	{
		innerId = nDBId * BASE_TIMES + dbCon.nConnID;
        m_nInnerId = innerId;
		m_dbConMap.insert(std::make_pair(innerId, dbCon));
        if(m_callBackWriteLog) {
            string msg = "get dbid =" + std::to_string(m_nInnerId);
            m_callBackWriteLog(msg.c_str());
        }
		return dbCon.pDBConn;
	}
	return nullptr;
}

DBConnBasePtr CDBConnectionWrapper::GetDBConn_random(const std::list<uint32_t>& nDBIdList, uint32_t& innerId)
{
	DBCONN dbCon;
	std::list<uint32_t> listDB = nDBIdList;
	std::default_random_engine random(time(NULL));
	while (listDB.size() > 0)
	{
		std::uniform_int_distribution<int> dist(1, (int)listDB.size());
		int idx = dist(random)-1;

		std::list<uint32_t>::iterator it = listDB.begin();
		std::advance(it, idx);
		int dbID = *it;
		listDB.erase(it);
		if (m_pDBPool->GetDBConn(dbID, dbCon))
		{
			innerId = dbID * BASE_TIMES + dbCon.nConnID;
            m_nInnerId = innerId;
			m_dbConMap.insert(std::make_pair(innerId, dbCon));
            if(m_callBackWriteLog) {
                string msg = "return dbid =" + std::to_string(m_nInnerId);
                m_callBackWriteLog(msg.c_str());
            }
			return dbCon.pDBConn;
		}
	}
	return nullptr;
}

DBConnBasePtr CDBConnectionWrapper::GetDBConn_custom(const std::list<uint32_t>& nDBIdList, uint32_t& innerId , std::function<int(std::list<unsigned int>&)>& funCustom)
{
	DBCONN dbCon;
	std::list<uint32_t> listDB = nDBIdList;
	while (listDB.size() > 0)
	{
		int idx = funCustom(listDB);
		std::list<uint32_t>::iterator it = listDB.begin();
		std::advance(it, idx);
		int dbID = *it;
		listDB.erase(it);
		if (m_pDBPool->GetDBConn(dbID, dbCon))
		{
			innerId = dbID * BASE_TIMES + dbCon.nConnID;
            m_nInnerId = innerId;
			m_dbConMap.insert(std::make_pair(innerId, dbCon));
			if(m_callBackWriteLog) {
                string msg = "return dbid =" + std::to_string(m_nInnerId);
                m_callBackWriteLog(msg.c_str());
            }
			return dbCon.pDBConn;
		}
	}
	return nullptr;
}

void CDBConnectionWrapper::releaseDbConn(uint32_t nInnerId)
{
	if (m_pDBPool && nInnerId != -1)
	{
		//DBCONN
		DBConMapIter iter;
		if( (iter = m_dbConMap.find(nInnerId)) != m_dbConMap.end() ) {
			uint32_t nDBId = nInnerId / BASE_TIMES;
			m_pDBPool->retDBConn(nDBId, iter->second);
			m_dbConMap.erase(nInnerId);
            if(m_callBackWriteLog) {
                string msg = "return dbid =" + std::to_string(m_nInnerId);
                m_callBackWriteLog(msg.c_str());
            }
		}
	}
}

bool CDBConnectionWrapper::SetCallBackWriteLog(OnWriteLog callBackWriteLog) {
    m_callBackWriteLog = callBackWriteLog;
    return true;
}
