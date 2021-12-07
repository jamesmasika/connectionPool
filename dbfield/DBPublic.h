#ifndef DBPublic_h__
#define DBPublic_h__
#include <string>
#include <functional>
#include "common.h"

typedef void( *OnWriteLog)(const std::string& szMessage);

#ifdef _WIN32
#ifndef NODBAPI_EXPORTS
#define	DBAPI
#elif defined DBAPI_EXPORTS
#define	DBAPI	__declspec(dllexport)
#else
#define	DBAPI	__declspec(dllimport) 
#endif
#else
#define	DBAPI 
#endif

enum enmDBType{
	DB_MYSQL = 0,
	DB_REDIS = 10,
    DB_SQAPI = 20,
};

enum enmCharSetEncode{
	CHARSET_ENCODE_NOT = 0,
	CHARSET_ENCODE_UTF8,
};

#endif // DBPublic_h__
