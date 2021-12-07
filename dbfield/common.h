#ifndef __COMMON_H__
#define __COMMON_H__
#include <map>
#include <set>
#include <string>
#include <vector>
#include <chrono>
#include <stdarg.h>

#ifdef _WIN32
	#include <windows.h>
	#include <objbase.h>
#else
	#include <stdint.h>
	#include <unistd.h>
#endif

class nocopyable
{
private:
	nocopyable(const nocopyable&) = delete;
	nocopyable& operator=(const nocopyable&) = delete;
public:
	nocopyable() = default;
	~nocopyable() = default;
};

template< class T > 
class SingletonInstance
{
public:
	static	T*	Instance(void)
	{
		static	T	_instance;
		return &_instance;
	}
};

template<class _KEY, class _VALUE, class _Pr = std::less<_KEY> >
inline  _VALUE * GetKeyResult(std::map<_KEY, _VALUE, _Pr>& mapSrc, _KEY key, bool bInsert = false)
{
	_VALUE *result = NULL;
	auto it = mapSrc.find(key);
	if (it != mapSrc.end())
	{
		result = &it->second;
	}
	else
	{
		if (bInsert)
		{
			result = &mapSrc[key];
		}
	}

	return result;
}

template<class _KEY1, class _KEY2, class _VALUE, class _Pr1 = std::less<_KEY1>, class _Pr2 = std::less<_KEY2> >
inline _VALUE * GetKeyResult2(std::map<_KEY1, std::map<_KEY2, _VALUE, _Pr2>, _Pr1> & mapSrc, _KEY1 key1, _KEY2 key2, bool bInsert = false)
{
	_VALUE *result = NULL;

	auto itValue1 = GetKeyResult<_KEY1, std::map<_KEY2, _VALUE, _Pr2>, _Pr1>(mapSrc, key1, bInsert);
	if (itValue1)
	{
		result = GetKeyResult<_KEY2, _VALUE, _Pr2>(*itValue1, key2, bInsert);
	}

	return result;
}

template<class _KEY, class _VALUE, class _Pr = std::less<_KEY> >
inline  _VALUE * GetKeyResult(std::map<_KEY, _VALUE, _Pr>& mapSrc, _KEY key, bool bInsert, _VALUE value)
{
	_VALUE *result = NULL;
	auto it = mapSrc.find(key);
	if (it != mapSrc.end())
	{
		result = &it->second;
	}
	else
	{
		if (bInsert)
		{
			mapSrc[key] = value;
			result = &mapSrc[key];
		}
	}

	return result;
}

template<class _KEY1, class _KEY2, class _VALUE, class _Pr1 = std::less<_KEY1>, class _Pr2 = std::less<_KEY2> >
inline _VALUE * GetKeyResult2(std::map<_KEY1, std::map<_KEY2, _VALUE, _Pr2>, _Pr1> & mapSrc, _KEY1 key1, _KEY2 key2, bool bInsert , _VALUE value)
{
	_VALUE *result = NULL;

	auto itValue1 = GetKeyResult<_KEY1, std::map<_KEY2, _VALUE, _Pr2>, _Pr1>(mapSrc, key1, bInsert);
	if (itValue1)
	{
		result = GetKeyResult<_KEY2, _VALUE, _Pr2>(*itValue1, key2, bInsert, value);
	}

	return result;
}

template<class _KEY1, class _KEY2, class _KEY3, class _VALUE, class _Pr1 = std::less<_KEY1>, class _Pr2 = std::less<_KEY2>, class _Pr3 = std::less<_KEY3>>
inline _VALUE * GetKeyResult3(std::map<_KEY1, std::map<_KEY2, std::map<_KEY3, _VALUE, _Pr3>, _Pr2>, _Pr1> & mapSrc, _KEY1 key1, _KEY2 key2, _KEY3 key3, bool bInsert, _VALUE value)
{
	_VALUE *result = NULL;

	auto itValue1 = GetKeyResult2<_KEY1, _KEY2, std::map<_KEY3,_VALUE,_Pr3>,_Pr1, _Pr2>(mapSrc, key1, key2, bInsert);
	if (itValue1)
	{
		result = GetKeyResult<_KEY3, _VALUE, _Pr3>(*itValue1, key3, bInsert, value);
	}

	return result;
}

class CUtilTool {
		public:
    CUtilTool() {}
    ~CUtilTool() {}

