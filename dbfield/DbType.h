#ifndef _DBTYPE_H_
#define _DBTYPE_H_

#include "SQLAPI.h"
#include <string>
#include <cstring>
#include <sstream>
using std::stringstream;
using std::string;

class DbType
{
public:
    DbType();
    DbType(const bool null);
	virtual ~DbType();
	//
	inline bool IsString() const;
	//
	SADataType_t getDbType() const;

	virtual void clear() = 0;

	virtual void setDefaultValue() = 0;

	virtual bool fromSAField(const SADataType_t fieldTypes, const std::string& fieldData) = 0;

	virtual void toOStream(std::ostream& os) const = 0;

	virtual bool null() const;

	virtual std::string toString() = 0;

protected:
	bool null(const bool null);

    SADataType_t m_SADataType;
private:
	bool        null__;
};

#endif
