
#ifndef DBConnectPool_h__
#define DBConnectPool_h__
#pragma once
#include "DBConnFactory.h"
#include "TimerInterface.h"
#include "DBConnContainer.h"
#include <mutex>
#include <stdint.h>

class DBAPI CDBConnectPool
{
public:
	CDBConnectPool();

	~CDBConnectPool();

	//���ӳس�ʼ��
	bool InitDBPool(
		const vector<DBCONNPARAM>& ParamVec ,	/* ��������*/
		unsigned int nInitConn ,				/* ��ʼ����������*/
		unsigned int nMinConn ,					/* ��С������*/
		unsigned int nMaxConn ,					/* ���������*/
		unsigned int nMaxIdle ,					/* ����������ʱ�䣬��λ(����)*/
		unsigned int nLoopPeriod);				/* ��ѯʱ�䣬��λ(����)*/

	//�������ӳأ���Ҫ����������ǰ����
	bool ReleaseDBPool();

	//��ʱ�ص������������ʱ����
	void  doLoopCheckDB();

	//��ȡ����
	bool  GetDBConn(const uint32_t nDBId, DBCONN& dbConn);

	//�黹����
	void  retDBConn(const uint32_t nDBId, DBCONN& dbConn);    

	virtual bool SetCallBackWriteLog(OnWriteLog callBackWriteLog);

	virtual void WriteLog(const char * strFormat, ...);

private:
	unsigned int m_InitConn;
	unsigned int m_maxConn;
	unsigned int m_minConn;
	unsigned int m_MaxIdle;
	unsigned int m_nLoopPeriod;

	CTimerInterface timer;
	typedef std::shared_ptr<CDBConnContainer> DBConnContainerPtr;
	std::map<unsigned int, DBConnContainerPtr> m_ConnContainerMap;
	std::mutex m_mutex;
	OnWriteLog	m_callBackWriteLog;
	std::shared_ptr<CDBConnectFactory> m_pFactory;
};

#endif // DBConnectPool_h__


