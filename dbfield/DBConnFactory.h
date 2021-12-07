
#ifndef DBConnectFactory_h__
#define DBConnectFactory_h__

#include "DBConnPooldef.h"
#include "common.h"

class CDBConnectFactory 
{
public:
	CDBConnectFactory();
	~CDBConnectFactory();

	void InitParam(const vector<DBCONNPARAM>& ParamVec);
	DBConnBasePtr  MakeConnect(unsigned int nConnId);
	void  DestoryConnect(DBConnBasePtr & pDBConn);
	bool  validateConnect(DBConnBasePtr & pDBConn);

	bool SetCallBackWriteLog(OnWriteLog callBackWriteLog, void* pCaller = NULL) ;
	void WriteLog(const char * strFormat, ...);

protected:
	std::map<unsigned int, DBCONNPARAM> m_mapDBParam;
	std::set<unsigned int> m_setDBParam;
	OnWriteLog	m_callBackWriteLog;

};


#endif // DBConnectFactory_h__


