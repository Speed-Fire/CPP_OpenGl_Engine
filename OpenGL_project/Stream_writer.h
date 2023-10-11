#pragma once

#include <ostream>
#include <functional>
#include <string>
#include "ISerializavle.h"
#include "Stream_io.h"

//Base
template<typename T, typename U = void>
class stream_writer
{
public:
	static auto write(std::ostream& os, const T& value)->std::size_t;
};

template<typename T, typename U>
auto stream_writer<T, U>::write(std::ostream& os, const T& value) -> std::size_t
{
	const auto pos = os.tellp();
	os.write(reinterpret_cast<const char*>(&value), sizeof(value));
	return static_cast<std::size_t>(os.tellp() - pos);
}

//Specifications

// ISerializable specification
template<typename T>
using only_if_serializavle = std::enable_if_t<std::is_base_of_v<ISerializable, T>>;

template<typename T>
class stream_writer<T, only_if_serializavle<T>>
	: public stream_io<T>
{
public:
	static auto write(std::ostream& os, const T& value)->std::size_t;
};


// String specification
template<typename T>
using only_if_string = std::enable_if_t<std::is_same_v<T, std::string>>;

template<typename T>
class stream_writer<T, only_if_string<T>>
{
public:
	static auto write(std::ostream& os, const T& value)->std::size_t;
};

template<typename T>
inline auto stream_writer<T, only_if_string<T>>::write(std::ostream& os, const T& value) -> std::size_t
{
	const auto pos = os.tellp();
	const auto len = static_cast<std::uint32_t>(value.size());
	os.write(reinterpret_cast<const char*>(&len), sizeof(len));
	if (len > 0)
		os.write(value.data(), len);
	return static_cast<std::size_t>(os.tellp() - pos);
}



