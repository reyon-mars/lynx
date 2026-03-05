#include "utils/thread_pool.hpp"
#include <mutex>
namespace utils
{

	thread_pool::thread_pool(size_t thread_count)
	{
		workers_.reserve(thread_count);
		for (size_t i = 0; i < thread_count; i++)
		{
			workers_.emplace_back(
				[this]
				{
					while (true)
					{
						utils::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(this->queue_mtx_);
							this->cv_.wait(lock,
										   [this]
										   {
											   return this->stop_ || !this->tasks_.empty();
										   });

							if (this->stop_ && this->tasks_.empty())
							{
								return;
							}
							task = std::move(this->tasks_.front());
							this->tasks_.pop();
						}
						if (task)
						{
							try
							{
								task();
							}
							catch (...)
							{
							}
						}
					}
				});
		}
	}

	thread_pool::~thread_pool()
	{
		shutdown();
	}

	void thread_pool::shutdown()
	{
		{
			std::lock_guard<std::mutex> lock{queue_mtx_};
			stop_ = true;
		}
		cv_.notify_all();

		for (auto& worker : workers_)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}
	}
} // namespace utils