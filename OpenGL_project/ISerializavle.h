#pragma once

#include <sstream>

class ISerializable
{
protected:
	~ISerializable() = default;

public:
	virtual auto serialize(std::ostream& os) const -> std::size_t = 0;
	virtual auto deserialize(std::istream& is) -> std::size_t = 0;
	virtual auto serialized_size() const noexcept -> std::size_t = 0;
};

