#pragma once
#include <memory>
namespace YYCOM
{
	//std::type_info的construct是私有的，没法做为map的key值。
	//并且还没有重载<号
	//所以用TypeInfoWrap包装一下,实现copy和大小判断
	struct TypeInfoWrap
	{
		explicit						TypeInfoWrap(std::type_info const & type):type_(&type){}

		friend bool						operator<(TypeInfoWrap const & rhs,TypeInfoWrap const &lhs)
		{
			return 0!=(rhs.type_->before(*lhs.type_));
		}

		friend bool						operator==(TypeInfoWrap const &rhs,TypeInfoWrap const &lhs)
		{
			return rhs.type_==lhs.type_;
		}

		std::type_info const *			type_;
	};

	//跨线程传递Exception用，目前没有使用
	template<class T>
	class RefCountPtr
	{
	public:
										RefCountPtr():pData_(NULL){};
										~RefCountPtr(){Release();}
										RefCountPtr(RefCountPtr const & x):pData_(x.pData_){AddRef();}

		RefCountPtr &					operator==(RefCountPtr const & x)
		{
			if(this==&x)
				return;
			Adopt(x.pData_);
			return *this;
		}

		void							Adopt(T *px);
		T*								Get() const;
		
	private:
		void							AddRef();
		void							Release();
		T*								pData_;
	};

	template<class T>
	void YYCOM::RefCountPtr<T>::Adopt(T *px)
	{
		Release();
		pData_=px;
		AddRef();
	}

	template<class T>
	T* YYCOM::RefCountPtr<T>::Get() const
	{
		return pData_;
	}

	template<class T>
	void YYCOM::RefCountPtr<T>::AddRef()
	{
		if(pData_)
			pData_->AddRef();
	}

	template<class T>
	void YYCOM::RefCountPtr<T>::Release()
	{
		if(pData_ && pData_->Release())
			pData_=nullptr;
	}
	
	//这里ErrorInfo的声明
	template <class Tag,class T>
	class ErrorInfo;
	//3个Exception基本定义的异常信息，
	typedef ErrorInfo<struct ThrowFunction_,char const *> ThrowFunction;
	typedef ErrorInfo<struct ThrowFile_,char const *> ThrowFile;
	typedef ErrorInfo<struct ThrowLine_,int> ThrowLine;

	template<>
	class ErrorInfo<struct ThrowFunction_,char const *>
	{
	public:
		typedef char const * ValueType;
		explicit						ErrorInfo(ValueType v):v_(v){}

		ValueType						v_;
	};

	template<>
	class ErrorInfo<struct ThrowFile_,char const *>
	{
	public:
		typedef char const * ValueType;
		explicit						ErrorInfo(ValueType v):v_(v){}

		ValueType						v_;
	};

	class Exception;
	template<>
	class ErrorInfo<struct ThrowLine_,int>
	{
	public:
		typedef int	ValueType;
		explicit						ErrorInfo(ValueType v):v_(v){}

		ValueType						v_;
	};

	class ErrorInfoBase;
	struct TypeInfo;

	//虚接口，由ErrorInfoContainerImp实现
	struct IErrorInfoContainer
	{
		virtual	char const *			DiagnosticInformation( char const * ) const =0;//打印出来所存的ErrorInfo的信息，Param是用来加在信息之前的信息头

		virtual std::shared_ptr<ErrorInfoBase>
										Get(TypeInfoWrap const &) const=0;
		virtual	void					Set(std::shared_ptr<ErrorInfoBase> const &,
										TypeInfoWrap const &)=0;//通过TypeInfoWrap来确定存信息。

		virtual	void					AddRef() const =0;
		virtual bool					Release() const =0;
	protected:
										~IErrorInfoContainer() throw() {}
	};


	class Exception
	{
	protected:
										Exception():
										throw_function_(0),
										throw_file_(0),
										throw_line_(-1)
										{}
		virtual							~Exception() throw(){};

	private:
		template<class E>
		friend E const &				SetInfo(E const &, ThrowFunction const & );

		template<class E>
		friend E const &				SetInfo(E const &, ThrowFile const & );

		template<class E>
		friend E const &				SetInfo(E const &, ThrowLine const & );

		template<class E,class Tag,class T>
		friend E const &				SetInfo( E const &, ErrorInfo<Tag,T> const & );

		friend char const *				GetDiagnosticInfomation(Exception const &, char const * );

		template<class>
		friend struct GetInfo;
		friend struct GetInfo<ThrowFunction>;
		friend struct GetInfo<ThrowFile>;
		friend struct GetInfo<ThrowLine>;
		//mutable因为exception经常是const的，但要修改
		mutable	RefCountPtr<IErrorInfoContainer> data_;
		mutable char const *			throw_function_;
		mutable char const *			throw_file_;
		mutable int						throw_line_;
	};


	template<class E>
	E const & YYCOM::SetInfo(E const & x, ThrowLine const & line)
	{
		x.throw_line_=line.v_;
		return x;
	}

	template<class E>
	E const & YYCOM::SetInfo(E const & x, ThrowFile const & file)
	{
		x.throw_file_=file.v_;
		return x;
	}

	template<class E>
	E const & YYCOM::SetInfo(E const & x, ThrowFunction const & fuc)
	{
		x.throw_function_=fuc.v_;
		return x;
	}
	
	//用来包装从std::exception过来的异常,使得从Exception里继承
	template <class T>
	struct ErrorInfoInjector:virtual T, virtual Exception
	{
		explicit						ErrorInfoInjector( T const & x ):T(x){}
										~ErrorInfoInjector() throw() {}
	};

	//DispatchException通过返回值来判断是不是从Exception类继承而来的
	//返回的是LargeSize的话，说明是从Exception类继承而来的，vice versa
	struct LargeSize {char c[256];};
	LargeSize							DispatchException(Exception const *);
	struct SmallSize {};
	SmallSize							DispatchException(void const *);

	//通过第二个模版实参来判断是不是从Exception里继承而来
	//如果不是从Exception继承而来，会调用ErrorInfoInjector来包装一下
	template<class, int>
	struct EnableErrorInfoHelper;

	template<class T>
	struct EnableErrorInfoHelper< T,sizeof(LargeSize)>
	{
		typedef T type;
	};

	template<class T>
	struct EnableErrorInfoHelper<T,sizeof(SmallSize)>
	{
		typedef ErrorInfoInjector<T> type;
	};

	template<class T>
	struct EnableErrorInfoReturnType
	{
		typedef typename EnableErrorInfoHelper<T,sizeof(DispatchException(static_cast<T *>(0)))>::type type;
	};

	//EnableErrorInfo是用来包裹非YYCOM::Exception继承而来的异常的。
	//SFINAE技巧
	//通过DispatchException<>来区分从YYCOM::Exception继承来的类和别的类。
	//如果是从YYCOM::Exception继承来的就直接转发，如果不是就调用ErrorInfoInjector来用YYCOM::Exception来包裹
	template <class T>
	inline typename EnableErrorInfoReturnType<T>::type
	EnableErrorInfo(T const & x)
	{
		typedef typename EnableErrorInfoReturnType<T>::type rt;
		return rt(x);
	}
}