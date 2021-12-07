#ifndef REDIS_WRITER_H__
#define REDIS_WRITER_H__

#include "readerwriterqueue.h"
#include "Thread.hpp"
#include "Singleton.hpp"
#include "DBConnRedis.h"
#include "DbConnSQLAPI.h"
#include "DBConnWrapper.h"
#include "t_alltype.hpp"


enum redis_init_status {
	startInit = 0,
	emptyVec,
	connFailed,
	success
};

class DBWriter : public TC::TC_Singleton<DBWriter>, public TC::TC_Thread {
public:
	DBWriter();
	virtual ~DBWriter();

public:
	//初始化数据库池
	bool InitDBPool(OnWriteLog callBackWriteLog, const vector<DBCONNPARAM>& paramVec,
		unsigned int nInitConn, unsigned int nMinConn, unsigned int nMaxConn,
		unsigned int nMaxIdle, unsigned int nLoopPeriod);

private:
#ifdef WITH_REDIS
	std::shared_ptr<CDBConnRedis> GetRedisConnection(uint32_t dbid);

	bool redis_hmset(const string& key, const VDATA& vData);

	bool redis_publish(const KEY & key, const VALUE &value);

	void Pop( const std::pair<string, string>& queue );
#endif

    std::shared_ptr<CDBConnSQAPI> GetSQLAPIConnection(uint32_t dbid);

public:
	void run() override;

	void Push(const string& key, const string& value);

	bool InitSuccess();

	void testSQLAPI(int dbid);

private:
	int m_nInitSuccess;

	OnWriteLog WriteLogFunctor;
	std::shared_ptr<CDBConnectPool> pDBPool;
	std::vector<uint32_t> m_redisIDVec;
	std::set<uint32_t> m_usedRedisSet;
	//
	std::vector<uint32_t> m_mysqlIDVec;
	std::set<uint32_t> m_usedMysqlSet;
#ifdef WITH_REDIS
	std::vector< std::shared_ptr<CDBConnRedis> > m_RedisObjVec;
#endif

	moodycamel::BlockingReaderWriterQueue<std::pair<string, string>> m_Queue;
};

#endif
