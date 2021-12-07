#ifndef _LONG_H_
#define _LONG_H_

#include "DbType.h"

class Long : public DbType
{
public:
	Long();
	Long(const int64_t value);
	virtual ~Long();

	virtual void clear();

	virtual void setDefaultValue();

	virtual bool fromSAField(const SADataType_t fieldTypes, const std::string& fieldData);

	virtual void toOStream(std::ostream& os) const;

    virtual std::string toString();

	int64_t getValue() const;

	operator int64_t() const;

	static int64_t parseLong(const std::string& value);

	void setValue(const int64_t value);

	// add by hanxu
	static Long valueOf(const long& value);

	virtual bool IsZero() const;
private:
	int64_t value__;
};


#endif
