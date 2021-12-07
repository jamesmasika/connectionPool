
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "String.h"
#include "Double.h"

String::String()
{
}

String::String(const int64_t value) : DbType(false)
{
	std::ostringstream oss;
	oss << value;
	value__ = oss.str();
}

String::String(const Double value) : DbType(value.null())
{
	if (!value.null()) {
		value__ = value.str();
	}
}

String::String(const char* const value)
{
	null(nullptr==value);
	if (nullptr!=value) {
		value__ = value;
	}
}

String::String(const std::string& value) : DbType(false)
{
	value__ = value;
}

String::~String()
{
}

bool String::startWith(const std::string& value) const
{
	if (value__.size() < value.size()) {
		return false;
	}
	return value == value__.substr(0, value.size());
}

bool String::endWith(const std::string& value) const
{
	if (value__.size() < value.size()) {
		return false;
	}
	return value == value__.substr(value__.size()-value.size(), value__.size());
}

bool String::replace(const std::string& oldStr, const std::string& newStr)
{
	bool replaceOk = false;
    std::string::size_type posCurrent(0);
    while(std::string::npos != (posCurrent=value__.find(oldStr.c_str(), posCurrent)))
    {
    	value__.replace(posCurrent, oldStr.length(), newStr.c_str());
    	posCurrent += newStr.length();
    	replaceOk = true;
    }
    return replaceOk;
}

void String::setValue(const String& value)
{
	null(value.null());
	value__ = value;
}

void String::clear()
{
	null(true);
	value__.clear();
}

void String::setDefaultValue()
{
	clear();
	null(false);
}

bool String::fromSAField(const SADataType_t fieldTypes, const std::string& fieldData)
{
    m_SADataType = fieldTypes;
	if (SADataType_t::SA_dtString==fieldTypes ||
            SADataType_t::SA_dtBytes==fieldTypes ||
            SADataType_t::SA_dtLongBinary==fieldTypes ||
            SADataType_t::SA_dtLongChar==fieldTypes ||
            SADataType_t::SA_dtBLob==fieldTypes ||
            SADataType_t::SA_dtCLob==fieldTypes
        )
	{
		null(false);
		value__ = fieldData;
        bSpecial__ = false;
		return true;
	}
    else if (SADataType_t::SA_dtDateTime==fieldTypes ||
            SADataType_t::SA_dtInterval==fieldTypes ) //TIME
    {
        null(false);
        value__ = fieldData;
        bSpecial__ = true;
        return true;
    }
	else if (SADataType_t::SA_dtShort ==fieldTypes ||
            SADataType_t::SA_dtUShort ==fieldTypes)  //YEAR
	{
        null(false);
        value__ = fieldData;
        bSpecial__ = true;
        return true;
	}
	return false;
}

void String::toOStream(std::ostream& os) const
{
    if(bSpecial__ && value__.empty()) {
        os << "NULL";
        return;
    }
    os << "'";
	String val(value__);
	while (val.replace("\\'", "'")) {}
	val.replace("'", "\\'");
	while ('\\' == val.c_str()[val.length()-1]) { val = val.substring(0, val.length()-1); }
	os << val.getValue();
    os << "'";
}

std::string String::toString()
{
    std::string ret;
    ret = "\"" + value__ + "\"";
    return ret;
}

std::string String::getValue() const
{
	return value__;
}

String::operator std::string() const
{
	return value__;
}

// 是否无实质数据，为null或者数据长度为0
bool String::empty() const
{
	return null() || value__.empty();
}

const char* String::c_str() const
{
	return value__.c_str();
}

const std::string String::str() const
{
	return value__;
}

//length
int String::length() const
{

	return (int)value__.length();
}

String String::valueOf(const String& value)
{
	return value;
}

String String::operator<<(const String& value) const
{
	return *this + value;
}

// 赋值运算符重载
void String::operator=(const String& value)
{
	setValue(value);
}

