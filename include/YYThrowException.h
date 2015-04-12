#pragma once
#include "YYException.hpp"
namespace YYCOM
{
	inline void ThrowExceptionAssertCompatibility(std::exception const &){}
	template<class E>
	inline void ThrowException( E const & e)
	{
		ThrowExceptionAssertCompatibility(e);
		throw EnableErrorInfo(e);
	}
	template< class E>
	void ThrowExceptionImpl(E const &e,char const *current_function,char const *file,int line)
	{
		ThrowException(
			SetInfo(
			SetInfo(
			SetInfo(EnableErrorInfo(e),
			ThrowFunction(current_function)),ThrowFile(file)),ThrowLine(line)));
	}
#define THROW_EXCEPTION(x) ThrowExceptionImpl(x,__FUNCTION__,__FILE__,__LINE__)
}