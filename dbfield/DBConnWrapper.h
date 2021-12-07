
#ifndef DBConnectionWrapper_h__
#define DBConnectionWrapper_h__
#pragma once
#include <memory>
#include <mutex>
#include <list>
#include "DBConnectPool.h"

class DBAPI CDBConnectionWrapper
{
public:
	CDBConnectionWrapper(CDBConnectPool* pDBPool=NULL);
	~CDBConnectionWrapper();

public:
	void releaseDbConn(uint32_t innerId = -1);
    DBConnBasePtr GetDBConn(uint32_t nDBId, uint32_t& innerId);

    DBConnBasePtr GetDBConn_random(const std::list<uint32_t>& nDBIdList, uint32_t& innerId);
    DBConnBasePtr GetDBConn_custom(const std::list<uint32_t>& nDBIdList, uint32_t& innerId
		, std::function<int(std::list<unsigned int>&)>& funCustom);

    bool SetCallBackWriteLog(OnWriteLog callBackWriteLog);

private:
	CDBConnectPool* m_pDBPool;
	std::map<uint32_t, DBCONN> m_dbConMap;
	typedef std::map<uint32_t, DBCONN>::iterator DBConMapIter;
	//auto return the conn to pool if m_nInnerId>-1
	int m_nInnerId;
    OnWriteLog	m_callBackWriteLog;
};


#endif // DBConnectionWrapper_h__
