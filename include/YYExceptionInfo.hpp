#pragma once
#include "YYException.hpp"
#include "YYErrorInfo.h"
#include <string>
#include <memory>
#include <assert.h>
#include <map>
#include <sstream>

namespace YYCOM
{
	//生成ErrorInfo类型的名子
	template<class Tag,class T>
	inline std::string					ErrorInfoName( ErrorInfo<Tag,T> const & x )
										{
											return  TagTypeName<Tag>();
										}

	template<class Tag,class T>
	inline std::string					ToString(ErrorInfo<Tag,T> const & x)
										{
											return '['+ErrorInfoName(x)+"]="+ToStringStub(x.Value())+'\n';
										}

	class ErrorInfoContainer:public IErrorInfoContainer
	{
	public:
										ErrorInfoContainer():count_(0){}
										~ErrorInfoContainer() throw(){}

		void							Set(std::shared_ptr<ErrorInfoBase> const &x,TypeInfoWrap const & typeid_)
										{
											assert(x);
											info_[typeid_]=x;
											diagnostic_info_str_.clear();
										}
		std::shared_ptr<ErrorInfoBase>	Get(TypeInfoWrap const &ti ) const
										{
											ErrorInfoMap::const_iterator i=info_.find(ti);
											if(info_.end()!=i)
											{
												std::shared_ptr<ErrorInfoBase> const & p=i->second;
												assert(typeid(*p)==*ti.type_);
												return p;
											}
											return std::shared_ptr<ErrorInfoBase>();
										}

		char const *					DiagnosticInformation(char const *header) const //生成所存的全部ErrorInfo的信息
										{
											if(header)
											{
												std::ostringstream tmp;
												tmp<<header;
												for(ErrorInfoMap::const_iterator i=info_.begin(),end=info_.end();
													i!=end;++i)
												{
													ErrorInfoBase const &x=*i->second;
													tmp<<x.NameValueString();
												}
												tmp.str().swap(diagnostic_info_str_);
											}
											return diagnostic_info_str_.c_str();
										}
	private:
		friend class YYCOM::Exception;

		typedef std::map<TypeInfoWrap,std::shared_ptr<ErrorInfoBase> > 
										ErrorInfoMap;

		ErrorInfoMap					info_;
		
		mutable int						count_;

		mutable std::string				diagnostic_info_str_;

										ErrorInfoContainer(ErrorInfoContainer const &);
										ErrorInfoContainer& operator=(IErrorInfoContainer const &);

		void							AddRef() const{ ++count_;}
		bool							Release() const
										{
											if(--count_) //!!FIX ME delay release??!!!
												return false;
											else
											{
												delete this;
												return true;
											}
										}
	};

	template< class E,class Tag,class T>
	inline E const &					SetInfo(E const &x,ErrorInfo<Tag,T> const &v)
										{
											typedef ErrorInfo<Tag,T> ErrorInfoTag_T;
											std::shared_ptr<ErrorInfoTag_T> p(new ErrorInfoTag_T(v));
											IErrorInfoContainer *c=x.data_.Get();
											if(!c)
												x.data_.Adopt(c=new ErrorInfoContainer);
											c->Set(p,TypeInfoWrap(typeid(ErrorInfoTag_T)));
											return x;
										}

	//value==true说明继承于Exception，vice versa
	template<class T>
	struct DeriveException
	{
		enum {value=(sizeof(DispatchException(static_cast<T*>(0)))==sizeof(LargeSize))};
	};

	template<class E,class Tag,class T>
	inline typename std::enable_if<DeriveException<E>::value,E const &>::type
										operator<<(E const &x,ErrorInfo<Tag,T> const &v)
										{
											//注意这里是const&，因为我们通常用exception()<<err_str("err")的时候，
											//err_str("err")是个临时变量，临时变量无法直接non-const lvalue reference
											return SetInfo(x,v);
										}
}