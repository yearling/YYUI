#pragma once
#include "YYExceptionToString.h"
#include <string>
namespace YYCOM
{
	
	class ErrorInfoBase
	{
	public:
		//用来打出来内容,如"<sturct tag_err_hr>= 0x22"
		virtual std::string				NameValueString() const =0;

	protected:
		virtual							~ErrorInfoBase() throw(){}
	};

	template<class Tag,class T>
	class ErrorInfo:public ErrorInfoBase
	{
	public:
		typedef T						ValueType;//用来保存值的类型

										ErrorInfo(ValueType const & value):value_(value){};
										~ErrorInfo() throw(){};

		ValueType const &				Value() const { return value_;}
		ValueType  &					Value()  { return value_;}
	private:
		std::string						NameValueString() const;
		ValueType						value_;
	};
	
	template<class Tag,class T>
	inline std::string					ErrorInfo<Tag,T>::NameValueString() const
	{
		return ToStringStub(*this);
	}
}