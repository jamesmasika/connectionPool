#include "DBConnContainer.h"
#include "common.h"

CDBConnContainer::CDBConnContainer()
	:m_nCount(0)
{

}


CDBConnContainer::~CDBConnContainer()
{
	m_nCount = 0;
	m_mapConn.clear();
}

void CDBConnContainer::AddFreeConn(DBConnBasePtr pConn)
{
	std::lock_guard<std::mutex> _lock(m_mutex);
	DBCONNINFO* pCnnInfo = GetKeyResult(m_mapConn, m_nCount, true);
	if (pCnnInfo)
	{
		pCnnInfo->nConnID = m_nCount;
		pCnnInfo->nIsUsed = 0;
		pCnnInfo->pDBConn = pConn;
		pCnnInfo->nTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());;
		m_nCount++;
	}
}

DBCONNINFO* CDBConnContainer::AddBusyConn(DBConnBasePtr pConn)
{
	std::lock_guard<std::mutex> _lock(m_mutex);
	return AddBusyConn_nolock(pConn);
}


DBCONNINFO* CDBConnContainer::AddBusyConn_nolock(DBConnBasePtr pConn)
{
	DBCONNINFO* pCnnInfo = GetKeyResult(m_mapConn, m_nCount, true);
	if (pCnnInfo)
	{
		pCnnInfo->nConnID = m_nCount;
		pCnnInfo->nIsUsed = 1;
		pCnnInfo->pDBConn = pConn;
		pCnnInfo->nTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());;

		m_nCount++;
	}
	return pCnnInfo;
}

DBCONNINFO* CDBConnContainer::FindFreeDBConn()
{
	std::lock_guard<std::mutex> _lock(m_mutex);
	return FindFreeDBConn_nolock();
}

DBCONNINFO* CDBConnContainer::FindFreeDBConn_nolock()
{
	DBCONNINFO *pDbInfo = NULL;
	for (auto& it : m_mapConn)
	{
		if (it.second.nIsUsed == 0 &&
			(it.second.pDBConn->IsConnect() || it.second.pDBConn->ReConnect()))
		{
			it.second.nIsUsed = 1;
			it.second.nTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			pDbInfo = &it.second;
			break;
		}
	}
	return pDbInfo;
}

void CDBConnContainer::RetDBConn(unsigned int nConnId)
{
	std::lock_guard<std::mutex> _lock(m_mutex);
	DBCONNINFO* pCnnInfo = GetKeyResult(m_mapConn, nConnId, false);
	if (pCnnInfo)
	{
		pCnnInfo->nTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		pCnnInfo->nIsUsed = 0;
	}
}

void CDBConnContainer::RemoveDBConn(unsigned int nConnId)
{
	std::lock_guard<std::mutex> _lock(m_mutex);
	m_mapConn.erase(nConnId);
}

void CDBConnContainer::Clear()
{
	m_nCount = 0;
	m_mapConn.clear();
}

unsigned int CDBConnContainer::GetDBCount()
{
	return m_mapConn.size();
}

void CDBConnContainer::CheckExpire(int64_t nExpired, unsigned int minConn, int64_t diffTime)
{
	size_t minSize = minConn > 1 ? minConn : 1;
	int64_t readTime = nExpired + diffTime;
	bool bUpdateTime = false;

	std::lock_guard<std::mutex> _lock(m_mutex);
	auto it = m_mapConn.begin();
	while (it != m_mapConn.end())
	{
		if (m_mapConn.size() <= minSize) {
			bUpdateTime = true;
			break;
		}

		if (it->second.nTime <= nExpired)
		{
			if(it->second.nIsUsed == 0) {
				it->second.pDBConn->Close();
				m_mapConn.erase(it++);
			}
			else {
				it->second.nTime = readTime;
			}
		}
		else
			++it;
	}

	if (!bUpdateTime)
		return;

	auto iter = m_mapConn.begin();
	while (iter != m_mapConn.end())
	{
		iter->second.nTime = readTime;
		iter++;
	}
}

unsigned int CDBConnContainer::GetFreeCount()
{
	int nCount = 0;
	std::lock_guard<std::mutex> _lock(m_mutex);
	for (auto it : m_mapConn)
	{
		if (it.second.nIsUsed == 0)
		{
			nCount++;
		}
	}
	return nCount;
}

