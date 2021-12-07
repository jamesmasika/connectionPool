
#ifndef DBConnectProxy_h__
#define DBConnectProxy_h__

#include "DBConnBase.h"
class  CDBConnectProxy :
	public CDBConnBase
{
public:
	CDBConnectProxy();
	virtual ~CDBConnectProxy();

	virtual bool Init(unsigned int nDBType
		, std::string  szIP
		, unsigned int nDBPort
		, std::string  szUser
		, std::string  szPsw
		, std::string  szDBName
		, unsigned int nCharSet
		, unsigned int nTimeOut = 120
		);

    bool Connect() override;

    bool Close() override;

    bool IsConnect() override;

    bool ReConnect() override;

    bool SetCallBackWriteLog(OnWriteLog callBackWriteLog) override;

    DBConnBasePtr Get() override;

protected:
    DBConnBasePtr m_pConn;
};


#endif // DBConnectProxy_h__

