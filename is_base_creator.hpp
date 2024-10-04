#ifndef IS_BASE_CREATOR_HPP
#define IS_BASE_CREATOR_HPP

#include <type_traits>


template <typename Derived, bool is_base>
struct is_base_creator_helper
{
};

template <typename Derived>
struct is_base_creator_helper<Derived, true>
{
    typedef Derived value;
};

template <typename Base, typename Derived>
struct is_base_creator
{
    typedef Derived derived_type;
    typedef Base base_type;

    typedef typename is_base_creator_helper<Derived, std::is_base_of<Base, Derived>::value>::value value;
};


#endif // IS_BASE_CREATOR_HPP
