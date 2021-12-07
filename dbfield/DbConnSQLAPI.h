#ifndef _DBCONNECTION_H_
#define _DBCONNECTION_H_

#include "SQLAPI.h"
#include "DBConnBase.h"

class CDBConnSQAPI : public CDBConnBase
{
public:
    CDBConnSQAPI();
	~CDBConnSQAPI();

	void CheckConntectionError(int nErrorNativeCode);

	void KeepAlive();

    bool Connect() override;

    bool Close() override;

    bool IsConnect() override;

    bool ReConnect() override;

    void WriteLog(const char * strFormat, ...);

    SAConnection& getAPI() ;

private:
    SAConnection m_SAConn;
    eSAClient m_eSAClient;
    bool	m_bConnected;
    int m_disconnCount;
};

#endif
