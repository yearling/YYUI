#pragma once

namespace YYCOM
{
	template <bool B,class T = void>
	struct disable_if_impl
	{
		typedef	T type;
	};
	template<class T>
	struct disable_if_impl<true,T>
	{
	};

	template<class Cond,class T =void>
	struct disable_if:public disable_if_impl<Cond::value,T>{};
}