#pragma once
//保持和com接口一致
class ICountRef
{
public:
	ICountRef(void){};
	virtual void AddRef() const = 0;
	virtual unsigned long Release() const = 0;
	virtual ~ICountRef(void){}
};
class CountRefObj
{
public:
    CountRefObj():m_lRefs(0){}
    virtual ~CountRefObj() { assert(m_lRefs == 0);}
    virtual void                        AddRef() const { InterlockedIncrement(&m_lRefs);}
    virtual unsigned long               Release() const 
    {
       if( InterlockedDecrement(&m_lRefs) == 0 )
       {
           delete this;
           return 0;
       }
       return m_lRefs;
    }
private:
    mutable unsigned long               m_lRefs;
};

template< class T>
class CountRefPtr
{
private:
    typedef CountRefPtr this_type;
public:
	typedef T element_type;

#pragma region Constructor & Destructor
	CountRefPtr()
        :px(nullptr){};

	CountRefPtr(T *p ,bool addref = true) throw()
		:px(p)
	{
		if(p!= nullptr && addref)
		{
			px->AddRef();
		}
	}

    template<class U>
    CountRefPtr( CountRefPtr<U> const &rhs) throw()
        :px(rhs.Get())
    {
         if( px != nullptr )
             px->AddRef();
    }

    CountRefPtr(CountRefPtr const & rhs) throw() :px(rhs.px)
    {
        if( px != nullptr )
            px->AddRef();
    }

    ~CountRefPtr()  throw()
    {
        if(px != nullptr )
            px->Release();
    }

    template<class U> CountRefPtr& operator= (CountRefPtr<U> const &rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }
    
    CountRefPtr(CountRefPtr && rhs )  throw():px(rhs.px)
    {
        rhs.px = nullptr;
    }

    CountRefPtr& operator = (CountRefPtr && rhs )  throw()
    {
        this_type(static_cast<CountRefPtr &&>( rhs) ).Swap(*this);
        return *this;
    }
    
    CountRefPtr& operator= (CountRefPtr const &rhs)  throw()
    {
        this_type(rhs).Swap(*this);
        return *this;
    }

    CountRefPtr& operator = (T *rhs)  throw()
    {
        this_type(rhs).Swap(*this);
        return *this;
    }
#pragma endregion  构造和析构 
    
    void Reset()  throw()
    {
        this_type().Swap( *this );
    }

    void Reset(T *rhs)  throw()
    {
        this_type( rhs ).Swap( *this );
    }
       
	T* Get() const  throw()
	{
		return px;
	}
    operator T*() const throw()
    {
        return px;
    }
	T & operator*() const  throw()
	{
		assert(px);
		return *px;
	}
	T *operator->() const  throw()
	{
		assert(px != nullptr);
		return px;
	}
    T** operator&() throw()
    {
        assert(px==NULL);
        return &px;
    }
	void Swap(CountRefPtr & rhs) 
	{
		T *tmp = px;
		px = rhs.px;
		rhs.px = tmp;
	}
    bool operator!() const throw()
    {	
        return (px == NULL);
    }
    bool operator<(T* pT) const throw()
    {
        return px < pT;
    }
    bool operator!=( T* pT) const  throw()
    {
        return !operator==(pT);
    }
    bool operator==( T* pT) const throw()
    {
        return px == pT;
    }
private:
	T *px;
};

template<class T, class U> inline bool operator==(CountRefPtr<T> const & a, CountRefPtr<U> const & b)
{
    return a.Get() == b.Get();
}

template<class T, class U> inline bool operator!=(CountRefPtr<T> const & a, CountRefPtr<U> const & b)
{
    return a.Get() != b.Get();
}

template<class T, class U> inline bool operator==(CountRefPtr<T> const & a, U * b)
{
    return a.Get() == b;
}

template<class T, class U> inline bool operator!=(CountRefPtr<T> const & a, U * b)
{
    return a.Get() != b;
}

template<class T, class U> inline bool operator==(T * a, CountRefPtr<U> const & b)
{
    return a == b.Get();
}

template<class T, class U> inline bool operator!=(T * a, CountRefPtr<U> const & b)
{
    return a != b.Get();
}

template<class T> inline bool operator<(CountRefPtr<T> const & a, CountRefPtr<T> const & b)
{
    return std::less<T *>()(a.Get(), b.Get());
}

template<class T> void swap(CountRefPtr<T> & lhs, CountRefPtr<T> & rhs)
{
    lhs.Swap(rhs);
}

template<class T> T * get_pointer(CountRefPtr<T> const & p)
{
    return p.Get();
}

template<class T, class U> CountRefPtr<T> static_pointer_cast(CountRefPtr<U> const & p)
{
    return static_cast<T *>(p.Get());
}

template<class T, class U> CountRefPtr<T> const_pointer_cast(CountRefPtr<U> const & p)
{
    return const_cast<T *>(p.Get());
}

template<class T, class U> CountRefPtr<T> dynamic_pointer_cast(CountRefPtr<U> const & p)
{
    return dynamic_cast<T *>(p.Get());
}

template<class Y> std::ostream & operator<< (std::ostream & os, CountRefPtr<Y> const & p)
{
    os << p.Get();
    return os;
}

template< class T > struct hash;

template< class T > std::size_t hash_value( CountRefPtr<T> const & p )
{
    return std::hash< T* >()( p.Get() );
}