#pragma once

#include "SpyAllocator.h"

namespace my
{
	template <typename T, class Allocator = SpyAllocator<T>>
	class list
	{
	private:
		struct node
		{
			node(const T& a) :
				m_data(a)
			{}

			node(T&& a) :
				m_data(std::move(a))
			{}

			T m_data;
			node* m_next = nullptr;
			node* m_prev = nullptr;
		};

		node* m_head = nullptr;
		node* m_tail = nullptr;
		size_t m_size = 0;

	public:

		class iterator
		{
		public:
			iterator(node* ptr) :
				m_itNode(ptr)
			{ }

			bool operator!=(const iterator& it) const
			{
				return (it.m_itNode != m_itNode);
			}
			bool operator==(const iterator& it) const
			{
				return (it.m_itNode == m_itNode);
			}

			T* operator->() const
			{
				return &m_itNode->m_data;
			}

			iterator& operator++()
			{
				*this = m_itNode->m_next;
				return *this;
			}
			iterator& operator++(int)
			{
				iterator temp(*this);
				operator++();
				return temp;
			}
			iterator& operator--()
			{
				*this = m_itNode->m_prev;
				return *this;
			}
			iterator& operator--(int)
			{
				iterator temp(*this);
				operator--();
				return temp;
			}

			T& operator*()
			{
				return m_itNode->m_data;
			}

		private:
			friend list;

			node* m_itNode = nullptr;
			size_t m_size = 0;
		};

		list() = default;

		list(const list& otherList)
		{
			node* current = otherList.m_head;

			while (current != nullptr)
			{
				push_back(current->m_data);
				current = current->m_next;
			}
		}

		list(list&& otherList) noexcept
		{
			m_size = otherList.m_size;
			otherList.m_size = 0;
			m_head = otherList.m_head;
			otherList.m_head = nullptr;
			m_tail = otherList.m_tail;
			otherList.m_tail = nullptr;
		}

		~list()
		{
			for (node* iNode = m_head; iNode != nullptr; /**/)
			{
				node* curNode = iNode;
				iNode = iNode->m_next;

				curNode->m_data.~T();
				curNode->~node();

				Allocator::template rebind<node>::other().deallocate(curNode, 1);
			}
		}

		void push_back(const T& a)
		{
			node* newNode = Allocator::template rebind<node>::other().allocate(1);

			new (newNode) node(a);
			newNode->m_prev = m_tail;
			if (m_head == nullptr)
			{
				m_head = newNode;
			}
			else
			{
				m_tail->m_next = newNode;
			}

			m_tail = newNode;
			m_size++;
		}

		void push_back(T&& a)
		{
			node* newNode = Allocator::template rebind<node>::other().allocate(1);

			new (newNode) node(std::move(a));
			newNode->m_prev = m_tail;
			if (m_head == nullptr)
			{
				m_head = newNode;
			}
			else
			{
				m_tail->m_next = newNode;
			}

			m_tail = newNode;
			m_size++;
		}

		size_t size() const noexcept
		{
			return m_size;
		}

		iterator begin()
		{
			return iterator(m_head);
		}

		iterator end()
		{
			return iterator(nullptr);
		}

		void remove(int a)
		{
			node* prevNode = m_head;
			node* currentNode = m_head->m_next;

			if (prevNode->m_data == a)
			{
				prevNode->m_next = currentNode;
				Allocator::template rebind<node>::other().deallocate(prevNode, 1);
				m_size--;
			}

			while (currentNode != nullptr)
			{
				if (currentNode->m_data == a)
				{
					currentNode->m_next->m_prev = prevNode;
					prevNode->m_next = currentNode->m_next;
					Allocator::template rebind<node>::other().deallocate(currentNode, 1);
					m_size--;
					break;
				}
				else
				{
					prevNode = currentNode;
					currentNode = currentNode->m_next;
				}
			}
		}

		iterator insert(const iterator it, const T& a)
		{
			node* newNode = Allocator::template rebind<node>::other().allocate(1);

			new(newNode) node(a);
			if (it.m_itNode->m_prev == nullptr)
			{
				newNode->m_prev = nullptr;
				newNode->m_next = it.m_itNode;
				it.m_itNode->m_prev = newNode;

				m_head = newNode;

				m_size++;
				return newNode;
			}

			newNode->m_next = it.m_itNode;
			it.m_itNode->m_prev->m_next = newNode;
			newNode->m_prev = it.m_itNode->m_prev;
			it.m_itNode->m_prev = newNode;

			m_size++;
			return newNode;
		}

		iterator insert(const iterator it, size_t st, const T& a)
		{
			iterator ite = it;
			iterator first = insert(it, a);

			for (size_t j = 0; j < st - 1; j++)
			{
				ite = insert(it, a);
			}

			return first;
		}

		template<class InputIterator>
		iterator insert(const iterator it, InputIterator head, InputIterator tail)
		{
			iterator ite = it;
			iterator first = insert(it, *head);
			head++;

			for (head; head != tail; head++)
			{
				ite = insert(it, *head);
			}

			return first;
		}

		list& operator=(list&& newList) noexcept
		{
			if (this == &newList)
			{
				return *this;
			}

			for (node* iNode = m_head; iNode != nullptr; /**/)
			{
				node* current = iNode;
				iNode = iNode->m_next;

				current->m_data.~T();
				current->~node();
				Allocator::template rebind<node>::other().deallocate(current, 1);
			}

			m_size = newList.m_size;
			newList.m_size = 0;
			m_head = newList.m_head;
			newList.m_head = nullptr;
			m_tail = newList.m_tail;
			newList.m_tail = nullptr;

			return *this;
		}

		template<class... Args>
		void emplace_back(Args... args)
		{
			node* newNode = Allocator::template rebind<node>::other().allocate(1);

			new(newNode) node(args...);
			newNode->m_prev = m_tail;
			if (m_head == nullptr)
			{
				m_head = newNode;
			}
			else
			{
				m_tail->m_next = newNode;
			}

			m_tail = newNode;
			m_size++;
		}
	};
}