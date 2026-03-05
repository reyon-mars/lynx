#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace byte
{
	enum class byte_order
	{
		little,
		big
	};

	constexpr inline byte_order sys_endian() noexcept
	{
		uint16_t num = 0x01;
		auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(uint16_t)>>(num);
		return bytes.front() == 0 ? byte_order::big : byte_order::little;
	}

	constexpr inline bool is_little_endian() noexcept
	{
		uint16_t num = 0x01;
		auto bytes = std::bit_cast<std::array<uint8_t, sizeof(uint16_t)>>(num);
		return bytes.front() == 1;
	}

	template <typename T>
	concept byte_swappable = (std::is_integral_v<T> && !std::is_same_v<T, bool> && sizeof(T) > 1);

	template <byte_swappable T>
	constexpr inline T byte_swap(T value) noexcept
	{
		auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
		size_t size = sizeof(T);
		for (size_t i = 0; i < size / 2; i++)
		{
			std::swap(bytes[i], bytes[size - i - 1]);
		}
		return std::bit_cast<T>(bytes);
	}

	template <byte_swappable T>
	constexpr inline T order_bytes(T value, byte_order order) noexcept
	{
		auto system_order = (is_little_endian() ? byte_order::little : byte_order::big);

		if (system_order == order)
		{
			return value;
		}

		return byte_swap<T>(value);
	}
} // namespace byte