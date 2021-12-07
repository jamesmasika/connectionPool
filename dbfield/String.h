#ifndef _STRING_H_
#define _STRING_H_

#include <map>
#include <vector>
#include "DbType.h"

class Double;

class String : public DbType
{
public:
	String();
	String(const int64_t value);
	String(const Double value);
	String(const char* const value);
	String(const std::string& value);
	virtual ~String();

	virtual bool startWith(const std::string& value) const;
	virtual bool endWith(const std::string& value) const;

	virtual bool replace(const std::string& oldStr, const std::string& newStr);

	virtual void setValue(const String& value);

	virtual void clear();

	virtual void setDefaultValue();

	virtual bool fromSAField(const SADataType_t fieldTypes, const std::string& fieldData);

	virtual void toOStream(std::ostream& os) const;

    virtual std::string toString();

	std::string getValue() const;

	operator std::string() const;

    bool empty() const;
    const char* c_str() const;
    const std::string str() const;

    static String valueOf(const String& value);

	String operator<<(const String& value) const;
	// 赋值运算符重载
	void operator=(const String& value);
	// 判断是否相等
	bool equals(const String& value) const;
	// 判断是否相等(忽略大小写)
	bool equalsIgnore(const String& value) const;
	// 等于号运算符重载
	bool operator==(const String& value) const;
	// 不等于号运算符重载
	bool operator!=(const String& value) const;
	// 小于号运算符重载
	bool operator<(const String& value) const;
	// 小于等于号运算符重载
	bool operator<=(const String& value) const;
	// 大于号运算符重载
	bool operator>(const String& value) const;
	// 大于等于号运算符重载
	bool operator>=(const String& value) const;
	// 加号运算符重载
	String operator+(const String& value) const;
	// 等于
	int compareTo(const String& value) const;
	//size
	int length() const;
	//字符截取
	String substring(size_t begPos, size_t endPos=-1) const;
	//字符追加
	String& append(const String& value);

	String trim();

	std::vector<std::string> split(const char seps[]);

	std::map<std::string, std::string> Split(const char seps[], const char assign);

	friend std::ostream& operator<< (std::ostream &os, const String& value);

	int indexOf(String str);

	std::string toLower() const;
	std::string toUpper() const;

	virtual bool IsZero() const;
private:
	std::string value__;
	bool bSpecial__;
};

#endif
