
#ifndef DBConnectionList_h__
#define DBConnectionList_h__
#pragma once
#include <memory>
#include <mutex>
#include "DBConnPooldef.h"


class CDBConnContainer
{
public:
    CDBConnContainer();

	virtual ~CDBConnContainer();

	void AddFreeConn(DBConnBasePtr pConn);
	void RetDBConn(unsigned int nConnId);
	void RemoveDBConn(unsigned int nConnId);

	unsigned int GetDBCount();
	unsigned int GetFreeCount();
	void Clear();

	DBCONNINFO* AddBusyConn(DBConnBasePtr pConn);
	DBCONNINFO* FindFreeDBConn();
	DBCONNINFO* AddBusyConn_nolock(DBConnBasePtr pConn);
	DBCONNINFO* FindFreeDBConn_nolock();

	void CheckExpire(int64_t nExpired, unsigned int minConn, int64_t diffTime);
	std::mutex& GetLock(){ return m_mutex; }
private:
	unsigned int m_nCount;
	std::map<unsigned int, DBCONNINFO > m_mapConn;
	std::mutex m_mutex;
};


#endif // DBConnectionLst_h__
