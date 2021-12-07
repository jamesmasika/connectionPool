#include "DBWriter.h"
#include <iostream>
using std::cout;
using std::endl;

DBWriter::DBWriter()
{
}

DBWriter::~DBWriter()
{
	CDBConnectionWrapper dbWrapper(pDBPool.get());
	for( auto nInnerId : m_usedRedisSet) {
		dbWrapper.releaseDbConn(nInnerId);
	}
}

bool DBWriter::InitDBPool(OnWriteLog callBackWriteLog, const vector<DBCONNPARAM>& paramVec, unsigned int nInitNum,
	unsigned int nMinNum, unsigned int nMaxNum, unsigned int nMaxIdle, unsigned int nLoopPeriod)
{
	m_nInitSuccess = redis_init_status::startInit;

	pDBPool = std::make_shared<CDBConnectPool>();
	pDBPool->SetCallBackWriteLog(callBackWriteLog);
	WriteLogFunctor = callBackWriteLog;

	if (paramVec.size() <= 0) {
		WriteLogFunctor("paramVec.size() <= 0");
		m_nInitSuccess = redis_init_status::emptyVec;
		return false;
	}

	if( !pDBPool->InitDBPool(paramVec, nInitNum, nMinNum, 
		nMaxNum, nMaxIdle, nLoopPeriod))
	{
		WriteLogFunctor("Init DbPool failed!");
		m_nInitSuccess = redis_init_status::connFailed;
		return false;
	}

	for(const auto& data : paramVec)
	{
		if(data.nDBType == DB_REDIS) {
			m_redisIDVec.push_back(data.nDBID);
		}
		else if(data.nDBType == DB_MYSQL) {
			m_mysqlIDVec.push_back(data.nDBID);
		}
	}
#ifdef WITH_REDIS
	if(m_RedisObjVec.size() <= 0) {
		for( auto & dbid : m_redisIDVec) {
			std::shared_ptr<CDBConnRedis> pRedis = GetRedisConnection(dbid);
			if(pRedis)
				m_RedisObjVec.emplace_back(pRedis);
		}
	}
#endif
	m_nInitSuccess = redis_init_status::success;
	return true;
}

#ifdef WITH_REDIS
std::shared_ptr<CDBConnRedis> DBWriter::GetRedisConnection(uint32_t dbid)
{
	CDBConnectionWrapper dbWrapper(pDBPool.get());
	DBConnBasePtr dbConPtr = nullptr;

	uint32_t nInnerId = 0;
	if ((dbConPtr = dbWrapper.GetDBConn(dbid, nInnerId)) != nullptr)
	{
		m_usedRedisSet.insert(nInnerId);
		std::shared_ptr<CDBConnRedis> pRedis = std::dynamic_pointer_cast<CDBConnRedis>(dbConPtr);
		return pRedis;
	}
	else
	{
		return std::shared_ptr<CDBConnRedis>();
	}
}
#endif

std::shared_ptr<CDBConnSQAPI> DBWriter::GetSQLAPIConnection(uint32_t dbid)
{
    CDBConnectionWrapper dbWrapper(pDBPool.get());
    DBConnBasePtr dbConPtr = nullptr;

    uint32_t nInnerId = 0;
    if ((dbConPtr = dbWrapper.GetDBConn(dbid, nInnerId)) != nullptr)
    {
        m_usedMysqlSet.insert(nInnerId);
        std::shared_ptr<CDBConnSQAPI> pSql = std::dynamic_pointer_cast<CDBConnSQAPI>(dbConPtr);
        return pSql;
    }
    else
    {
        return std::shared_ptr<CDBConnSQAPI>();
    }
}

#ifdef WITH_REDIS
bool DBWriter::redis_hmset(const string & key, const VDATA & vData)
{
	std::shared_ptr<CDBConnRedis> pRedis = GetRedisConnection(m_redisIDVec.front());
	if(pRedis) {
		if (!pRedis->hmset(key, vData))
		{
			WriteLogFunctor(pRedis->GetErrInfo());
			return false;
		}
		return true;
	}
	return false;
}

