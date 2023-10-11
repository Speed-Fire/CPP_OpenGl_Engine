#pragma once

#include <concepts>
#include <type_traits>
#include <string>

#include "ISerializavle.h"

template<typename T>
concept String = std::is_same_v<T, std::string>;

//template<typename T>
//concept Container = !String<T> && requires(T a)
//{
//	typename T::value_type;
//	typename T::reference;
//	typename T::const_reference;
//	typename T::iterator;
//	typename T::const_iterator;
//	typename T::size_type;
//	{ a.begin() } -> typename T::iterator;
//	{ a.end() } -> typename T::iterator;
//	{ a.cbegin() } -> typename T::const_iterator;
//	{ a.cend() } -> typename T::const_iterator;
//	{ a.clear() } -> void;
//};

