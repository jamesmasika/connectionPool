
#include "Double.h"
#include <stdio.h>
#include <sstream>
#include <stdlib.h>

#include "String.h"

Double Double::ZERO = Double(0);

Double::Double()
{
	value__ = 0.0;
}

Double::Double(const double value) : DbType(false)
{
	if (1.7e308 <= value) {
		value__ = 0;
	} else {
		value__ = value;
	}
}

Double::Double(const String& value)
{
	if (value.null()) {
		null(true);
		value__ = 0.0;
	} else {
		null(false);
		value__ = doubleValue(value);
	}
}

Double::~Double()
{
}

void Double::clear()
{
	null(true);
	value__ = 0.0;
}

void Double::setDefaultValue()
{
	clear();
	null(false);
}

bool Double::fromSAField(const SADataType_t fieldTypes, const std::string& fieldData)
{
    m_SADataType = fieldTypes;
	if (SADataType_t::SA_dtDouble==fieldTypes || SADataType_t::SA_dtNumeric==fieldTypes)
	{
		null(false);
		value__ = std::stod(fieldData.c_str(), nullptr);
		return true;
	}
	else
	{
		clear();
	}
	return false;
}

void Double::toOStream(std::ostream& os) const
{
	os << str();
}

std::string Double::toString()
{
    return std::to_string(value__);
}

double Double::getValue() const
{
	return value__;
}

//取小数点位数
Double Double::setScale(int precision, int/* roundingMode=ROUND_HALF_UP*/) const
{
	double dMod = 0.0000001;
	if (value__ < 0.0) dMod=-0.0000001;
	double result = value__;
	result += (5.0 / pow(10.0, precision + 1.0));
	result *= pow(10.0, precision);
	result = floor(result + dMod);
	result /= pow(10.0, precision);

	return Double(result);
}

int Double::intValue() const
{
	return value__;
}

double Double::doubleValue(const String& value)
{
	if (!value.null()) {
		return strtod(value.c_str(), nullptr);
	}
	return 0;
}

Double::operator double() const
{
	return value__;
}

Double::operator std::string() const
{
	return str();
}

std::string Double::str() const
{
	char szValue[512] = {'\0'};
	snprintf(szValue, sizeof(szValue)-1, "%.6lf", value__);
	return std::string(szValue);
}

// 加法
const Double Double::add(const Double& value) const
{
	return value__ + value.value__;
}

// 减法
const Double Double::subtract(const Double& value) const
{
	return value__ - value.value__;
}

// 乘法
const Double Double::multiply(const Double& value) const
{
	return value__ * value.value__;
}
// 除法 TODO
const Double Double::divide(const Double& value, int newScale/*=INT_MAX*/, int roundingMode/*=ROUND_HALF_UP*/) const
{
	if (newScale == roundingMode) //仅作消除编译警告之用
	{

	} 
	if( value.value__>-0.0000001 && value.value__< 0.000001 )
		return Double(0);
	return value__/value.value__;
}

// 除法
const Double Double::divide(const Double& value) const
{
	if (value.value__ > -0.0000001 && value.value__ < 0.000001)
		return Double(0);
	return value__ / value.value__;
}

/**
 * 除法，仿 Java BigDecimal
 * @param: value        [除数]
 * @param: roundingMode [取整模式]
 * @Added by zhaohui.sun
 * @Added time: 2016-12-24 13:41:00
 * @Last modified time: 2016-12-24 14:31:00
 */
const Double Double::divide(const Double& value, const int& roundingMode) const
{
	if (value.value__>-0.0000001 && value.value__< 0.000001)
		return Double(0);

	double rtn = value__ / value.value__;

	// 取整模式
	switch (roundingMode)
	{
	case ROUND_UP: // 从0远离
	{
		if (rtn >= 0.000001) {
			return ceil(rtn);
		} else {
			return floor(rtn);
		}
	}
	case ROUND_DOWN: // 向0靠拢
	{
		if (rtn >= 0.000001) {
			return floor(rtn);
		} else {
			return ceil(rtn);
		}
	}
	case ROUND_CEILING: // 向正向靠拢
	{
		return ceil(rtn);
	}
	case ROUND_FLOOR: // 向负向靠拢
	{
		return floor(rtn);
	}
	case ROUND_HALF_UP: // 四舍五入
	{
		if (rtn >= 0.0000001) {
			return ceil(rtn - 0.5);
		} else {
			return floor(rtn + 0.5);
		}
	}
	case ROUND_HALF_DOWN: // 五舍（小数部分<=0.5则舍，>0.5入）
	{
		// TODO by zhaohui.sun
		break;
	}
	case ROUND_HALF_EVEN: // 舍弃部分左边的数字：
						  // 如果为奇数，则作 ROUND_HALF_UP（四舍五入）;
						  // 如果为偶数，则作 ROUND_HALF_DOWN（五舍：五及以下舍，其余入）
	{
		// TODO by zhaohui.sun
		break;
	}
	case ROUND_UNNECESSARY: // 不作舍入
	{
		return rtn;
	}
	default:
		return rtn;
	}
	return 0;
}

// 判断大于小于或等于
int Double::compareTo(const Double& value) const
{
	if (value__ > value.value__) return 1;
	else if (value__ == value.value__) return 0;
	else return -1;
}

//设置值
void Double::setValue(const double value)
{
	value__ = value;
	null(false);
}

// 向下取整 -- 废弃，将在 setScale() 中实现。by zhaohui.sun
//const Double Double::toFloor() const
//{
//	return floor(value__);
//}

/**
 * 判断是否相等
 */
bool Double::equals(const Double& value) const
{
	const double result = value.value__ - value__;
	return (null()==value.null()) && (1e-6>fabs(result));
}

	bool Double::lessEquals(const Double& value) const
	{
		return value__ < value.value__ || equals(value);
	}

	bool Double::moreEquals(const Double& value) const
	{
		return value__ > value.value__ || equals(value);
	}

std::ostream& operator<< (std::ostream &os, const Double& value)
{
	os << value.str();
    return os;
}

bool Double::IsZero() const
{
	return IsZero(value__);
}

bool Double::IsZero(const double value)
{
	return ((value > -0.000001) && (value < 0.000001));
}

// 通过浮点型数据，获取小数点后有效位数
	unsigned int Double::calcDecimalNum(const double priceTick)
	{
		unsigned int decimalNum = 6;

		double price = (priceTick+0.0000009);
		price -= (int64_t)price;
		int64_t tick = price * 1000000;

		while (0==tick%10 && 0<decimalNum)
		{
			--decimalNum;
			tick /= 10;
		}
		return decimalNum;
	}

