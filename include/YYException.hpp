#pragma once
#include <memory>
namespace YYCOM
{
	//std::type_info��construct��˽�еģ�û����Ϊmap��keyֵ��
	//���һ�û������<��
	//������TypeInfoWrap��װһ��,ʵ��copy�ʹ�С�ж�
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

	//���̴߳���Exception�ã�Ŀǰû��ʹ��
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
	
	//����ErrorInfo������
	template <class Tag,class T>
	class ErrorInfo;
	//3��Exception����������쳣��Ϣ��
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

	//��ӿڣ���ErrorInfoContainerImpʵ��
	struct IErrorInfoContainer
	{
		virtual	char const *			DiagnosticInformation( char const * ) const =0;//��ӡ���������ErrorInfo����Ϣ��Param������������Ϣ֮ǰ����Ϣͷ

		virtual std::shared_ptr<ErrorInfoBase>
										Get(TypeInfoWrap const &) const=0;
		virtual	void					Set(std::shared_ptr<ErrorInfoBase> const &,
										TypeInfoWrap const &)=0;//ͨ��TypeInfoWrap��ȷ������Ϣ��

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
		//mutable��Ϊexception������const�ģ���Ҫ�޸�
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
	
	//������װ��std::exception�������쳣,ʹ�ô�Exception��̳�
	template <class T>
	struct ErrorInfoInjector:virtual T, virtual Exception
	{
		explicit						ErrorInfoInjector( T const & x ):T(x){}
										~ErrorInfoInjector() throw() {}
	};

	//DispatchExceptionͨ������ֵ���ж��ǲ��Ǵ�Exception��̳ж�����
	//���ص���LargeSize�Ļ���˵���Ǵ�Exception��̳ж����ģ�vice versa
	struct LargeSize {char c[256];};
	LargeSize							DispatchException(Exception const *);
	struct SmallSize {};
	SmallSize							DispatchException(void const *);

	//ͨ���ڶ���ģ��ʵ�����ж��ǲ��Ǵ�Exception��̳ж���
	//������Ǵ�Exception�̳ж����������ErrorInfoInjector����װһ��
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

	//EnableErrorInfo������������YYCOM::Exception�̳ж������쳣�ġ�
	//SFINAE����
	//ͨ��DispatchException<>�����ִ�YYCOM::Exception�̳�������ͱ���ࡣ
	//����Ǵ�YYCOM::Exception�̳����ľ�ֱ��ת����������Ǿ͵���ErrorInfoInjector����YYCOM::Exception������
	template <class T>
	inline typename EnableErrorInfoReturnType<T>::type
	EnableErrorInfo(T const & x)
	{
		typedef typename EnableErrorInfoReturnType<T>::type rt;
		return rt(x);
	}
}