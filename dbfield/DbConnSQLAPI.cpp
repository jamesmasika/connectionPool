#include "DbConnSQLAPI.h"

CDBConnSQAPI::CDBConnSQAPI() {
    m_bConnected = false;
    m_eSAClient = SA_Client_NotSpecified;
    m_disconnCount = 0;
}

CDBConnSQAPI::~CDBConnSQAPI() {

}

bool CDBConnSQAPI::Connect()
{	
	try 
	{
		//this->setOption("CLIENT_MULTI_RESULTS")=true;
		//this->setOption("CLIENT_MULTI_STATEMENTS")=true;

		//建立数据库连接
		//连接格式为 MYSQL://192.168.0.1:3306@quantdo or ORACLE://bizorz 等，先用数据库连接方式作为使用哪个数据库的标示
		string split = "://";
		string fullname = "MYSQL://" + m_strIP + ":" +std::to_string(m_nPort) + "@" + m_strDBName;
		size_t pos = fullname.find(split);
		string dbname = fullname.substr(0, pos);
		string firstParam = fullname.substr(pos+split.size());
		//客户端默认使用MYSQL
		if(m_eSAClient == SA_Client_NotSpecified) {
            //m_eSAClient= SA_MySQL_Client;
            //
            if(dbname=="MYSQL")
            {
                //setOption("CharacterSet")="gbk";
                m_SAConn.setOption("CharacterSet") = "utf8";
                m_eSAClient = SA_MySQL_Client;
            }
            else if(dbname=="ORCLE")
            {
                m_eSAClient = SA_Oracle_Client;
            }
            else if(dbname=="SYBASE")
            {
                m_eSAClient = SA_Sybase_Client;
            }
            else if(dbname=="DB2")
            {
                m_eSAClient = SA_DB2_Client;
            }
            else if(dbname=="SQLSERVER")
            {
                m_eSAClient = SA_SQLServer_Client;
            }
		}

        m_SAConn.Connect(firstParam.c_str(), m_strUser.c_str(), m_strPsw.c_str(), m_eSAClient);
		//setAutoCommit(SA_AutoCommitOff);
        m_disconnCount = 0;
	}	
	catch(SAException &x)
	{
        m_disconnCount++;
        WriteLog("Connect Database Error,errorid:%d, msg:%s", x.ErrNativeCode(), (const char*)x.ErrText());
		try
		{
			CheckConntectionError(x.ErrNativeCode());
			m_SAConn.Rollback();
		}
		catch(SAException &)
		{
		}
		return false;
	}
	return true;
}

void CDBConnSQAPI::CheckConntectionError(int nErrorNativeCode)
{
	const int ORACLE_CONNECTION_ERROR_CODE[]=
	{
		0, 12500, 1034, 3113, 3114, 12571, 1033, 1012, 28, 3135, 2006,2013
	};
	for (int i=0; i<sizeof(ORACLE_CONNECTION_ERROR_CODE)/sizeof(int); i++)
	{
		if (nErrorNativeCode == ORACLE_CONNECTION_ERROR_CODE[i])

		{
            m_SAConn.Disconnect();
            WriteLog("Database Disconnect errorid:%d", nErrorNativeCode);
            if(m_disconnCount <= 3) {
                Connect();
            }
			break;
		}
	}
}

void CDBConnSQAPI::KeepAlive()
{
	SACommand cmd(&m_SAConn, "select count(*) from tsync_tmdbaccomplish"); // 因为这里表里面的数据少，所以使用这张表
	try 
	{
		cmd.Execute();
		while(cmd.FetchNext());
	}	
	catch(SAException &x)
	{
        WriteLog("Database KeepAlive Error,errorid:%d, msg:%s", x.ErrNativeCode(), (const char*)x.ErrText());
		try
		{			
			CheckConntectionError(x.ErrNativeCode());
			if(!Connect())
			{
				CheckConntectionError(x.ErrNativeCode());
                m_SAConn.Rollback();
			}		
			
		}
		catch(SAException &)
		{
		}
	}
}

bool CDBConnSQAPI::Close() {
    m_SAConn.Disconnect();
    return true;
}

bool CDBConnSQAPI::IsConnect() {
    return m_SAConn.isConnected();
}

bool CDBConnSQAPI::ReConnect() {
    return Connect();
}

void CDBConnSQAPI::WriteLog(const char * strFormat, ...)
{
    if (m_callBackWriteLog)
    {
        char strMsg[256];
        va_list ap;
        va_start(ap, strFormat);
        vsnprintf(strMsg, sizeof(strMsg), strFormat, ap);
        va_end(ap);
        m_callBackWriteLog(strMsg);
    }
}

SAConnection &CDBConnSQAPI::getAPI()  {
    return m_SAConn;
};
