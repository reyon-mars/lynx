#pragma once
#include "utils/function.hpp"
#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace utils
{
	class thread_pool
	{
	private:
		std::vector<std::jthread> workers_;
		std::queue<utils::function<void()>> tasks_;

		std::mutex queue_mtx_;
		std::condition_variable cv_;
		bool stop_ = false;

		void shutdown();

	public:
		thread_pool(size_t thread_count = (std::max<size_t>(1, std::thread::hardware_concurrency() * 2)));
		thread_pool(const thread_pool& other) = delete;
		thread_pool& operator=(const thread_pool& other) = delete;

		thread_pool(thread_pool&& other) = delete;
		thread_pool& operator=(thread_pool&& other) = delete;

		~thread_pool();

		template <typename F>
		void submit(F&& task)
		{
			{
				std::lock_guard<std::mutex> lock{queue_mtx_};
				tasks_.emplace(std::forward<F>(task));
			}
			cv_.notify_one();
		}
	};
} // namespace utils