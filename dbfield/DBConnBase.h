
#ifndef DBConnection_h__
#define DBConnection_h__
#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include "DBPublic.h"
using std::string;
using std::vector;

class CDBConnBase;
typedef std::shared_ptr<CDBConnBase> DBConnBasePtr;



class DBAPI CDBConnBase
	: public std::enable_shared_from_this<CDBConnBase>
{
public:
    CDBConnBase();

	virtual ~CDBConnBase();

	virtual bool Init(unsigned int nDBType
		, std::string  szIP
		, unsigned int nDBPort
		, std::string  szUser
		, std::string  szPsw
		, std::string  szDBName
		, unsigned int nCharSet
		, unsigned int nTimeOut = 120
		);

	virtual bool SetCallBackWriteLog(OnWriteLog callBackWriteLog);
	virtual void WriteLog(const char * strFormat, ...);

	virtual bool Connect() = 0;
	virtual bool Close() = 0;
	virtual bool IsConnect() = 0;
	virtual bool ReConnect() = 0;

	virtual DBConnBasePtr  Get();

	inline std::string GetIp()
	{
		return m_strIP;
	}
protected:
	unsigned int m_nDBType;
	std::string  m_strIP;
	unsigned int m_nPort;
	std::string  m_strUser;
	std::string  m_strPsw;
	std::string  m_strDBName;
	unsigned int m_nTimeOut;
	unsigned int m_nCharSet;

	OnWriteLog	m_callBackWriteLog;
};


#endif // DBConnection_h__
