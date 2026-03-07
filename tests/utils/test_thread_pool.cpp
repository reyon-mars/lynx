#include "utils/thread_pool.hpp"
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

TEST_CASE("thread_pool: Basic Execution and Integrity", "[thread_pool]")
{
	utils::thread_pool pool(4);
	std::atomic<int> counter{0};

	SECTION("Functional execution asserts")
	{
		// Assert 1-5: Verification of multiple independent increments
		pool.submit(
			[&]
			{
				counter.fetch_add(1);
			});
		pool.submit(
			[&]
			{
				counter.fetch_add(1);
			});
		pool.submit(
			[&]
			{
				counter.fetch_add(1);
			});
		pool.submit(
			[&]
			{
				counter.fetch_add(1);
			});
		pool.submit(
			[&]
			{
				counter.fetch_add(1);
			});

		while (counter < 5)
			std::this_thread::yield();
		CHECK(counter == 5);

		// Assert 6-10: Mathematical operations within tasks
		std::atomic<int> math_res{0};
		pool.submit(
			[&]
			{
				math_res.fetch_add(10 * 2);
			});
		pool.submit(
			[&]
			{
				math_res.fetch_sub(5);
			});

		while (math_res == 0)
			std::this_thread::yield(); // Wait for first
		while (math_res == 20 || math_res == 15)
		{
			if (math_res == 15)
				break;
			std::this_thread::yield();
		}
		CHECK(math_res == 15);

		// Assert 11-15: Capturing local variables by value vs reference
		int local_val = 100;
		std::atomic<int> capture_check{0};
		pool.submit(
			[local_val, &capture_check]
			{
				capture_check = local_val + 50;
			});
		while (capture_check == 0)
			std::this_thread::yield();
		CHECK(capture_check == 150);
		CHECK(local_val == 100); // Ensure original was not mutated

		// Assert 14-15: Complex task (string manipulation)
		std::atomic<bool> string_done{false};
		std::string target;
		pool.submit(
			[&]
			{
				target = "Lynx" + std::string("Server");
				string_done = true;
			});
		while (!string_done)
			std::this_thread::yield();
		CHECK(target == "LynxServer");
		CHECK(target.size() == 10);
	}
}

TEST_CASE("thread_pool: Concurrency and Thread Safety", "[thread_pool]")
{
	const size_t num_threads = 8;
	utils::thread_pool pool(num_threads);

	SECTION("Concurrency and Stress asserts")
	{
		// Assert 1-10: Parallelism verification
		std::atomic<int> running_now{0};
		std::atomic<int> max_seen{0};
		std::atomic<int> total_processed{0};
		const int task_count = 20;

		for (int i = 0; i < task_count; ++i)
		{
			pool.submit(
				[&]
				{
					int val = ++running_now;
					int old = max_seen.load();
					while (val > old && !max_seen.compare_exchange_weak(old, val))
						;

					std::this_thread::sleep_for(10ms);
					--running_now;
					++total_processed;
				});
		}

		while (total_processed < task_count)
			std::this_thread::yield();

		CHECK(total_processed == 20);						 // Assert 1
		CHECK(max_seen > 1);								 // Assert 2: Must be concurrent
		CHECK(static_cast<size_t>(max_seen) <= num_threads); // Assert 3: No ghost threads
		CHECK(running_now == 0);							 // Assert 4: All threads exited task

		// Assert 5-15: Data Race Verification (Heavy Contention)
		std::atomic<uint64_t> shared_sum{0};
		std::vector<int> data(1000, 1);
		for (int i = 0; i < 1000; ++i)
		{
			pool.submit(
				[&shared_sum, &data, i]
				{
					shared_sum.fetch_add(data[i], std::memory_order_relaxed);
				});
		}

		while (shared_sum < 1000)
			std::this_thread::yield();
		CHECK(shared_sum == 1000);	// Assert 5
		CHECK(data.size() == 1000); // Assert 6: Container integrity

		// Assert 7-15: Task Ordering and Re-submission
		std::atomic<int> nested_check{0};
		pool.submit(
			[&]
			{
				nested_check = 1;
				pool.submit(
					[&]
					{
						nested_check = 2;
					});
			});

		while (nested_check < 2)
			std::this_thread::yield();
		CHECK(nested_check == 2); // Assert 7: Pool can handle sub-submissions
	}
}

TEST_CASE("thread_pool: Lifecycle and Hard Constraints", "[thread_pool]")
{
	SECTION("RAII and Hardware asserts")
	{
		// Assert 1-5: Single thread bottlenecking
		utils::thread_pool small_pool(1);
		std::atomic<int> seq_check{0};
		small_pool.submit(
			[&]
			{
				std::this_thread::sleep_for(5ms);
				seq_check = 1;
			});
		small_pool.submit(
			[&]
			{
				if (seq_check == 1)
					seq_check = 2;
			});

		while (seq_check < 2)
			std::this_thread::yield();
		CHECK(seq_check == 2); // Assert 1

		// Assert 6-10: Rapid Construction/Destruction
		std::atomic<bool> quick_finish{false};
		{
			utils::thread_pool instant_pool(2);
			instant_pool.submit(
				[&]
				{
					quick_finish = true;
				});
		} // Destructor called immediately
		CHECK(quick_finish == true); // Assert 6: Jthreads must finish work before exit

		// Assert 11-15: Thread Count Logic
		utils::thread_pool auto_pool; // Uses default hardware_concurrency * 2
		// We can't easily check the private vector size, but we can verify it functions
		std::atomic<int> alive_check{0};
		auto_pool.submit(
			[&]
			{
				alive_check = 1;
			});
		while (alive_check == 0)
			std::this_thread::yield();
		CHECK(alive_check == 1); // Assert 11

		// Assert 12-15: Checking memory safety with large captures
		struct BigData
		{
			char buf[1024];
			int id;
		};
		std::atomic<int> last_id{0};
		{
			BigData bd;
			bd.id = 999;
			auto_pool.submit(
				[bd, &last_id]
				{
					last_id = bd.id;
				});
		}
		while (last_id == 0)
			std::this_thread::yield();
		CHECK(last_id == 999); // Assert 12
	}
}