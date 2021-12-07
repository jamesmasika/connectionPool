#ifndef cta_t_alltype_h_
#define cta_t_alltype_h_
#include <string>
#include <set>
#include <map>
#include "dbfield/MainStruct.hpp"


namespace CTA {

class t_alltype : public CMainStruct<t_alltype> {
public:
	t_alltype()
	{
	}

	//t_alltype
	std::string getTableName() override
	{
		return "t_alltype";
	}

	//String,Double,Long
	AddMember(Long, 		intd);
	AddMember(Long, 		tintd);
	AddMember(Long, 		sintd);
	AddMember(Long, 		mintd);
	AddMember(Long, 		bintd);
	AddMember(String, 		bitd);
	AddMember(Double, 		dou);
	AddMember(Double, 		flo);
	AddMember(Double, 		dec);
	AddMember(String, 		ch);
	AddMember(String, 		vch);
	AddMember(String, 		text);
	AddMember(String, 		ltext);
	AddMember(String, 		js);
	AddMember(String, 		da);
	AddMember(String, 		ti);
	AddMember(String, 		year);
	AddMember(String, 		dtim);
	AddMember(String, 		ts);

private:
    MEMBER_REGISTER_FUNC(
#ifndef MYSQLCASESENSE
		MEMBER_REGISTER(intd, "intd");
		MEMBER_REGISTER(tintd, "tintd");
		MEMBER_REGISTER(sintd, "sintd");
		MEMBER_REGISTER(mintd, "mintd");
		MEMBER_REGISTER(bintd, "bintd");
		MEMBER_REGISTER(dou, "dou");
		MEMBER_REGISTER(flo, "flo");
		MEMBER_REGISTER(dec, "dec");
		MEMBER_REGISTER(ch, "ch");
		MEMBER_REGISTER(vch, "vch");
		MEMBER_REGISTER(text, "text");
		MEMBER_REGISTER(ltext, "ltext");
		MEMBER_REGISTER(js, "js");
		MEMBER_REGISTER(da, "da");
		MEMBER_REGISTER(ti, "ti");
		MEMBER_REGISTER(year, "year");
		MEMBER_REGISTER(dtim, "dtim");
		MEMBER_REGISTER(ts, "ts");
#else
		MEMBER_REGISTER(intd, "intd");
		MEMBER_REGISTER(tintd, "tintd");
		MEMBER_REGISTER(sintd, "sintd");
		MEMBER_REGISTER(mintd, "mintd");
		MEMBER_REGISTER(bintd, "bintd");
		MEMBER_REGISTER(bitd, "bitd");
		MEMBER_REGISTER(dou, "dou");
		MEMBER_REGISTER(flo, "flo");
		MEMBER_REGISTER(dec, "dec");
		MEMBER_REGISTER(ch, "ch");
		MEMBER_REGISTER(vch, "vch");
		MEMBER_REGISTER(text, "text");
		MEMBER_REGISTER(ltext, "ltext");
		MEMBER_REGISTER(js, "js");
		MEMBER_REGISTER(da, "da");
		MEMBER_REGISTER(ti, "ti");
		MEMBER_REGISTER(year, "year");
		MEMBER_REGISTER(dtim, "dtim");
		MEMBER_REGISTER(ts, "ts");
		MEMBER_REGISTER(bin, "bin");
		MEMBER_REGISTER(vbin, "vbin");
		MEMBER_REGISTER(blob, "blob");
#endif
    )

public:
	List<t_alltype> Query(SAConnection &conn) {
		std::ostringstream oss;
		oss << "SELECT " << getFieldSql() << " FROM " << getTableName();
		return queryAll(oss.str(), conn);
	}
};

} //namesapce_CTA


#endif
///////////////////////////////////////////////////////////////////////////////////


