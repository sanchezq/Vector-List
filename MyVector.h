#pragma once

#include "SpyAllocator.h"

namespace my
{
	template <typename T, class Allocator = SpyAllocator<T>>
	class vector
	{
	private:

		T* m_data = nullptr;
		size_t m_capacity = 0;
		size_t m_size = 0;

	public:
		vector() = default;

		vector(const vector& other)
		{
			reserve(other.m_capacity);
			for (int iObj = 0; iObj < other.m_size; iObj++)
			{
				push_back(other[iObj]);
			}
		}

		vector(vector&& v) noexcept
		{
			m_data = v.m_data;
			m_capacity = v.m_capacity;
			m_size = v.m_size;

			v.m_data = nullptr;
			v.m_capacity = 0;
			v.m_size = 0;
		}

		~vector()
		{
			clear();
			if (m_capacity != 0)
			{
				Allocator().deallocate(m_data, m_capacity);
			}
		}

		size_t capacity() const noexcept
		{
			return m_capacity;
		}

		size_t size() const noexcept
		{
			return m_size;
		}
		
		void push_back(const T& a)
		{
			if (m_capacity == m_size)
			{
				if (m_capacity == 0)
				{
					reserve(1);
					new(&m_data[m_size]) T(a);
				}
				else
				{
					T* newArray = Allocator().allocate(m_capacity * 2);
					new(&newArray[m_size]) T(a);
					if (m_size != 0)
					{
						for (int iObj = 0; iObj < m_size; iObj++)
						{
							new (&newArray[iObj]) T(m_data[iObj]);
						}
						for (int iObj = 0; iObj < m_size; iObj++)
						{
							m_data[iObj].~T();
						}
						Allocator().deallocate(m_data, m_capacity);
					}
					m_data = newArray;
					m_capacity *= 2;
				}
			}
			else
			{
				new(&m_data[m_size]) T(a);
			}
			m_size++;
		}
		
		void push_back(T&& a)
		{
			if (m_capacity == m_size)
			{
				if (m_capacity == 0)
				{
					reserve(1);
					new(&m_data[m_size]) T(std::move(a));
				}
				else
				{
					T* newArray = Allocator().allocate(m_capacity * 2);
					new(&newArray[m_size]) T(std::move(a));
					if (m_size != 0)
					{
						for (int iObj = 0; iObj < m_size; iObj++)
						{
							new (&newArray[iObj]) T(m_data[iObj]);
						}
						for (int iObj = 0; iObj < m_size; iObj++)
						{
							m_data[iObj].~T();
						}
						Allocator().deallocate(m_data, m_capacity);
					}
					m_data = newArray;
					m_capacity *= 2;
				}
			}
			else
			{
				new(&m_data[m_size]) T(std::move(a));
			}
			m_size++;
		}

		template<class... Args>
		void emplace_back(Args... args)
		{
			if (m_capacity == m_size)
			{
				if (m_capacity == 0)
				{
					reserve(1);
					new(&m_data[m_size]) T(args...);
				}
				else
				{
					T* newArray = Allocator().allocate(m_capacity * 2);
					new(&newArray[m_size]) T(args...);
					if (m_size != 0)
					{
						for (int iObj = 0; iObj < m_size; iObj++)
						{
							new (&newArray[iObj]) T(args...);
						}
						for (int iObj = 0; iObj < m_size; iObj++)
						{
							m_data[iObj].~T();
						}
						Allocator().deallocate(m_data, m_capacity);
					}
					m_data = newArray;
					m_capacity *= 2;
				}
			}
			else
			{
				new(&m_data[m_size]) T(args...);
			}
			m_size++;
		}
		/*
		void resize(size_t n, const value_type& val)
		{

		}*/

		void resize(size_t n)
		{
			if (n < m_size)
			{
				for (size_t iObj = n; iObj < m_size; iObj++)
				{
					m_data[iObj].~T();
				}
			}

			if (n > m_capacity)
			{
				reserve(n);
			}

			if (n > m_size)
			{
				for (size_t iObj = m_size; iObj < n; iObj++)
				{
					new (&m_data[iObj]) T();
				}
			}
			m_size = n;
		}

		void clear() noexcept
		{
			for (int iObj = 0; iObj < m_size; iObj++)
			{
				m_data[iObj].~T();
			}
			m_size = 0;
		}

		void reserve(size_t newCapacity)
		{
			if (m_capacity >= newCapacity)
				return;

			T* newArray = Allocator().allocate(newCapacity);

			if (m_size != 0)
			{
				for (int iObj = 0; iObj < m_size; iObj++)
				{
					new (&newArray[iObj]) T(m_data[iObj]);
					m_data[iObj].~T();
				}

				Allocator().deallocate(m_data, m_capacity);
			}

			m_data = newArray;

			m_capacity = newCapacity;
		}

		T& operator[](const size_t n) const
		{
			return m_data[n];
		}

		T& operator[](size_t n)
		{
			return m_data[n];
		}

		vector& operator=(vector& v)
		{
			clear();
			reserve(v.m_capacity);
			for (int i = 0; i < v.m_size; i++)
			{
				push_back(v[i]);
			}
			return v;
		}

		vector& operator=(vector&& v) noexcept
		{
			clear();
			Allocator().deallocate(m_data, m_capacity);
			m_data = v.m_data;
			m_capacity = v.m_capacity;
			m_size = v.m_size;

			v.m_data = nullptr;
			v.m_capacity = 0;
			v.m_size = 0;

			return v;
		}

		class iterator
		{
		public:
			iterator(T* ptr) :
				m_ptr(ptr) 
			{ }

			bool operator==(const iterator& v) const
			{
				return (m_ptr == v.m_ptr);
			}
			bool operator!=(const iterator& v) const
			{
				return (m_ptr != v.m_ptr);
			}
			iterator& operator++()
			{
				++m_ptr;
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp(*this);
				operator++();
				return tmp;
			}
			iterator& operator--()
			{
				--m_ptr;
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp(*this);
				operator--();
				return tmp;
			}
			iterator& operator+=(int a)
			{
				m_ptr += a;
				return *this;
			}
			iterator& operator-=(int a)
			{
				m_ptr -= a;
				return *this;
			}
			T& operator*() const
			{
				return *m_ptr;
			}
			T* operator->() const
			{
				return m_ptr;
			}
			iterator operator+(int a)
			{
				return m_ptr += a;
			}
			iterator operator-(int a)
			{
				return m_ptr -= a;
			}
		private:
			friend vector;
			T* m_ptr = nullptr;
		};

		iterator begin() noexcept
		{
			return iterator(&m_data[0]);
		}

		iterator end() noexcept
		{
			return iterator(&m_data[m_size]);
		}

		/*
		const_iterator begin() const
		{
			return const_iterator(m_data);
		}*/
	};
}