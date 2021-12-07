#include "DbType.h"

DbType::DbType() : null__(true)
{

}

DbType::DbType(const bool null) : null__(null)
{
}

DbType::~DbType()
{
}

bool DbType::IsString() const
{
	return (m_SADataType == SADataType_t::SA_dtString || m_SADataType == SADataType_t::SA_dtBytes);
}

SADataType_t DbType::getDbType() const
{
    return m_SADataType;
}

bool DbType::null() const
{
	return null__;
}

// 设置null标志，返回设置的值
bool DbType::null(const bool null)
{
	null__ = null;
	return null__;
}
