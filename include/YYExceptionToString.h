#pragma once
#include "YYDisableIf.h"
#include <type_traits>
#include <ostream>
#include <xtr1common>
#include <iomanip>
namespace YYCOM
{
	namespace ToStringDetial
	{
		struct PartialOrideringHelp1
		{
			template<class CharT,class Traits>
			PartialOrideringHelp1(std::basic_ostream<CharT,Traits> &);
		};

		struct PartialOrideringHelp2
		{
			template<class T>
			PartialOrideringHelp2(T const &);
		};

		char operator << (PartialOrideringHelp1,PartialOrideringHelp2);

		template<class T,class CharT,class Traits>
		struct
			IsOutPutStreamableImpl
		{
			static std::basic_ostream<CharT,Traits> & f();
			static T const & g();
			//如果是重载了operater<<或者是常用的可输出的，比如int,char,等，
			//这时候operator<<会返回一个basic_ostream的引用，也就是我们每次写cout重载时要返回cout的引用
			//sizeof(cout)一般是80
			//如果没有定义cout重载，会调用上面的那个operator
			enum e{ value=(1!=(sizeof(f() << g())))};
		};
	}
	template< class T, class CharT=char,class Traits=std::char_traits<CharT> >
	struct
		IsOutputStreamable
	{
		enum e{value=ToStringDetial::IsOutPutStreamableImpl<T,CharT,Traits>::value};
	};

	template<class T,class U>
	std::string ToString(std::pair<T,U> const &);
	std::string ToString(std::exception const &);
	namespace ToStringDetial
	{
		template<class T>
		typename disable_if<IsOutputStreamable<T>,char>::type ToString(T const &);
		using YYCOM::ToString;
		template<class,bool IsOoutStreamable>
		struct HasToStringImpl;

		template<class T>
		struct
			HasToStringImpl<T,true>
		{
			enum e{value=1};
		};
		template<class T>
		struct
			HasToStringImpl<T,false>
		{
			static T const &f();
			enum e{ value=(1!=sizeof(ToString(f())))};
		};
	}
	template<class T>
	inline
		typename std::enable_if<IsOutputStreamable<T>::value,std::string>::type
		ToString(T const & x)
	{
		std::ostringstream out;
		out<<x;
		return out.str();
	}

	template<class T>
	struct HasToString
	{
		enum e{ value=ToStringDetial::HasToStringImpl<T,IsOutputStreamable<T>::value>::value};
	};

	template<class T,class U>
	inline
		std::string
		ToString(std::pair<T,U> const &x)
	{
		return std::string("(")+ToString(x.first)+','+ToString(x.second)+')';
	}
	inline
		std::string
		ToString(std::exception const & x)
	{
		return x.what();
	}

	template<class T>
	inline std::string
		ObjectHexDump(T const & x, std::size_t max_size=16)
	{
		std::ostringstream s;
		s<<"type: "<<TypeName<T>() <<", size: " <<sizeof(T)<<",dump:";
		std::size_t n=sizeof(T) > max_size?max_size:sizeof(T);
		s.fill('0');
		s.width(2);
		unsigned char const *b=reinterpret_cast<unsigned char const *>(&x);
		s<<std::setw(2)<<std::hex<<(unsigned int )*b;
		for(unsigned char const *e=b+n;++b!=e;)
			s<<" "<<std::setw(2)<<std::hex<<(unsigned int)*b;
		return s.str();
	}
	template<class T>
	inline
		std::string
		TagTypeName()
	{
		return (typeid(T*).name());
	}
	template<class T>
	inline
		std::string
		TypeName()
	{
		return (typeid(T).name());
	}

	template <bool toStringAvailable>
	struct
		ToStringDispatcher
	{
		template<class T,class Stub>
		static
			std::string
			Convert(T const &x,Stub)
		{
			return ToString(x);
		}
	};

	template<>
	struct
		ToStringDispatcher<false>
	{
		template<class T,class Stub>
		static
			std::string
			Convert(T const &x,Stub s)
		{
			return s(x);
		}

		template<class T>
		static
			std::string
			Convert(T const &x,std::string s)
		{
			return s;
		}

		template <class T>
		static
			std::string
			Convert(T const &x, char const *s)
		{
			assert(s!=0);
			return s;
		}
	};

	template <class T,class Stub>
	inline
		std::string
		Dispatch(T const &x,Stub s)
	{
		return ToStringDispatcher<HasToString<T>::value>::Convert(x,s);
	}
	template <class T>
	inline
		std::string
		StringStubDump(T const & x)
	{
		return "["+ObjectHexDump(x)+"]";
	}
	template <class T>
	inline
		std::string
		ToStringStub(T const & x)
	{
		return Dispatch(x,&StringStubDump<T>);
	}
	template <class T,class Stub>
	inline
		std::string
		ToStringStub(T const &x,Stub s)
	{
		return Dispatch(x,s);
	}
	template<class T,class U,class Stub>
	inline
		std::string
		ToStringStub(std::pair<T,U> const &x,Stub s)
	{
		return std::string("(")+ToStringStub(x.first,s)+','+ToStringStub(x.second,s)+')';
	}
}