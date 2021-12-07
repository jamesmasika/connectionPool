#ifndef _MAINSTRUCT_H_
#define _MAINSTRUCT_H_
#include <map>
#include <list>
#include <memory>
#include "DbType.h"
#include "String.h"
#include "Double.h"
#include "Long.h"
#include "SQLAPI.h"
//
using std::map;

#define AddMember(type, name)\
    protected:\
        type m_##name;\
    public:\
    inline void set##name(const type value) {\
        m_##name = value;\
    }\
    inline type get##name() const {\
        return m_##name;\
    }                        \

#define MEMBER_REGISTER(value, fieldName) registerMember(m_##value, fieldName)
#define MEMBER_REGISTER_FUNC(member)\
protected:\
    virtual void initMember()  {\
        m_memMap.clear();\
        member\
    }

struct FetchValues {
    int64_t nValue;
    double fValue;
    SADataType_t type;
    string sValue;
    //
    FetchValues() {
        nValue = 0;
        fValue = 0.0;
        sValue = "";
        type = SADataType_t::SA_dtUnknown;
    }
};
typedef std::map<string, FetchValues> FetchValueMapType;

template <typename Type>
class CMember
{
public:
    CMember(const std::string name, Type& value)
            : m_Name(name), m_Value(value)
    {  }

    AddMember(std::string,   Name);
    AddMember(Type&,         Value);
};

template<typename T>
class List : public std::shared_ptr<std::list<T>>
{
public:
    List() : std::shared_ptr<std::list<T>>(new std::list<T>())
    {
    }

    operator std::list<T>& ()
    {
        return *(this->get());
    }
    operator const std::list<T>& () const
    {
        return *(this->get());
    }
};

template<typename Type>
class CMainStruct {
    //
protected:
    std::map<string, CMember<DbType>> m_memMap;
    //
    CMainStruct() {}
    //
    virtual ~CMainStruct() {}
public:
    //
    virtual std::string getTableName() = 0;

protected:
    virtual void initMember() = 0;
    //
    inline void registerMember(const DbType& value, const std::string& fieldName)
    {
        m_memMap.insert(std::make_pair(fieldName, CMember<DbType>(fieldName, const_cast<DbType&>(value))));
    }

    std::string getFieldSql()
    {
        initMember();
        std::string sql;
        if(m_memMap.size() <= 0) {
            sql = "*";
            return sql;
        }
        //
        for (const auto& iterDbField : m_memMap) {
            const CMember<DbType>& member = iterDbField.second;
            if (!sql.empty()) sql += ",";
            sql += "`" + member.getName() + "`";
        }
        return sql;
    }

    std::string getValueSql()
    {
        initMember();
        std::ostringstream oss;
        for (const auto& iterDbField : m_memMap) {
            const CMember<DbType>& member = iterDbField.second;
            if (!oss.str().empty()) oss << ",";
            if (!member.getValue().null()) {
                member.getValue().toOStream(oss);
            }
            else {
                oss << "NULL";
            }
        }
        return oss.str();
    }

    void operator<< (const FetchValueMapType & valueMap)
    {
        initMember();
        for (const auto& iter : valueMap)
        {
            const std::string& name = iter.first;
            const auto& field = iter.second;
            auto localter = m_memMap.find(name);
            if (m_memMap.end() != localter)
            {
                CMember<DbType>& member = localter->second;
                if (!member.getValue().fromSAField(field.type, field.sValue)) {
                    string errorMsg(String("table(")+getTableName()+")field("+name+")field_type("
                                          + field.type + ") not match (" + member.getValue().getDbType() + ") value ("+field.sValue+")");
                    throw SAException(SAErrorClass_t::SA_UserGenerated_Error, 0, 0, errorMsg.c_str());
                }
            } else {
                string errorMsg(String("table(")+getTableName()+")field("+name+") not exist");
                throw SAException(SAErrorClass_t::SA_UserGenerated_Error, 0, 0, errorMsg.c_str());
            }
        }
    }

public:
    std::string toJson() {
        initMember();
        std::ostringstream  ss;
        ss << "{";
        size_t cnt = 0;
        size_t totcnt = m_memMap.size();
        for(auto& mem : m_memMap) {
            ss << "\"";
            ss << mem.second.getName();
            ss << "\":";
            ss << mem.second.getValue().toString();
            cnt++;
            if(cnt != totcnt) {
                ss << ",";
            }
        }
        ss << "}";
        return ss.str();
    }

    List<Type> queryAll(const std::string& sql, SAConnection &conn)
    {
        initMember();
        List<Type> listValue;
        try {
            SACommand cmd(&conn);

            cmd.setCommandText(sql.c_str());
            cmd.Execute();

            while (cmd.FetchNext()) {
                Type value;
                FetchValueMapType _valueMap;
                for (const auto &mem : m_memMap) {
                    auto &field_ = cmd.Field(mem.second.getName().c_str());
                    auto &fetVal = _valueMap[mem.second.getName()];
                    fetVal.sValue = (const char *) field_.asString();
                    fetVal.type = field_.DataType();
                }
                value << _valueMap;
                listValue->push_back(value);}
        }
        catch (SAException& e) {
            printf("catch SAException=[%s], sql=[%s]\n", (const char*)e.ErrText(), sql.c_str());
        }

        return listValue;
    }

    static void exec(const string& sql, SAConnection &conn)
    {
        try {
            SACommand cmd(&conn);

            cmd.setCommandText(sql.c_str());
            cmd.Execute();
        }
        catch (SAException& e) {
            printf("catch SAException=[%s], sql=[%s]\n", (const char*)e.ErrText(), sql.c_str());
        }
    }

    static void replaceAll(std::list<Type>& listValues, SAConnection &conn)
    {
        if(listValues.size() <= 0)
            return;
        //
        Type& temp = listValues.front();
        std::ostringstream oss;
        oss << "REPLACE INTO " << temp.getTableName() << " (" << temp.getFieldSql() << ") VALUES ";
        size_t cnt = 0;
        for (auto& value : listValues)
        {
            if (0 != cnt) oss << ",";
            oss << "(" << value.getValueSql() << ")";
            cnt++;
        }
        string sql = oss.str();
        exec(sql, conn);
    }

    void replaceOrInsert(SAConnection &conn,  bool insert = false)
    {
        string cmd = "INSERT INTO ";
        if(!insert) {
            cmd = "REPLACE INTO ";
        }
        std::ostringstream oss;
        oss << cmd << getTableName() << " (" << getFieldSql() << ") VALUES ";
        oss << "(" << getValueSql() << ")";
        string sql = oss.str();
        exec(sql, conn);
    }
};

#endif