#pragma once

#include <cstdint>
#include <cstring>

#include "log.h"

namespace myro
{
	struct shallow_copy{};
	struct deep_copy{};

	// Non-owning raw buffer class
	// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
	struct raw_buffer
	{
		uint8_t* data = nullptr;
		uint64_t size = 0;

		raw_buffer() = default;
		raw_buffer(uint64_t size_in) { allocate(size_in); }
		raw_buffer(void* data_in, uint64_t size_in) : data(static_cast<uint8_t*>(data_in)), size(size_in) {}
		raw_buffer(nullptr_t) {}
		raw_buffer& operator=(nullptr_t) { release(); return *this; }

		raw_buffer(const raw_buffer& other, deep_copy = deep_copy{})
		{
			if (other.data && other.size > 0)
			{
				allocate(other.size);
				std::memcpy(data, other.data, size);
			}
			else 
			{
				data = nullptr;
				size = 0;
			}
		}

		raw_buffer(const raw_buffer& other, shallow_copy)
		{
			data = other.data;
			size = other.size;
		}

		// Deep copy assignment
		raw_buffer& operator=(const raw_buffer& other) 
		{
			if (this != &other) 
			{
				if (other.data && other.size > 0) 
				{
					allocate(other.size);
					std::memcpy(data, other.data, size);
				}
				else 
				{
					release();
				}
			}
			return *this;
		}

		// Move constructor
		raw_buffer(raw_buffer&& other) noexcept 
		{
			data = other.data;
			size = other.size;
			other.data = nullptr;
			other.size = 0;
		}

		// Move assignment
		raw_buffer& operator=(raw_buffer&& other) noexcept 
		{
			if (this != &other) 
			{
				release();
				data = other.data;
				size = other.size;
				other.data = nullptr;
				other.size = 0;
			}
			return *this;
		}

		static raw_buffer copy(const raw_buffer& other)
		{
			raw_buffer result(other.size);
			std::memcpy(result.data, other.data, other.size);
			return result;
		}

		raw_buffer& shallow_copy(const raw_buffer& other)
		{
			release();
			data = other.data;
			size = other.size;
			return *this;
		}

		void allocate(uint64_t size_in)
		{
			if (size_in == size) return;

			release();

			data = new uint8_t[size_in];
			MYRO_ASSERT(data, "Memory allocation failed!");
			size = size_in;
		}

		void reallocate(uint64_t size_in)
		{
			if (size_in == size) return;
			void* temp_data = realloc(data, size_in);
			MYRO_ASSERT(temp_data, "Memory allocation failed!");
			data = static_cast<uint8_t*>(temp_data);
			size = size_in;
		}

		void release()
		{
			delete[] data;
			data = nullptr;
			size = 0;
		}

		template<typename T = uint8_t>
		T* unbound()
		{
			T* buf = reinterpret_cast<T*>(data);
			data = nullptr;
			size = 0;
			return buf;
		}

		template <class T>
		void store(const T& value)
		{
			allocate(sizeof(T));
			std::memcpy(data, &value, size);
		}

		template <class T>
		T& load()
		{
			MYRO_ASSERT(sizeof(T) <= size, "Buffer overflow!");
			return *as<T>();
		}

		template <class T>
		bool can_cast_to() const { return sizeof(T) <= size; }
		template<typename T>
		T* as() { return reinterpret_cast<T*>(data); }
		const uint8_t* begin() const { return data; }
		const uint8_t* end() const { return data + size; }
		operator bool() const { return static_cast<bool>(data); }
		size_t operator-(const raw_buffer& buffer) const { return data - buffer.data; }
	};

	// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
	struct scoped_buffer
	{
		scoped_buffer(raw_buffer buffer) : m_buffer(std::move(buffer)) {}
		scoped_buffer(uint64_t size) : m_buffer(size) {}
		~scoped_buffer() { m_buffer.release(); }

		uint8_t* data() { return m_buffer.data; }
		const uint8_t* data() const { return m_buffer.data; }
		uint8_t* unbound() { return m_buffer.unbound(); }
		uint64_t size() const { return m_buffer.size; }

		template<typename T>
		T* as() { return m_buffer.as<T>(); }

		operator bool() const { return (bool)m_buffer; }
	private:
		raw_buffer m_buffer;
	};

	namespace detail
	{
		template <uint64_t Size, uint64_t Align>
		concept valid_align = ((Align & (Align - 1)) == 0) && Size >= Align;
	}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 6385 6386)
#endif

	template <size_t MinimumSize, uint64_t Align = 1>
		requires detail::valid_align<MinimumSize, Align>
	struct stack_buffer
	{
		alignas(Align) uint8_t data[(MinimumSize + Align - 1) & ~(Align - 1)];
		static constexpr size_t size = sizeof(data);
		static constexpr uint64_t align = Align;

		constexpr void zero()
		{
			std::memset(data, 0, size);
		}

		constexpr void set(const void* src, size_t copy_size = 0)
		{
			if (copy_size == 0 || copy_size > size)
				copy_size = size;
			std::memcpy(data, src, copy_size);
		}

		template <class T>
		constexpr void store(const T& value)
		{
			static_assert(sizeof(T) <= size, "Type too large!");
			std::memcpy(data, &value, sizeof(value));
		}

		template<class T>
		constexpr T* as()
		{
			return reinterpret_cast<T*>(data);
		}

		template<class T>
		constexpr const T* as() const
		{
			return reinterpret_cast<const T*>(data);
		}

		template <class T>
		constexpr T& load()
		{
			static_assert(sizeof(T) <= size, "Type too large!");
			return *as<T>();
		}

		template <class T>
		constexpr const T& load() const
		{
			static_assert(sizeof(T) <= size, "Type too large!");
			return *as<T>();
		}

		constexpr bool filled_with_zeros() const
		{
			uint8_t temp[size]{};
			return (std::memcmp(data, temp, size) == 0);
		}

		// NOLINTNEXTLINE
		constexpr bool is_null() const { return false; }

		template <class T>
		static constexpr bool fit = sizeof(T) <= size;
	};

	// NOLINTBEGIN
	template<>
	struct stack_buffer<0, 0>
	{
		uint8_t* data = nullptr;
		static constexpr size_t size = 0;

		void zero() {}
		void set(const void*, size_t = 0) {}
		template <class T>
		void store(const T&) { static_assert(false, "null stack buffer!"); }
		template<class T>
		T* as() { return nullptr; }
		template<class T>
		const T* as() const { return nullptr; }
		template <class T>
		T& load() { static_assert(false, "null stack buffer!"); return *as<T>(); }
		template <class T>
		const T& load() const { static_assert(false, "null stack buffer!"); return *as<T>(); }
		bool filled_with_zeros() const { return true; }
		bool is_null() const { return true; }
		template <class>
		static constexpr bool fit = false;
	};
	// NOLINTEND

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

}
