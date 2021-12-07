#ifndef DBConnPooldef_h__
#define DBConnPooldef_h__
#include "DBConnBase.h"

typedef struct __DBCONNPARAM{
	unsigned int nDBID   ;
	unsigned int nDBType ;
	std::string  szDBAddr;
	unsigned int nDBPort ;
	std::string  szDBName;
	std::string  szDBUser;
	std::string  szDBPwd ;
	unsigned int nTimeOut;
	unsigned int nCharSet;
	bool bEnable;
	__DBCONNPARAM() :nDBID(0), nDBType(0), nDBPort(0), nTimeOut(0), nCharSet(0), bEnable(false)
	{
	}
}DBCONNPARAM, *pDBCONNPARAM;



typedef struct __DBCONNINFO{
	unsigned int	nConnID;
	volatile int	nIsUsed;
	int64_t         nTime;
    DBConnBasePtr	pDBConn;

	__DBCONNINFO() :
		nConnID(0),
		nIsUsed(0),
		pDBConn(NULL)
	{
	}

}DBCONNINFO;

typedef struct __DBCONN{
	unsigned int	nConnID;
    DBConnBasePtr	pDBConn;
	__DBCONN()
		: nConnID(0)
		, pDBConn(NULL)
	{
	}
}DBCONN, *PDBCONN;

#endif // DBConnPooldef_h__
