#pragma once

#include <cstdint>
#include <numeric>

template<typename T, typename U = void>
class stream_io
{
public:
	using value_type = T;

	static auto size_of(T)->std::size_t;
};

template<typename T, typename U>
inline auto stream_io<T, U>::size_of(T) -> std::size_t
{
	return sizeof(T);
}

