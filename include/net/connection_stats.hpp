#pragma once
#include <atomic>
#include <cstdint>
#include <new>
#include <utility>

namespace net
{
	class connection_stats
	{
	private:
		alignas(std::hardware_destructive_interference_size) inline static std::atomic<uint64_t> active_conn_{0};
		alignas(std::hardware_destructive_interference_size) inline static std::atomic<uint64_t> total_conn_{0};

		bool owns_count_ = true;

	public:
		connection_stats()
		{
			active_conn_.fetch_add(1, std::memory_order_relaxed);
			total_conn_.fetch_add(1, std::memory_order_relaxed);
		}

		~connection_stats()
		{
			if (owns_count_)
			{
				active_conn_.fetch_sub(1, std::memory_order_relaxed);
			}
		}

		connection_stats(const connection_stats& other) = delete;
		connection_stats& operator=(const connection_stats& other) = delete;

		connection_stats(connection_stats&& other) noexcept : owns_count_(other.owns_count_)
		{
			other.owns_count_ = false;
		}

		connection_stats& operator=(connection_stats&& other) = delete;

		static uint64_t get_active_conn()
		{
			return active_conn_.load(std::memory_order_relaxed);
		}

		static uint64_t get_total_conn()
		{
			return total_conn_.load(std::memory_order_relaxed);
		}

		static std::pair<uint64_t, uint64_t> get_stats()
		{
			return {total_conn_.load(std::memory_order_relaxed), active_conn_.load(std::memory_order_relaxed)};
		}
	};
} // namespace net