    static inline std::string GetAppPath()
    {
        std::string strPath;
    #ifdef WIN32
        char pBuf[260];
        GetModuleFileNameA(NULL, pBuf, 260);
        char* pch = strrchr(pBuf, '\\');
        if (pch)
            *(pch + 1) = 0;
        else
            strcat_s(pBuf, _MAX_PATH, "\\");
        strPath = pBuf;
    #else
        char buf[260];
        int count = 260;
        int rslt = readlink("/proc/self/exe", buf, count - 1);
        if (rslt < 0 || (rslt >= count - 1))
        {
            return NULL;
        }
        buf[rslt] = '\0';
        for (int i = rslt; i >= 0; i--)
        {
            if (buf[i] == '/')
            {
                buf[i + 1] = '\0';
                break;
            }
        }
        strPath = buf;
    #endif
        return strPath;
    }

    static inline int getCurrentDate()
    {
        time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm * local = localtime(&t);
        return (local->tm_year + 1900) * 10000 + (local->tm_mon + 1) * 100 + local->tm_mday;
    }

    static inline int getYesterdayDate(int day = 1)
    {
        time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - 24*day*3600;
        struct tm * local = localtime(&t);
        return (local->tm_year + 1900) * 10000 + (local->tm_mon + 1) * 100 + local->tm_mday;
    }

    static inline int getCurrentTime()
    {
        auto nowLocalTimeCount
                = std::chrono::system_clock::now().time_since_epoch() + std::chrono::hours(8);

        std::chrono::hours now_h = std::chrono::duration_cast<std::chrono::hours>(nowLocalTimeCount);
        std::chrono::minutes now_m = std::chrono::duration_cast<std::chrono::minutes>(nowLocalTimeCount);
        std::chrono::seconds now_s = std::chrono::duration_cast<std::chrono::seconds>(nowLocalTimeCount);
        //std::chrono::milliseconds now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(nowLocalTimeCount);

        int hour = now_h.count() % 24;
        int minute = now_m.count() % 60;
        int second = now_s.count() % 60;

        return hour * 10000 + minute * 100  + second ;
    }

    static inline int getCurrentTime2()
    {
        auto nowLocalTimeCount
                = std::chrono::system_clock::now().time_since_epoch() + std::chrono::hours(8);

        std::chrono::hours now_h = std::chrono::duration_cast<std::chrono::hours>(nowLocalTimeCount);
        std::chrono::minutes now_m = std::chrono::duration_cast<std::chrono::minutes>(nowLocalTimeCount);
        std::chrono::seconds now_s= std::chrono::duration_cast<std::chrono::seconds>(nowLocalTimeCount);
        std::chrono::milliseconds now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(nowLocalTimeCount);

        int hour = now_h.count() % 24;
        int minute = now_m.count() % 60;
        int second = now_s.count() % 60;
        int msecond = now_ms.count() % 1000;

        return hour * 10000 * 1000 + minute * 100 * 1000 + second * 1000 + msecond % 1000;
    }

    static inline std::vector<std::string> splitString(std::string str, std::string pattern)
    {
        std::string::size_type pos;
        std::vector<std::string> result;
        str += pattern;
        size_t size = str.size();

        for (size_t i = 0; i < size; i++)
        {
            pos = str.find(pattern, i);
            if (pos < size)
            {
                std::string s = str.substr(i, pos - i);
                result.push_back(s);
                i = pos + pattern.size() - 1;
            }
        }
        return result;
    }

    static inline void StringReplace(std::string &strBase, const std::string& strSrc, const std::string& strDes)
    {
        std::string::size_type pos = 0;
        std::string::size_type srcLen = strSrc.size();
        std::string::size_type desLen = strDes.size();
        pos = strBase.find(strSrc, pos);
        while ((pos != std::string::npos))
        {
            strBase.replace(pos, srcLen, strDes);
            pos = strBase.find(strSrc, (pos + desLen));
        }
    }

    template<class T>
    inline 	std::string FormatValue(T value)
    {
        std::string strRet;
        char szFormat[40];
        if (value / 1000000. / 1000 > 1.0)
        {
            sprintf_s(szFormat, "%.2f billion", value / 1000000. / 1000);
        }
        else if (value / 1000000. > 1.0)
        {
            sprintf_s(szFormat, "%.2f million", value / 1000000.);
        }
        else
        {
            sprintf_s(szFormat, "%.2f", value);
        }
        strRet = szFormat;

        return strRet;
    }
		};

#endif