bool DBWriter::redis_publish(const KEY & key, const VALUE & value)
{
	
	if(m_RedisObjVec.size() <= 0) {
		WriteLogFunctor("get redis point again");
		for( auto & dbid : m_redisIDVec) {
			std::shared_ptr<CDBConnRedis> pRedis = GetRedisConnection(dbid);
			if(pRedis)
				m_RedisObjVec.emplace_back(pRedis);
		}
		
		if(m_RedisObjVec.size() <= 0) {
			WriteLogFunctor("can't get redis point");
			return false;
		}
	}
	
	for(auto pDBObj : m_RedisObjVec) {
		if (!pDBObj->publish(key, value))
		{
			WriteLogFunctor(pDBObj->GetIp() + pDBObj->GetErrInfo());
		}
	}
	
	return true;
}

void DBWriter::Pop(const std::pair<string, string>& data )
{
	redis_publish(data.first, data.second);
}
#endif

void DBWriter::run()
{
	while(_running) 
	{
		std::pair<string, string>  tmp;
		bool succ = m_Queue.wait_dequeue_timed(tmp, std::chrono::milliseconds(16));
		//if (succ)
			//Pop(tmp);
	}
}

void DBWriter::Push(const string& key, const string& value)
{
	m_Queue.enqueue(std::make_pair(key, value));

}

bool DBWriter::InitSuccess()
{
	return m_nInitSuccess == redis_init_status::success;
}

void DBWriter::testSQLAPI(int dbid) {
    try{
        CDBConnectionWrapper dbWrapper(pDBPool.get());
        //打印获取dbid信息，一般不要打开
        //dbWrapper.SetCallBackWriteLog(WriteLogFunctor);
        DBConnBasePtr dbConPtr = nullptr;

        uint32_t nInnerId = 0;
        std::shared_ptr<CDBConnSQAPI> pSql = nullptr;
        if ((dbConPtr = dbWrapper.GetDBConn(dbid, nInnerId)) != nullptr) {
            pSql = std::dynamic_pointer_cast<CDBConnSQAPI>(dbConPtr);
        }
        if(!pSql) return;
        //
        auto &SAConn = pSql->getAPI();
        CTA::t_alltype acc;
        auto addlist = acc.Query(SAConn);
        if(addlist->size() > 0) {
            cout << "get size=" << addlist->size()<<endl;
        }

        auto remlist = *addlist;
        auto& front = remlist.front();
        front.setdec(1243.6);
        front.setintd(2);
        front.setbitd("0");
        string js = front.toJson();
        printf("%s\n", js.c_str());
        front.replaceOrInsert(SAConn, false);
        remlist.push_back(front);
        CTA::t_alltype::replaceAll(remlist, SAConn);
    }
    catch(SAException& e) {
        cout << "\nwe get " << (const char*)e.ErrText() << endl;
    }

    try {
        //这个类在函数结束会自动返还连接到连接池内部
        CDBConnectionWrapper dbWrapper(pDBPool.get());
        //打印获取dbid信息，一般不要打开
        //dbWrapper.SetCallBackWriteLog(WriteLogFunctor);
        DBConnBasePtr dbConPtr = nullptr;

        uint32_t nInnerId = 0;
        if ((dbConPtr = dbWrapper.GetDBConn(dbid, nInnerId)) != nullptr) {
            std::shared_ptr<CDBConnSQAPI> pSql = std::dynamic_pointer_cast<CDBConnSQAPI>(dbConPtr);
            auto &SAConn = pSql->getAPI();
            SACommand cmd(&SAConn);

            cmd.setCommandText("SELECT * FROM t_biz_systeminfo");
            //cmd.Param(1).setAsLong() = 1;
            cmd.Execute();

            if (cmd.FetchNext()) {
                cmd.Field("systemid").FieldType();
                string systemid = (const char *) cmd.Field("systemid").asString();
                string systemname = (const char *) cmd.Field("systemname").asString();
                string systemip = (const char *) cmd.Field("systemip").asString();

                std::cout << systemid << "|" <<  systemname << "|" << systemip << "threadid=" << std::this_thread::get_id() << endl;
            }

        }
    }
    catch(SAException& e) {
        cout << (const char*)e.ErrText() << endl;
    }
}
