#ifndef FIRST_ARG_TYPE_HPP
#define FIRST_ARG_TYPE_HPP

#include <functional>


template <typename F>
struct first_arg_type{
};

template <template<typename> typename B, typename T>
struct first_arg_type<B<T>>{
    typedef typename first_arg_type<T>::value value;
};

template <typename FObj, typename... FArgs, typename Ret>
struct first_arg_type<Ret (FObj::*(FArgs...))>{
    typedef typename first_arg_type<Ret>::value value;
};

template <typename Ret, typename Arg>
struct first_arg_type<Ret(Arg) const>{
    typedef Arg value;
};

template <typename Ret, typename Arg>
struct first_arg_type<Ret(Arg)>{
    typedef Arg value;
};

template <typename Ret, typename Arg>
struct first_arg_type<std::function<Ret(Arg)>>{
    typedef Arg value;
};

template <typename Ret, typename Arg>
struct first_arg_type<Ret(*)(Arg)>{
    typedef Arg value;
};


#endif // FIRST_ARG_TYPE_HPP
