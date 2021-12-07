#include <iostream>
#include "DBWriter.h"

using namespace TC;

void OnCallbackWriteLog(const std::string& szMessage)
{
	printf("%s\n", szMessage.c_str());
}

#include <iostream>
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	DBWriter* pWriter = DBWriter::getInstance();
	std::vector<DBCONNPARAM> vecDBParam;
	{
		DBCONNPARAM param2;
		param2.nDBID 		= 1;
		param2.nDBType 		= 20;
		param2.szDBAddr 	= "127.0.0.1";
		param2.nDBPort 		= 3306;
		param2.szDBName 	= "yes_mng";
		param2.szDBUser 	= "root";
		param2.szDBPwd 		= "root";
		param2.nTimeOut 	= 120;
		param2.nCharSet 	= 1;
		param2.bEnable 		= true;
		vecDBParam.emplace_back(param2);
		
		int nInitNum = 2;
		int nMinNum = 1;
		int nMaxNum = 5;
		int nMaxIdle = 5;
		int nLoopPeriod = 1;
		
		if(!pWriter->InitDBPool(OnCallbackWriteLog, vecDBParam
			, nInitNum, nMinNum, nMaxNum, nMaxIdle, nLoopPeriod)) {
			std::cout << "init pool failed!" << std::endl;
			return 0;
		}

        pWriter->start();
		std::cout << "init pool success!" << std::endl;
	}

    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        pWriter->testSQLAPI(1);
    }

	int cnt = 0;
    while(1) {std::this_thread::sleep_for(std::chrono::seconds(1));}
//	while(1) {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        cnt++;
//        if(cnt%10==0) {
//            pWriter->testSQLAPI(1);
//        }
//	}
	printf("exit with code 0\n");
	return 0;
}
