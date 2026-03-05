#pragma once
#include <concepts>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace utils
{
	template <typename>
	class function;

	template <typename R, typename... Args>
	class function<R(Args...)>
	{
	private:
		struct callable_base
		{
			virtual R invoke(Args... args) = 0;
			virtual callable_base* clone() const = 0;
			virtual ~callable_base() = default;
		};

		template <typename Callable>
		struct callable_impl : callable_base
		{
			Callable m_callable_;

			template <typename T>
			callable_impl(T&& c) : m_callable_(std::forward<T>(c))
			{
			}

			R invoke(Args... args) override
			{
				return m_callable_(std::forward<Args>(args)...);
			}

			callable_base* clone() const override
			{
				return nullptr;
			}
		};

		template <typename Callable>
			requires std::copy_constructible<Callable>
		struct callable_impl<Callable> : callable_base
		{
			Callable m_callable_;

			template <typename T>
			callable_impl(T&& c) : m_callable_(std::forward<T>(c))
			{
			}

			R invoke(Args... args) override
			{
				return m_callable_(std::forward<Args>(args)...);
			}

			callable_base* clone() const override
			{
				return new callable_impl<Callable>(m_callable_);
			}
		};

		callable_base* m_callable = nullptr;

	public:
		function() noexcept = default;

		template <typename T>
			requires(!std::same_as<std::decay_t<T>, function> && std::is_invocable_r_v<R, T, Args...>)
		function(T&& f) : m_callable(new callable_impl<std::decay_t<T>>(std::forward<T>(f)))
		{
		}

		~function()
		{
			delete m_callable;
		}

		function(const function& other)
		{
			if (other.m_callable)
			{
				m_callable = other.m_callable->clone();
				if (!m_callable)
				{
					throw std::runtime_error("utils::function: Attempted to copy a move-only task.");
				}
			}
			else
			{
				m_callable = nullptr;
			}
		}

		function(function&& other) noexcept : m_callable(other.m_callable)
		{
			other.m_callable = nullptr;
		}

		function& operator=(function&& other) noexcept
		{
			if (this != &other)
			{
				delete m_callable;
				m_callable = other.m_callable;
				other.m_callable = nullptr;
			}
			return *this;
		}

		function& operator=(const function& other)
		{
			if (this != &other)
			{
				callable_base* new_callable = other.m_callable ? other.m_callable->clone() : nullptr;
				if (other.m_callable && !new_callable)
				{
					throw std::runtime_error("utils::function: Attempted to copy a move-only task.");
				}
				delete m_callable;
				m_callable = new_callable;
			}
			return *this;
		}

		R operator()(Args... args) const
		{
			if (!m_callable)
			{
				throw std::bad_function_call();
			}
			return m_callable->invoke(std::forward<Args>(args)...);
		}

		explicit operator bool() const
		{
			return m_callable != nullptr;
		}
	};
} // namespace utils