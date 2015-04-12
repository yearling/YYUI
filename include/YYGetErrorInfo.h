#pragma  once
#ifndef __YYGETERRORINFO_H__
#define  __YYGETERRORINFO_H__
#include "YYException.hpp"
#include "YYExceptionInfo.hpp"
#include <typeinfo>
#include <memory>
#include <sstream>
#include <string>

namespace YYCOM
{
	template<class T>
	struct GetInfo
	{
		static typename T::ValueType *Get(Exception const &x)
		{
			if(IErrorInfoContainer *c=x.data_.Get())
			{
				if(std::shared_ptr<ErrorInfoBase> eib=c->Get(TypeInfoWrap(typeid(T))))
				{
					assert(0!=dynamic_cast<T*>(eib.get()));
					//因为返回类型不同，所以不能提供一个统一的Value接口
					T *w=static_cast<T*>(eib.get());
					return &w->Value();
				}
			}
			return 0;
		}
	};
	template<>
	struct GetInfo<ThrowFunction>
	{
		static char const **Get(Exception const &x)
		{
			return x.throw_function_? &x.throw_function_:0;
		}
	};
	template<>
	struct GetInfo<ThrowFile>
	{
		static char const **Get(Exception const &x)
		{
			return x.throw_file_? &x.throw_file_:0;
		}
	};
	template<>
	struct GetInfo<ThrowLine>
	{
		static int *Get(Exception const &x)
		{
			return x.throw_line_!=-1? &x.throw_line_:0;
		}
	};

	template<class T,class R>
	struct GetErrorInfoReturnType
	{
		typedef R * type;
	};
	template<class T,class R>
	struct GetErrorInfoReturnType<T const,R>
	{
		typedef R const *type;
	};
	template<class T,class E>
	inline
		typename GetErrorInfoReturnType<E,typename T::ValueType>::type
		GetErrorInfo(E & some_exception)
	{
		if( Exception const *x=dynamic_cast<Exception const *>(&some_exception))
			return GetInfo<T>::Get(*x);
		else
			return 0;
	}
	
	
	template<class E>
	inline E*CurrentExceptionCast()
	{
		try{throw;}
		catch( E & e)
		{
			return &e;
		}
		catch(...)
		{
			return 0;
		}
	}
	
	
	inline Exception const *GetException(Exception const *e) {return e;}
	inline Exception const *GetException(...) {return 0;}
	inline std::exception const *GetStdException(std::exception const *e)
	{
		return e;
	}
	inline std::exception const *GetStdException(...){ return 0;}

	inline char const *
		GetDiagnosticInfomation(Exception const & x, char const * Header)
	{
		try
		{
			IErrorInfoContainer* c=x.data_.Get();
			if(!c)
				x.data_.Adopt(c=new ErrorInfoContainer);
			char const *di=c->DiagnosticInformation(Header);
			assert(di!=0);
			return di;
		}
		catch(...)
		{
			return 0;
		}
	}
	
	inline std::string					DiagnosticInformationImp(
										YYCOM::Exception const *be,
										std::exception const *se,
										bool with_what,bool verbose)
	{
		if(!be && !se)
			return "Unknow exception.";
		if(!be)
			be=dynamic_cast<YYCOM::Exception const *>(se);
		if(!se)
			se=dynamic_cast<std::exception const *>(be);
		char const *wh=0;
		if(with_what &&se)
		{
			wh=se->what();
			//用来判断是不是直接enable_error_info包裹的std::exception
			if(be && GetDiagnosticInfomation(*be,0)==wh)
				return wh;
		}
		std::ostringstream tmp;
		if(be &&verbose)
		{
			char const *const *f=GetErrorInfo<ThrowFile>(*be);
			int const *l =GetErrorInfo<ThrowLine>(*be);
			char const *const *fn=GetErrorInfo<ThrowFunction>(*be);
			if(!f && !l && !fn)
				tmp<<"Throw location unkown(consider using THROW_EXCEPTION)\n";
			else
			{
				if(f)				//有文件名
				{
					tmp << *f;
					if( int const *l=GetErrorInfo<ThrowLine>(*be))
					{
						tmp<<'('<<*l<<"):";
					}
				}
				tmp<<"Throw in Function ";
				if(char const *const* fn=GetErrorInfo<ThrowFunction>(*be))
					tmp<< *fn;
				else
					tmp<<"(unknown)";
				tmp<< '\n';
			}
		}
		if(verbose)
			tmp<< std::string("Dynamic exception type: ")<<
			((be?typeid(*be).name():typeid(*se).name()))<<'\n';
		if(with_what && se &&verbose)
			tmp<<"std::exception::what: "<<wh<<'\n';
		if(be)
		{
			if(char const * s=GetDiagnosticInfomation(*be,tmp.str().c_str()))
			{
				if(*s)
					return std::string(s);
			}
		}
		return tmp.str();
	}

	inline std::string CurrentExceptionDiagnosticInformation(bool verbose=true)
	{
		YYCOM::Exception const *be=CurrentExceptionCast<YYCOM::Exception const>();
		std::exception const *se=CurrentExceptionCast<std::exception const>();
		if(be||se)
			return DiagnosticInformationImp(be,se,true,verbose);
		else
			return "No diagnostic information available";
	}

	template<class T>
	std::string DiagnosticInformation(T const &e,bool verbose=true)
	{
		return DiagnosticInformationImp(GetException(&e),GetStdException(&e),true,verbose);
	}
	inline const char *DiagnosticInormationWhat(Exception const &e,bool verbose=true) throw()
	{
		char const *w=0;
		try
		{
			(void)DiagnosticInformationImp(&e,0,false,verbose);
			if(char const *di=GetDiagnosticInfomation(e,0))
				return di;
			else
				return "Failed to produce YYCOM::DiagnosticInformationImpWhat()";
		}
		catch(...)
		{

		}
		return w;
	}
}

#endif