// 判断是否相等
bool String::equals(const String& value) const
{
	return *this == value;
}

// 判断是否相等(忽略大小写)
bool String::equalsIgnore(const String& value) const
{
	return null()==value.null() && 0==strcmp(value__.c_str(),value.value__.c_str());
}

// 等于
int String::compareTo(const String& value) const
{
	if (null() && value.null()) {
		return 0;
	} else if (!null() && value.null()) {
		return 1;
	} else if (null() && !value.null()) {
		return -1;
	} else {
		if (value__ >  value.value__) return 1;
		else if (value__ == value.value__) return 0;
		else return -1;
	}
}

// 等于号运算符重载
bool String::operator==(const String& value) const
{
	return null()==value.null() && value__==value.value__;
}

// 不等于号运算符重载
bool String::operator!=(const String& value) const
{
	return null()!=value.null() || value__!=value.value__;
}

// 小于号运算符重载
bool String::operator<(const String& value) const
{
	if (null()) {
		return true;
	} else if (!value.null()) {
		return value__ < value.value__;
	}
	return false;
}

// 小于等于号运算符重载
bool String::operator<=(const String& value) const
{
	return *this<value || *this==value;
}

// 大于号运算符重载
bool String::operator>(const String& value) const
{
	return !(*this <= value);
}

// 大于等于号运算符重载
bool String::operator>=(const String& value) const
{
	return *this>value || *this==value;
}

// 加号运算符重载
String String::operator+(const String& value) const
{
	return String(value__ + value.value__);
}

std::vector<std::string> String::split(const char seps[])
{
	std::vector<std::string> strVector;
	if (!null())
	{
		const std::string buffer(value__);
		char *mybuf = nullptr;
		char *token = strtok((char*)buffer.c_str(), seps);
		while(token != nullptr)
		{
			strVector.push_back(token);
			token = strtok(nullptr,seps);
		}
	}
	return strVector;
}

std::map<std::string, std::string> String::Split(const char seps[], const char assign)
{
	std::map<std::string, std::string> mapParams;

	std::vector<std::string> vectorParams = split(seps);
	for (unsigned int uiIndex=0; uiIndex<vectorParams.size(); ++uiIndex)
	{
		const std::string value = vectorParams[uiIndex];
		const std::string::size_type uiIndexPos = value.find_first_of(assign, 0);
		if (std::string::npos!=uiIndexPos && uiIndexPos<(value.length()-1))
		{
			mapParams.insert(std::make_pair(value.substr(0, uiIndexPos), value.substr(uiIndexPos+1)));
		}
	}
	return mapParams;
}

String String::substring(size_t begPos, size_t endPos) const
{
	std::string val = value__.substr(begPos, endPos);
	return val;
}

String& String::append(const String& value)
{
	value__.append(value.value__);
	return *this;
}
/**
 * 去掉首尾空格。对应 Java 代码中的 String PsUtil.convertToNull(String)
 * add by zhaohui.sun 2016-12-27 17:39:00
 */
String String::trim()
{
	if (value__.empty())
	{
		return String();
	}
	std::string s(value__);
	s.erase(0, value__.find_first_not_of(" "));
	s.erase(value__.find_last_not_of(" ") + 1);
	return String(s);
}

/**
* add by wangzhen
* << 重载
*/
std::ostream& operator<< (std::ostream &os, const String& value)
{
	os << value.value__;
    return os;
}

// add by hanxu
int String::indexOf( String str )
{
	std::string::size_type iPos = value__.find(str.getValue());
	if (std::string::npos == iPos)
		return -1;
	else
		return iPos;
}

std::string String::toLower() const
{
	std::string val = getValue();
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    return val;
}

std::string String::toUpper() const
{
	std::string val = getValue();
    std::transform(val.begin(), val.end(), val.begin(), ::toupper);
    return val;
}

bool String::IsZero() const
{
    return value__.empty();
}



