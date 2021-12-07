#ifdef WITH_REDIS
#ifndef DBConnRedis_h__
#define DBConnRedis_h__

#include "DBConnBase.h"
//redis header
#include "RedisCommon.h"


class DBAPI CDBConnRedis
	: public CDBConnBase
{
public:
	CDBConnRedis();
	virtual ~CDBConnRedis();

	bool Connect() override;

	bool Close() override;

	bool IsConnect() override;

	bool ReConnect() override;

public:
	//  operating DB
	bool Select(int mDbName);
	bool Save();
	bool BGSave();
	bool BGRewriteAof();
	bool FlushDB();
	bool FlushAll();

	//
	bool subscribe(const KEYS & keys, ReplyData& vDdata);
	bool unsubscribe(const KEYS & keys, ReplyData& vDdata);
	bool psubscribe(const KEYS & keys, ReplyData& vDdata);
	bool punsubscribe(const KEYS & keys, ReplyData& vDdata);

	bool publish(const KEY & key, const VALUE &value);

	//Commands operating on string values
	bool append(const string& key, const string& value);
	bool bitcount(const string& key, int& count, const int start = 0, const int end = 0);
	bool bitop(const BITOP operation, const string& destkey, const KEYS& keys, int& lenght);
	bool bitpos(const string& key, const int bit, int& pos, const int start = 0, const int end = 0);
	bool decr(const string& key, int& result);
	bool decrby(const string& key, const int by, int& result);
	bool get(const string& key, string& value);
	bool getbit(const string& key, const int& offset, int& bit);
	bool getrange(const string& key, const int start, const int end, string& out);
	bool getset(const string& key, const string& newValue, string& oldValue);
	bool incr(const string& key, int& result);
	bool incrby(const string& key, const int by, int& result);


	bool mget(const KEYS &  keys, ReplyData& vDdata);
	bool mset(const VDATA& data);

	bool psetex(const string& key, const int milliseconds, const string& value);
	bool set(const string& key, const string& value);
	bool setbit(const string& key, const int offset, const int64_t newbitValue, int64_t oldbitValue);
	bool setex(const string& key, const int seconds, const string& value);
	bool setnx(const string& key, const string& value);
	bool setrange(const string& key, const int offset, const string& value, int& length);
	bool strlen(const string& key, int& length);


	bool del(const string& key);
	bool del(const KEYS &  vkey, int64_t& count);

	bool type(const string& key, string& value);
	bool exists(const string& key);
	bool expire(const string& key, const unsigned int second);
	bool expireat(const string& key, const unsigned int timestamp);
	bool keys(const string& Par, KEYS & vkey);



	bool persist(const string& key);
	bool pexpire(const string& key, const unsigned int milliseconds);
	bool pexpireat(const string& key, const unsigned int millisecondstimestamp);
	bool pttl(const string& key, int64_t &milliseconds);
	bool randomkey(KEY& key);
	bool ttl(const string& key, int64_t& seconds);


	//Commands operating on Hash values

	bool hdel(const string& key, const string& filed, int64_t& num);
	bool hdel(const string& key, const KEYS& vfiled, int64_t& num);
	bool hexist(const string& key, const string& filed);
	bool hget(const string& key, const string& filed, string& value);
	bool hgetall(const string& key, ArrayReply& array);
	bool hincrby(const string& key, const string& filed, const int64_t increment, int64_t& value);
	bool hincrbyfloat(const string& key, const string& filed, const float increment, float& value);
	bool hkeys(const string& key, KEYS& keys);
	bool hlen(const string& key, int64_t& count);
	bool hmget(const string& key, const KEYS& filed, ArrayReply& array);
	bool hmset(const string& key, const VDATA& vData);

	bool hset(const string& key, const string& filed, const string& value, int64_t& retval);
	bool hsetnx(const string& key, const string& filed, const string& value);
	bool hvals(const string& key, VALUES& values);



	//Commands operating on list values

	bool lindex(const string& key, const int64_t index, VALUE& value);
	bool linsert(const string& key, const LMODEL mod, const string& pivot, const string& value, int64_t& retval);
	bool llen(const string& key, int64_t& len);
	bool lpop(const string& key, string& value);
	bool lpush(const string& key, const VALUES& vValue, int64_t& length);
	bool lpushx(const string& key, const string& value, int64_t& length);
	bool lrange(const string& key, const int64_t start, const int64_t end, ArrayReply& array);
	bool lrem(const string& key, const int count, const string& value, int64_t num);
	bool lset(const string& key, const int index, const string& value);
	bool ltrim(const string& key, const int start, const int end);
	bool rpop(const string& key, string& value);
	bool rpoplpush(const string& key_src, const string& key_dest, string& value);
	bool rpush(const string& key, const VALUES& vValue, int64_t& length);
	bool rpushx(const string& key, const string& value, int64_t& length);

	//Commands operating on set values

	bool sadd(const KEY& key, const VALUES& vValue, int64_t& count);
	bool scrad(const KEY& key, int64_t& count);
	bool sdiff(const KEYS& vKkey, VALUES& vValue);
	bool sdiffstore(const KEY& destinationkey, const KEYS& vkey, int64_t& count);
	bool sinter(const KEYS& vkey, VALUES& vValue);
	bool sinterstore(const KEY& destinationkey, const KEYS& vkey, int64_t& count);
	bool sismember(const KEY& key, const VALUE& member);
	bool smember(const KEY& key, VALUES& vValue);
	bool smove(const KEY& srckey, const KEY& deskey, const VALUE& member);
	bool spop(const KEY& key, VALUE& member);
	bool srandmember(const KEY& key, VALUES& vmember, int num = 0);
	bool srem(const KEY& key, const VALUES& vmembers, int64_t& count);

	bool sunion(const KEYS& vkey, VALUES& vValue);
	bool sunionstore(const KEY& deskey, const KEYS& vkey, int64_t& count);

	//Commands operating on SortedSet values
	bool zadd(const KEY& deskey, const VALUES& vValues, int64_t& count);
	bool zscrad(const string& key, int64_t& num);

	bool zincrby(const string& key, const double &increment, const string& member, string& value);

	bool zrange(const string& key, int start, int end, VALUES& vValues, bool withscore = false);

	bool zrank(const string& key, const string& member, int64_t &rank);
	bool zrem(const KEY& key, const VALUES& vmembers, int64_t &num);
	bool zremrangebyrank(const string& key, const int start, const int stop, int64_t& num);

	bool zrevrange(const string& key, int start, int end, VALUES& vValues, bool withscore = false);

	bool zrevrangebyscore(const string& key, const std::string& start, const std::string& end, VALUES& vValues, bool withscore = false);

	bool zrevrank(const string& key, const string &member, int64_t& rank);

	bool zscore(const string& key, const string &member, string& score);

private:
	bool CheckReply(const redisReply* reply, const char *szCmp);
	void FreeReply(const redisReply* reply);

public:
	//������Ϣ
	string GetErrInfo() { return m_strErr; }
	bool SetErrInfo(const string& info);
private:
	std::string m_strErr;
private:
	redisContext *_connect;

	void addparam(VDATA& vDes, const VDATA& vSrc) {		
		vDes.insert(vDes.end(), vSrc.begin(), vSrc.end());
	}
public:
	bool command_bool(const char* cmd, ...);
	bool command_status(const char* cmd, ...);
	bool command_integer(int64_t &intval, const char* cmd, ...);
	bool command_string(string &data, const char* cmd, ...);
	bool command_list(VALUES &vValue, const char* cmd, ...);
	bool command_array(ArrayReply& array, const char* cmd, ...);

	bool commandargv_bool(const VDATA& vData);
	bool commandargv_status(const VDATA& vData);
	bool commandargv_array(const VDATA& vDataIn, ArrayReply& array);
	bool commandargv_array(const VDATA& vDataIn, VALUES& array);
	bool commandargv_integer(const VDATA& vDataIn, int64_t& retval);


};


#endif // DBConnRedis_h__
#endif
