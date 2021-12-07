#ifdef WITH_REDIS
#include "DBConnRedis.h"
#include "common.h"
#include <algorithm>
#include <cstring>

CDBConnRedis::CDBConnRedis()
	:_connect(NULL)
{

}


CDBConnRedis::~CDBConnRedis()
{
	Close();
}


bool CDBConnRedis::CheckReply(const redisReply *reply, const char *szCmp){
	if (NULL == reply) {
		return false;
	}

	switch (reply->type){
	case REDIS_REPLY_STRING:{
		return true;
	}
	case REDIS_REPLY_ARRAY:{
		if (reply->str)
			return (strcmp(reply->str, szCmp) == 0) ? true : false;
	}
	case REDIS_REPLY_INTEGER:{
		return true;
	}
	case REDIS_REPLY_NIL:{
		return false;
	}
	case REDIS_REPLY_STATUS:{
		if (reply->str)
			return (strcmp(reply->str, szCmp) == 0) ? true : false;
	}
	case REDIS_REPLY_ERROR:{
		return false;
	}
	default:{
		return false;
	}
	}

	return false;
}

void CDBConnRedis::FreeReply(const redisReply *reply){
	if (NULL != reply) {
		freeReplyObject((void*)reply);
	}
}


bool CDBConnRedis::command_bool(const char *cmd, ...) {
	bool bRet = false;
	va_list args;
	va_start(args, cmd);
	redisReply *reply = static_cast<redisReply *>(redisvCommand(this->_connect, cmd, args));
	va_end(args);

	if (CheckReply(reply,"OK")) {
		bRet = (reply->integer == 1) ? true : false;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::command_status(const char* cmd, ...) {
	bool bRet = false;

	va_list args;
	va_start(args, cmd);
	redisReply *reply = static_cast<redisReply *>(redisvCommand(this->_connect, cmd, args));
	va_end(args);

	if (CheckReply(reply,"OK")) {
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::command_integer(int64_t &retval, const char* cmd, ...) {
	bool bRet = false;

	va_list args;
	va_start(args, cmd);
	redisReply *reply = static_cast<redisReply *>(redisvCommand(this->_connect, cmd, args));
	va_end(args);
	if (CheckReply(reply,"OK")) {
		retval = reply->integer;
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::command_string(string &data, const char* cmd, ...) {
	bool bRet = false;

	va_list args;
	va_start(args, cmd);
	redisReply *reply = static_cast<redisReply *>(redisvCommand(this->_connect, cmd, args));
	va_end(args);
	if (CheckReply(reply,"OK")) {
		if (reply->str){
			data = reply->str;
		}
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);

	return bRet;
}

bool CDBConnRedis::command_list(VALUES &vValue, const char* cmd, ...) {
	bool bRet = false;

	va_list args;
	va_start(args, cmd);
	redisReply *reply = static_cast<redisReply *>(redisvCommand(this->_connect, cmd, args));
	va_end(args);
	if (CheckReply(reply,"OK")) {
		for (unsigned int i = 0; i < reply->elements; i++) {
			if (reply->element[i]->str){
				vValue.push_back(reply->element[i]->str);
			}
			else{
				vValue.push_back("");
			}
		}
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::command_array(ArrayReply& array, const char* cmd, ...){
	bool bRet = false;
	
	va_list args;
	va_start(args, cmd);
	redisReply *reply = static_cast<redisReply *>(redisvCommand(this->_connect, cmd, args));
	va_end(args);
	if (CheckReply(reply,"OK")) {
		for (unsigned int i = 0; i < reply->elements; i++) {
			DataItem item;
			if (reply->element[i]->str){
				item.type = reply->element[i]->type;
				item.str = reply->element[i]->str;
			}
			else{
				item.type = REDIS_REPLY_NIL;
				item.str = "";
			}
			array.push_back(item);
		}
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::commandargv_bool(const VDATA& vData) {
	bool bRet = false;

	vector<const char *> argv(vData.size());
	vector<size_t> argvlen(vData.size());
	unsigned int j = 0;
	for (vector<string>::const_iterator i = vData.begin(); i != vData.end(); ++i, ++j) {
		argv[j] = i->c_str(), argvlen[j] = i->size();
	}

	redisReply *reply = static_cast<redisReply *>(redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0])));
	if (CheckReply(reply,"OK")) {
		bRet = (reply->integer == 1) ? true : false;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::commandargv_status(const VDATA& vData) {
	bool bRet = false;

	vector<const char *> argv(vData.size());
	vector<size_t> argvlen(vData.size());
	unsigned int j = 0;
	for (vector<string>::const_iterator i = vData.begin(); i != vData.end(); ++i, ++j) {
		argv[j] = i->c_str(), argvlen[j] = i->size();
	}

	redisReply *reply = static_cast<redisReply *>(redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0])));
	if (CheckReply(reply,"OK")) {
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}


	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::commandargv_array(const VDATA& vDataIn, ArrayReply& array){
	bool bRet = false;

	vector<const char*> argv(vDataIn.size());
	vector<size_t> argvlen(vDataIn.size());
	unsigned int j = 0;
	for (vector<string>::const_iterator i = vDataIn.begin(); i != vDataIn.end(); ++i, ++j) {
		argv[j] = i->c_str(), argvlen[j] = i->size();
	}

	redisReply *reply = static_cast<redisReply *>(redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0])));
	if (CheckReply(reply,"OK")) {
		for (unsigned int i = 0; i < reply->elements; i++) {
			DataItem item;
			if (reply->element[i]->str){
				item.type = reply->element[i]->type;
				item.str = reply->element[i]->str;
			}
			else{
				item.type = REDIS_REPLY_NIL;
				item.str = "";
			}
			array.push_back(item);
		}
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::commandargv_array(const VDATA& vDataIn, VALUES& array){
	bool bRet = false;

	vector<const char*> argv(vDataIn.size());
	vector<size_t> argvlen(vDataIn.size());
	unsigned int j = 0;
	for (vector<string>::const_iterator i = vDataIn.begin(); i != vDataIn.end(); ++i, ++j) {
		argv[j] = i->c_str(), argvlen[j] = i->size();
	}

	redisReply *reply = static_cast<redisReply *>(redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0])));
	if (CheckReply(reply,"OK")) {
		for (unsigned int i = 0; i < reply->elements; i++) {
			if (reply->element[i]->str){
				array.push_back(reply->element[i]->str);
			}
			else{
				array.push_back("");
			}
		}
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::commandargv_integer(const VDATA& vDataIn, int64_t& retval){
	bool bRet = false;

	vector<const char*> argv(vDataIn.size());
	vector<size_t> argvlen(vDataIn.size());
	unsigned int j = 0;
	for (vector<string>::const_iterator iter = vDataIn.begin(); iter != vDataIn.end(); ++iter, ++j) {
		argv[j] = iter->c_str(), argvlen[j] = iter->size();
	}

	redisReply *reply = static_cast<redisReply *>(redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0])));
	if (CheckReply(reply,"OK")) {
		retval = reply->integer;
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::Connect()
{
	if (NULL != this->_connect){
		redisFree(this->_connect);
		this->_connect = NULL;
	}

	bool bRet = false;
	struct timeval timeoutVal;
	timeoutVal.tv_sec = m_nTimeOut;
	timeoutVal.tv_usec = 0;
	//�������ݿ������
	this->_connect = redisConnectWithTimeout(m_strIP.c_str(), m_nPort, timeoutVal);
	if (NULL == this->_connect || this->_connect->err) {
		if (NULL != this->_connect) {
			SetErrInfo("redis Connect timeout");
			redisFree(this->_connect);
			this->_connect = NULL;
		}
	}
	else {
		if (0 == m_strPsw.length()) {
			bRet = true;
		}
		else {
			//��������
			redisReply *replyPass = static_cast<redisReply *>(redisCommand(this->_connect, "AUTH %s", m_strPsw.c_str()));
			if (CheckReply(replyPass,"OK")) {
				bRet = true;
			}
			else {
				if (replyPass) 
					SetErrInfo(replyPass->str);
			}
			FreeReply(replyPass);
		}
	}
	//�������ݿ��Ƿ����
	if (bRet){
		int nDBName = atoi(m_strDBName.c_str());
		bRet = Select(nDBName);
	}

	if (!bRet)
		WriteLog("Connect Redis[%s:%d] Error:%s", m_strIP.c_str(), m_nPort, GetErrInfo().c_str());

	
	return bRet;


}

bool CDBConnRedis::Close()
{
	bool bRet = false;
	if (this->_connect){
		redisFree(this->_connect);
		this->_connect = NULL;
		bRet = true;
	}
	return bRet;
}

bool CDBConnRedis::IsConnect()
{

	bool bRet(false);
	if (!this->_connect)
		return false;

	redisReply *reply = static_cast<redisReply *>(redisCommand(this->_connect, "PING"));
	if (CheckReply(reply, "PONG")){
		bRet = true;
	}
	else {
		if (reply)
			SetErrInfo(reply->str);
	}

	FreeReply(reply);

	if (!bRet)
		WriteLog( "Pine Error:%s", GetErrInfo().c_str());

	return bRet;
}


bool CDBConnRedis::Select(int mDbName){
	bool bRet(true);
	char mstrCmd[256] = {};
	sprintf(mstrCmd, "SELECT %d", mDbName);
	redisReply *reply = static_cast<redisReply *>(redisCommand(this->_connect, mstrCmd));
	if (CheckReply(reply,"OK")) {
		bRet = true;
	}
	else {
		if (reply) SetErrInfo(reply->str);
	}

	FreeReply(reply);


	return bRet;
}

bool CDBConnRedis::keys(const string& Par, KEYS & vkey){
	char mstrCmd[256] = {};
	sprintf(mstrCmd, "KEYS %s", Par.c_str());
	return command_list(vkey, mstrCmd);
}
bool CDBConnRedis::psetex(const string& key, const int milliseconds, const string& value) {
	return command_bool("PSETEX %s %d %s", key.c_str(), milliseconds, value.c_str());
}

bool CDBConnRedis::set(const string& key, const string& value) {
	VDATA vCmdData;
	vCmdData.push_back("SET");
	vCmdData.push_back(key);
	vCmdData.push_back(value);
	return commandargv_status(vCmdData);
}

bool CDBConnRedis::setbit(const string& key, const int offset, const int64_t newbitValue, int64_t oldbitValue) {
	return command_integer(oldbitValue, "SETBIT %s %d %lld", key.c_str(), offset, newbitValue);
}

bool CDBConnRedis::get(const string& key, string& value) {
	return command_string(value, "GET %s", key.c_str());
}

bool CDBConnRedis::getbit(const string& key, const int& offset, int& bit) {
	return command_integer((int64_t&)bit, "GETBIT %s %d", key.c_str(), offset);
}

bool CDBConnRedis::getrange(const string& key, const int start, const int end, string& out) {
	return command_string(out, "GETRANGE %s %d %d", key.c_str(), start, end);
}

bool CDBConnRedis::getset(const string& key, const string& newValue, string& oldValue) {
	return command_string(oldValue, "GETSET %s %s", key.c_str(), newValue.c_str());
}

bool CDBConnRedis::mget(const KEYS &  keys, ReplyData& vDdata) {
	VDATA vCmdData;
	vCmdData.push_back("MGET");
	addparam(vCmdData, keys);
	return commandargv_array(vCmdData, vDdata);
}

bool CDBConnRedis::mset(const VDATA& vData) {
	VDATA vCmdData;
	vCmdData.push_back("MSET");
	addparam(vCmdData, vData);
	return commandargv_status(vCmdData);
}

bool CDBConnRedis::setex(const string& key, const int seconds, const string& value) {
	VDATA vCmdData;
	char szTemp[16] = { 0 };
	sprintf(szTemp, "%d", seconds);

	vCmdData.push_back("SETEX");
	vCmdData.push_back(key);
	vCmdData.push_back(szTemp);
	vCmdData.push_back(value);
	return commandargv_bool(vCmdData);
}

bool CDBConnRedis::setnx(const string& key, const string& value) {
	VDATA vCmdData;
	vCmdData.push_back("SETNX");
	vCmdData.push_back(key);
	vCmdData.push_back(value);
	return commandargv_bool(vCmdData);
}

bool CDBConnRedis::setrange(const string& key, const int offset, const string& value, int& length) {
	return command_integer((int64_t&)length, "setrange %s %d %s", key.c_str(), offset, value.c_str());
}

bool CDBConnRedis::strlen(const string& key, int& length) {
	return command_integer((int64_t&)length, "STRLEN %s", key.c_str());
}

bool CDBConnRedis::incr(const string& key, int& result) {
	return command_integer((int64_t&)result, "INCR %s", key.c_str());
}

bool CDBConnRedis::incrby(const string& key, const int by, int& result) {
	return command_integer((int64_t&)result, "INCR %s %d", key.c_str(), by);
}

bool CDBConnRedis::bitcount(const string& key, int& count, const int start, const int end) {
	if ((start != 0) || (end != 0)) {
		return command_integer((int64_t&)count, "bitcount %s %d %d", key.c_str(), start, end);
	}
	return command_integer((int64_t&)count, "bitcount %s", key.c_str());
}

bool CDBConnRedis::bitop(const BITOP operation, const string& destkey, const KEYS& keys, int& lenght) {
	static const char *op_cmd[4] = { "AND", "OR", "XOR", "NOT" };
	VDATA vCmdData;
	vCmdData.push_back("bitop");
	vCmdData.push_back(op_cmd[operation]);
	vCmdData.push_back(destkey);
	addparam(vCmdData, keys);
	return commandargv_integer(vCmdData, (int64_t&)lenght);
}

bool CDBConnRedis::bitpos(const string& key, const int bit, int& pos, const int start, const int end) {
	if ((start != 0) || (end != 0)) {
		return command_integer((int64_t&)pos, "BITPOS %s %d %d %d", key.c_str(), bit, start, end);
	}
	return command_integer((int64_t&)pos, "BITPOS %s %d", key.c_str(), bit);
}

bool CDBConnRedis::decr(const string& key, int& result) {
	return command_integer((int64_t&)result, "decr %s", key.c_str());
}

bool CDBConnRedis::decrby(const string& key, const int by, int& result) {
	return command_integer((int64_t&)result, "decrby %s %d", key.c_str(), by);
}

bool CDBConnRedis::hdel(const string& key, const string& filed, int64_t& count) {
	return command_integer(count, "HDEL %s %s %s", key.c_str(), filed.c_str());
}

bool CDBConnRedis::hdel(const string& key, const KEYS& vfiled, int64_t& count) {
	VDATA vCmdData;
	vCmdData.push_back("HDEL");
	vCmdData.push_back(key);
	addparam(vCmdData, vfiled);
	return commandargv_integer(vCmdData, count);
}

bool CDBConnRedis::hexist(const string& key, const string& filed){
	return command_bool("HEXISTS %s %s", key.c_str(), filed.c_str());
}

bool CDBConnRedis::hget(const string& key, const string& filed, string& value) {
	return command_string(value, "HGET %s %s", key.c_str(), filed.c_str());
}

bool  CDBConnRedis::hgetall(const string& key, ArrayReply& array){
	return command_array(array, "HGETALL %s", key.c_str());
}

bool CDBConnRedis::hincrby(const string& key, const string& filed, const int64_t increment, int64_t& num) {
	return command_integer(num, "HINCRBY %s %s %lld", key.c_str(), filed.c_str(), increment);
}

bool CDBConnRedis::hincrbyfloat(const string& key, const string& filed, const float increment, float& value) {
	bool bRet = false;

	redisReply *reply = static_cast<redisReply *>(redisCommand(this->_connect, "HINCRBY %s %s %f", key.c_str(), filed.c_str(), increment));
	if (CheckReply(reply,"OK")) {
		value = atof(reply->str);
		bRet = true;
	}

	FreeReply(reply);
	return bRet;
}

bool CDBConnRedis::hkeys(const string& key, KEYS& keys){
	return command_list(keys, "HKEYS %s", key.c_str());
}

bool CDBConnRedis::hlen(const string& key, int64_t& count){
	return command_integer(count, "HLEN %s", key.c_str());
}

bool CDBConnRedis::hmget(const string& key, const KEYS& filed, ArrayReply& array){
	VDATA vCmdData;
	vCmdData.push_back("HMGET");
	vCmdData.push_back(key);
	addparam(vCmdData, filed);
	return commandargv_array(vCmdData, array);
}

bool CDBConnRedis::hmset(const string& key, const VDATA& vData){
	VDATA vCmdData;
	vCmdData.push_back("HMSET");
	vCmdData.push_back(key);
	addparam(vCmdData, vData);
	return commandargv_status(vCmdData);
}

bool CDBConnRedis::hset(const string& key, const string& filed, const string& value, int64_t& retval){
	return command_integer(retval, "HSET %s %s %s", key.c_str(), filed.c_str(), value.c_str());
}

bool CDBConnRedis::hsetnx(const string& key, const string& filed, const string& value){
	return command_bool("HSETNX %s %s %s", key.c_str(), filed.c_str(), value.c_str());
}

bool CDBConnRedis::hvals(const string& key, VALUES& values) {
	return command_list(values, "HVALS %s", key.c_str());
}

bool  CDBConnRedis::del(const string& key) {
	return command_bool("DEL %s", key.c_str());
}

bool  CDBConnRedis::del(const KEYS &  vkey, int64_t& count) {
	count = 0;
	KEYS::const_iterator iter_key = vkey.begin();
	for (; iter_key != vkey.end(); ++iter_key) {
		const string &key = (*iter_key);
		if (del(key)) {
			count++;
		}
	}
	return true;
}

bool CDBConnRedis::exists(const string& key) {
	return command_bool("EXISTS %s", key.c_str());
}

bool CDBConnRedis::type(const string& key, string& value)
{
	return command_string(value, "TYPE %s", key.c_str());
}

bool CDBConnRedis::expire(const string& key, const unsigned int second) {
	return command_bool("EXPIRE %s %u", key.c_str(), second);
}

bool CDBConnRedis::expireat(const string& key, const unsigned int timestamp) {
	return command_bool("EXPIREAT %s %u", key.c_str(), timestamp);
}

bool CDBConnRedis::persist(const string& key) {
	return command_bool("PERSIST %s %u", key.c_str());
}

bool CDBConnRedis::pexpire(const string& key, const unsigned int milliseconds) {
	return command_bool("PEXPIRE %s %u", key.c_str(), milliseconds);
}

bool CDBConnRedis::pexpireat(const string& key, const unsigned int millisecondstimestamp) {
	return command_bool("PEXPIREAT %s %u", key.c_str(), millisecondstimestamp);
}

bool CDBConnRedis::pttl(const string& key, int64_t &milliseconds) {
	return command_integer(milliseconds, "PEXPIREAT %s", key.c_str());
}

bool CDBConnRedis::ttl(const string& key, int64_t &seconds) {
	return command_integer(seconds, "TTL %s", key.c_str());
}

bool CDBConnRedis::randomkey(KEY& key){
	return command_string(key, "RANDOMKEY");
}

bool CDBConnRedis::lindex(const string& key, const int64_t index, VALUE& value){
	return command_string(value, "LINDEX %s %lld", key.c_str(), index);
}

bool CDBConnRedis::linsert(const string& key, const LMODEL mod, const string& pivot, const string& value, int64_t& retval){
	static const char *lmodel[2] = { "BEFORE", "AFTER" };
	return command_integer(retval, "LINSERT %s %s %s %s", key.c_str(), lmodel[mod], pivot.c_str(), value.c_str());
}

bool CDBConnRedis::llen(const string& key, int64_t& retval){
	return command_integer(retval, "LLEN %s", key.c_str());
}

bool CDBConnRedis::lpop(const string& key, string& value){
	return command_string(value, "LPOP %s", key.c_str());
}

bool CDBConnRedis::lpush(const string& key, const VALUES& vValue, int64_t& length){
	VDATA vCmdData;
	vCmdData.push_back("lpush");
	vCmdData.push_back(key);
	addparam(vCmdData, vValue);
	return commandargv_integer(vCmdData, length);
}

bool CDBConnRedis::lrange(const string& key, const int64_t start, const int64_t end, ArrayReply& array){
	return command_array(array, "LRANGE %s %lld %lld", key.c_str(), start, end);
}

bool CDBConnRedis::lrem(const string& key, const int count, const string& value, int64_t num){
	return command_integer(num, "LREM %s %d %s", key.c_str(), count, value.c_str());
}

bool CDBConnRedis::lset(const string& key, const int index, const string& value){
	return command_status("LSET %s %d %s", key.c_str(), index, value.c_str());
}

bool CDBConnRedis::ltrim(const string& key, const int start, const int end){
	return command_status("ltrim %s %d %d", key.c_str(), start, end);
}

bool CDBConnRedis::rpop(const string& key, string& value){
	return command_string(value, "LPOP %s", key.c_str());
}

bool CDBConnRedis::rpoplpush(const string& key_src, const string& key_dest, string& value){
	return command_string(value, "RPOPLPUSH %s %s", key_src.c_str(), key_dest.c_str());
}

bool CDBConnRedis::rpush(const string& key, const VALUES& vValue, int64_t& length){
	VDATA vCmdData;
	vCmdData.push_back("rpush");
	vCmdData.push_back(key);
	addparam(vCmdData, vValue);
	return commandargv_integer(vCmdData, length);
}

bool CDBConnRedis::rpushx(const string& key, const string& value, int64_t& length){
	return command_integer(length, "RPUSHX %s %s", key.c_str(), value.c_str());
}

bool CDBConnRedis::sadd(const string& key, const VALUES& vValue, int64_t& count){
	VDATA vCmdData;
	vCmdData.push_back("SADD");
	vCmdData.push_back(key);
	addparam(vCmdData, vValue);
	return commandargv_integer(vCmdData, count);
}

bool CDBConnRedis::scrad(const string& key, int64_t& count){
	return command_integer(count, "SCRAD %s", key.c_str());
}

bool CDBConnRedis::sdiff(const KEYS& vkey, VALUES& sValue){
	int size = vkey.size();
	VALUES *setData = new VALUES[size];
	VALUES::iterator endpos;

	KEYS::const_iterator     iter_key = vkey.begin();
	int i = 0;
	for (; iter_key != vkey.end(); ++iter_key, ++i) {
		const string &key = *iter_key;
		if (!smember(key, setData[i])) {
			delete[] setData;
			return false;
		}
	}

	int n = 0;
	while (n++ < size - 1) {
		endpos = set_difference(setData[n].begin(), setData[n].end(), setData[n + 1].begin(), setData[n + 1].end(), sValue.begin());
		sValue.resize(endpos - sValue.begin());
	}
	delete[] setData;
	return true;
}

bool CDBConnRedis::sdiffstore(const KEY& destinationkey, const KEYS& vkey, int64_t& count){
	VALUES sValue;
	if (!sdiff(vkey, sValue)) {
		return false;
	}
	return sadd(destinationkey, sValue, count);
}

bool CDBConnRedis::sinter(const KEYS& vkey, VALUES& sValue){
	int size = vkey.size();
	VALUES *setData = new VALUES[size];
	VALUES::iterator endpos;

	KEYS::const_iterator     iter_key = vkey.begin();
	int i = 0;
	for (; iter_key != vkey.end(); ++iter_key, ++i) {
		const string &key = *iter_key;
		if (!smember(key, setData[i])) {
			delete[] setData;
			return false;
		}
	}

	int n = 0;
	while (n++ < size - 1){
		endpos = set_intersection(setData[n].begin(), setData[n].end(), setData[n + 1].begin(), setData[n + 1].end(), sValue.begin());
		sValue.resize(endpos - sValue.begin());
	}
	delete[] setData;

	return true;
}

bool CDBConnRedis::sinterstore(const KEY& destinationkey, const KEYS& vkey, int64_t& count){
	VALUES sValue;
	if (!sinter(vkey, sValue)) {
		return false;
	}
	return sadd(destinationkey, sValue, count);
}

bool CDBConnRedis::sismember(const KEY& key, const VALUE& member){
	return command_bool("SISMEMBER %s %s", key.c_str(), member.c_str());
}

bool CDBConnRedis::smember(const KEY& key, VALUES& vValue){
	return command_list(vValue, "SMEMBER %s", key.c_str());
}

bool CDBConnRedis::smove(const KEY& srckey, const KEY& deskey, const VALUE& member){
	return command_bool("SMOVE %s", srckey.c_str(), deskey.c_str(), member.c_str());
}

bool CDBConnRedis::spop(const KEY& key, VALUE& member){
	return command_string(member, "SPOP %s", key.c_str());
}

bool CDBConnRedis::srandmember(const KEY& key, VALUES& members, int count){
	if (0 == count) {
		return command_list(members, "SRANDMEMBER %s", key.c_str());
	}
	return command_list(members, "SRANDMEMBER %s %d", key.c_str(), count);
}

bool CDBConnRedis::srem(const KEY& key, const VALUES& vmembers, int64_t& count){
	VDATA vCmdData;
	vCmdData.push_back("SREM");
	vCmdData.push_back(key);
	addparam(vCmdData, vmembers);
	return commandargv_integer(vCmdData, count);
}

bool CDBConnRedis::sunion(const KEYS& vkey, VALUES& sValue){
	int size = vkey.size();
	VALUES *setData = new VALUES[size];
	VALUES::iterator endpos;

	KEYS::const_iterator     iter_key = vkey.begin();
	int i = 0;
	for (; iter_key != vkey.end(); ++iter_key, ++i) {
		const string &key = *iter_key;
		if (!smember(key, setData[i])) {
			delete[] setData;
			return false;
		}
	}

	int n = 0;
	while (n++ < size - 1) {
		endpos = set_union(setData[n].begin(), setData[n].end(), setData[n + 1].begin(), setData[n + 1].end(), sValue.begin());
		sValue.resize(endpos - sValue.begin());
	}
	delete[] setData;
	return true;
}

bool CDBConnRedis::sunionstore(const KEY& deskey, const KEYS& vkey, int64_t& count){
	VALUES sValue;
	if (!sunion(vkey, sValue)) {
		return false;
	}
	return sadd(deskey, sValue, count);
}

bool CDBConnRedis::zadd(const KEY& key, const VALUES& vValues, int64_t& count){
	VDATA vCmdData;
	vCmdData.push_back("ZADD");
	vCmdData.push_back(key);
	addparam(vCmdData, vValues);
	return commandargv_integer(vCmdData, count);
}

bool CDBConnRedis::zscrad(const string& key, int64_t& count){
	return command_integer(count, "ZSCRAD %s", key.c_str());
}

bool CDBConnRedis::zincrby(const string& key, const double &increment, const string& member, string& value) {
	return command_string(value, "ZINCRBY %s %f %s", key.c_str(), increment, member.c_str());
}

bool CDBConnRedis::zrange(const string& key, int start, int end, VALUES& vValues, bool withscore) {
	if (withscore) {
		return command_list(vValues, "ZRANGE %s %d %d %s", key.c_str(), start, end, "WITHSCORES");
	}
	return command_list(vValues, "ZRANGE %s %d %d", key.c_str(), start, end);
}

bool CDBConnRedis::zrank(const string& key, const string& member, int64_t &rank) {
	return command_integer(rank, "ZRANK %s %s", key.c_str(), member.c_str());
}

bool CDBConnRedis::zrem(const KEY& key, const VALUES& vmembers, int64_t &count) {
	VDATA vCmdData;
	vCmdData.push_back("ZREM");
	vCmdData.push_back(key);
	addparam(vCmdData, vmembers);
	return commandargv_integer(vCmdData, count);
}

bool CDBConnRedis::zremrangebyrank(const string& key, const int start, const int stop, int64_t& count) {
	return command_integer(count, "ZREMRANGEBYRANK %s %d %d", key.c_str(), start, stop);
}

bool CDBConnRedis::zrevrange(const string& key, int start, int end, VALUES& vValues, bool withscore) {
	if (withscore) {
		return command_list(vValues, "ZREVRANGE %s %d %d %s", key.c_str(), start, end, "WITHSCORES");
	}
	return command_list(vValues, "ZREVRANGE %s %d %d", key.c_str(), start, end);
}


bool CDBConnRedis::zrevrangebyscore(const string& key, const std::string& start, const std::string& end, VALUES& vValues, bool withscore /*= false*/)
{
	if (withscore) {
		return command_list(vValues, "ZREVRANGEBYSCORE %s %s %s %s", key.c_str(), start.c_str(), end.c_str(), "WITHSCORES");
	}
	return command_list(vValues, "ZREVRANGEBYSCORE %s %s %s", key.c_str(), start.c_str(), end.c_str());
}


bool CDBConnRedis::zrevrank(const string& key, const string &member, int64_t& rank){
	return command_integer(rank, "ZREVRANK %s %s", key.c_str(), member.c_str());
}

bool CDBConnRedis::zscore(const string& key, const string &member, string& score){
	return command_string(score, "ZSCORE %s %s", key.c_str(), member.c_str());
}

bool CDBConnRedis::subscribe(const KEYS & keys, ReplyData& vDdata)
{
	VDATA vCmdData;
	vCmdData.push_back("SUBSCRIBE");
	addparam(vCmdData, keys);
	return commandargv_array(vCmdData, vDdata);
}

bool CDBConnRedis::unsubscribe(const KEYS & keys, ReplyData& vDdata)
{
	VDATA vCmdData;
	vCmdData.push_back("UNSUBSCRIBE");
	addparam(vCmdData, keys);
	return commandargv_array(vCmdData, vDdata);
}

bool CDBConnRedis::publish(const KEY & key, const VALUE &value)
{
	VDATA vCmdData;
	vCmdData.push_back("PUBLISH");
	vCmdData.push_back(key);
	vCmdData.push_back(value);
	return commandargv_status(vCmdData);
}

bool CDBConnRedis::psubscribe(const KEYS & keys, ReplyData& vDdata)
{
	VDATA vCmdData;
	vCmdData.push_back("PSUBSCRIBE");
	addparam(vCmdData, keys);
	return commandargv_array(vCmdData, vDdata);
}

bool CDBConnRedis::punsubscribe(const KEYS & keys, ReplyData& vDdata)
{
	VDATA vCmdData;
	vCmdData.push_back("PUNSUBSCRIBE");
	addparam(vCmdData, keys);
	return commandargv_array(vCmdData, vDdata);
}

bool CDBConnRedis::SetErrInfo(const string& info)
{
	m_strErr = info;
	return true;
}

bool CDBConnRedis::ReConnect()
{
	return Connect();
}

#endif