
#ifndef _DOUBLE_H_
#define _DOUBLE_H_
#include <math.h>
#include <limits.h>
#include "DbType.h"

class String;

class Double : public DbType
{
public:

	// 取整方式
	enum {
		ROUND_UP = 0,         // 从0远离
		ROUND_DOWN = 1,       // 向0靠拢
		ROUND_CEILING = 2,    // 向正向靠拢
		ROUND_FLOOR = 3,      // 向负向靠拢
		ROUND_HALF_UP = 4,    // 四舍五入
		ROUND_HALF_DOWN = 5,  // 五舍（小数部分<=0.5则舍，>0.5则入）
		ROUND_HALF_EVEN = 6,  // 舍弃部分最左边的数字：
							  // 如果为奇数，则作 ROUND_HALF_UP（四舍五入）
							  // 如果为偶数，则作 ROUND_HALF_DOWN（五舍）
		ROUND_UNNECESSARY = 7 // 不作舍入
	};

	Double();
	Double(const double value);
	Double(const String& value);
	virtual ~Double();

	virtual void clear();

	virtual void setDefaultValue();

	virtual bool fromSAField(const SADataType_t fieldTypes, const std::string& fieldData);

	virtual void toOStream(std::ostream& os) const;

    virtual std::string toString();

	double getValue() const;

	//取小数点位数
	Double setScale(int precision, int roundingMode=ROUND_HALF_UP) const;

	int intValue() const;

	static double doubleValue(const String& value);

	operator double() const;
	operator std::string() const;
	std::string str() const;

	// 加法
	const Double add(const Double& value) const;
	// 减
	const Double subtract(const Double& value) const;
	// 乘法
	const Double multiply(const Double& value) const;
	// 除法 TODO
	const Double divide(const  Double& value, int newScale/*=INT_MAX*/, int roundingMode/*=ROUND_HALF_UP*/) const;

	// 除法
	const Double divide(const Double& value) const;

	/**
	 * 除法，仿Java
	 * @param: value        [除数]
	 * @param: roundingMode [取整模式]
	 * @Added by zhaohui.sun
	 * @Added time: 2016-12-24 13:41:00
	 * @Last modified time: 2016-12-24 14:31:00
	 */
	const Double divide(const Double& value, const int& roundingMode) const;

	// 判断大于小于或等于
	int compareTo(const Double& value) const;
	/**
	 * 设置value__的值
	 * @param value [新值]
	 * @Add by:   wangzhen
	 */
	void setValue(const double value);
	
	/**
	 * 判断是否相等
	 */
	bool equals(const Double& value) const;
	bool lessEquals(const Double& value) const;
	bool moreEquals(const Double& value) const;

	friend std::ostream& operator<< (std::ostream &os, const Double& value);

	virtual bool IsZero() const;
	static bool IsZero(const double value);

	// 通过浮点型数据，获取小数点后有效位数
	static unsigned int calcDecimalNum(const double priceTick);

private:
	double value__;

public:
	static Double ZERO;
};

#endif
