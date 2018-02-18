#ifndef _MY_vector_H_
#define _MY_vector_H_
#include<iostream>
#include <assert.h>

template<typename T>
class vector
{
private:
#define LENGTH 100;

public:
	vector() :array(0), count(0), allocCount(0)
	{

	}
	vector(const T& t, unsigned int n) :array(0), count(0), allocCount(0)
	{
		while (n--)
		{
			push_back(t);
		}
	}

	vector(const vector<T>& other) :array(0), count(0), allocCount(0)
	{
		*this = other;
	}


	~vector()
	{
		clear();
	}

	T& operator[](unsigned int pos)
	{
		assert(pos<count);
		return array[pos];
	}
	const vector<T> & operator = (const vector<T> & t)
	{
		if (&t != this)
		{
			delete[]array;
			count = t.count;
			allocCount = t.allocCount;
			array = new T[allocCount];
			for (unsigned int i = 0; i<count; i++)
			{
				array[i] = t.array[i];
			}
		}
		return *this;
	}


	unsigned int size()
	{
		return count;
	}


	unsigned int allocSize()
	{
		return allocCount;
	}

	bool empty()
	{
		return count == 0;
	}


	void clear()
	{
		deallocator(array);
		array = 0;
		count = 0;
		allocCount = 0;
	}


	void push_back(const T& t)
	{
		insert_after(count - 1, t);
	}

	void push_front(const T& t)
	{
		insert_before(0, t);
	}


	void insert_after(int pos, const T& t)
	{
		insert_before(pos + 1, t);
	}


	void insert_before(int pos, const T& t)
	{
		if (count == allocCount)
		{
			T* oldArray = array;

			allocCount += LENGTH;
			array = allocator(allocCount);
			for (unsigned int i = 0; i<count; ++i)
			{
				array[i] = oldArray[i];
			}
			deallocator(oldArray);
		}

		for (int i = (int)count++; i>pos; --i)
		{
			array[i] = array[i - 1];
		}
		array[pos] = t;
	}
	void erase(unsigned int pos)
	{
		if (pos<count)
		{
			--count;
			for (unsigned int i = pos; i<count; ++i)
			{
				array[i] = array[i + 1];
			}
		}
	}
	T *begin()
	{
		return array;
	}
	T *end()
	{
		return array + count;
	}

private:
	T*  allocator(unsigned int size)
	{
		return new T[size];
	}

	void deallocator(T* arr)
	{
		if (arr)
			delete[] arr;
	}
private:
	T*				array;
	unsigned int	count;
	unsigned int	allocCount;
};
#endif