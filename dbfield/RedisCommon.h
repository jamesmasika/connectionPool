#ifndef __REDIS_COMMON_H__
#define __REDIS_COMMON_H__
#define MAX_ERR_STR_LEN 128
#include <vector>
#include <set>
#include <string>
#include <map>
using namespace std;
#include "hiredis.h"

typedef std::string             KEY;
typedef std::string             VALUE;
typedef std::vector<KEY>        KEYS;
typedef std::vector<VALUE>      VALUES;
typedef std::vector<string>     VDATA;

typedef std::set<string>        SETDATA;
typedef struct _DATA_ITEM_{
	int    type;
	string str;
}DataItem;
typedef std::vector<DataItem>       ReplyData;
typedef ReplyData                   ArrayReply;
typedef std::map<string, double>    ZSETDATA;

typedef enum _BIT_OP_{
	AND = 0,
	OR = 1,
	XOR = 2,
	NOT = 3
}BITOP;

typedef enum _LIST_MODEL_{
	BEFORE = 0,
	AFTER = 1
}LMODEL;


#endif