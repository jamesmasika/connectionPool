#include "Long.h"

#include <stdlib.h>

Long::Long()
{
	value__ = 0;
}

Long::Long(const int64_t value) : DbType(false)
{
	value__ = value;
}

Long::~Long()
{
}

void Long::clear()
{
	null(true);
	value__ = 0;
}

void Long::setDefaultValue()
{
	clear();
	null(false);
}

bool Long::fromSAField(const SADataType_t fieldTypes, const std::string& fieldData)
{
    m_SADataType = fieldTypes;
	if (SADataType_t::SA_dtShort ==fieldTypes ||
            SADataType_t::SA_dtNumeric ==fieldTypes || /*BIGINT*/
            SADataType_t::SA_dtUShort ==fieldTypes ||
            SADataType_t::SA_dtLong ==fieldTypes ||
            SADataType_t::SA_dtBool==fieldTypes ||
            SADataType_t::SA_dtULong ==fieldTypes )
	{
		null(false);
		value__ = std::stoll(fieldData.c_str());
		return true;
	}
	else
	{
		clear();
	}
	return false;
}

void Long::toOStream(std::ostream& os) const
{
	os << value__;
}

std::string Long::toString()
{
	return std::to_string(value__);
}

int64_t Long::getValue() const
{
	return value__;
}

Long::operator int64_t() const
{
	return value__;
}

int64_t Long::parseLong(const std::string& value)
{
	return atoll(value.c_str());
}

void Long::setValue(const int64_t value)
{
	value__ = value;
	null(false);
}

Long Long::valueOf( const long& value )
{
	return value;
}

bool Long::IsZero() const
{
    return 0 == value__;
}

