#pragma once

class CountRef
{
public:
	CountRef(void){};
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual int GetRefCount()const = 0;
	virtual ~CountRef(void){ Release();}
};

template< class T>
class CountRefPtr
{
public:
	typedef T element_type;
	CountRefPtr():px(nullptr){};
	CountRefPtr(T *p ,bool addref = true)
		:px(p)
	{
		if(p!= nullptr && addref)
		{
			px->AddRef();
		}
	}

	T* Get() const 
	{
		return px;
	}
	T & operator*() const 
	{
		assert(px);
		return *px;
	}
	T *operator->() const
	{
		assert(px != nullptr);
		return px;
	}

	void Swap(CountRefPtr & rhs) 
	{
		T *tmp = px;
		px = rhs.px;
		rhs.px = this;
	}
private:
	T *px;
};
