#ifndef MAP_H
#define MAP_H
#include "BinTree.h"

template<typename key_type, typename mapped_type>
class pair
{
public:
	pair(){}
	pair(key_type key) :first(key){}
	key_type first;
	mapped_type second;
	pair &operator=(const pair &rhs)
	{
		first = rhs.first;
		second = rhs.second;
		return *this;
	}
};

template<typename key_type, typename mapped_type>
bool operator<(const pair<key_type, mapped_type> &lhs, const pair<key_type, mapped_type> &rhs)
{
	return lhs.first < rhs.first;
}

template<typename key_type, typename mapped_type>
bool operator==(const pair<key_type, mapped_type> &lhs, const pair<key_type, mapped_type> &rhs)
{
	return lhs.first == rhs.first;
}

/*
template<typename T>
class BinTree
{
public:
	T a[64];
	size_t length;
	BinTree() :length(0){}
	size_t size(){ return length; }
	T *head() { return a; }
	const T *head() const { return a; }
	T *tail(){ return a + length; }
	const T *tail() const { return a + length; }
	void clear(){ length = 0; }
	T* insert(const T &t)
	{
		size_t i;
		for (i = 0; i < length; ++i)
		if (a[i] == t)
		{
			a[i] = t;
			return a + i;
		}
		a[i] = t;
		++length;
		return a + i;
	}
	const T* find(const T &t) const
	{
		for (size_t i = 0; i < length; ++i)
		if (a[i] == t) return a + i;
		return a + length;
	}
	T* find(const T &t)
	{
		for (size_t i = 0; i < length; ++i)
		if (a[i] == t) return a + i;
		return a + length;
	}
	const T* next(const T *t) const
	{
		return ++t;
	}
	T* next(T *t)
	{
		return ++t;
	}
	void erase(const T &t)
	{
		for (size_t i = 0; i <length; ++i)
		if (a[i] == t)
		for (size_t j = i; j < length - 1; ++j) a[j] = a[j + 1];
		--length;
	}
};
*/

template<typename T>
class tree_base_iterator
{
	template<typename U> friend bool operator!=(const tree_base_iterator<U> &lhs, const tree_base_iterator<U> &rhs);
	template<typename U> friend bool operator==(const tree_base_iterator<U> &lhs, const tree_base_iterator<U> &rhs);
public:
	tree_base_iterator(){}
	tree_base_iterator(BinTree<T> *p_t, T *p) :p_tree(p_t), pointer(p){}
	//tree_base_iterator(const BinTree<T> *p_t, const T *p) :p_tree(p_t), pointer(p){}
	T& operator*() const
	{
		return *pointer;
	}
	T* operator->() const
	{
		return pointer;
	}
	tree_base_iterator operator++()
	{
		return (*this) = tree_base_iterator(p_tree, p_tree->next(pointer));
	}
private:
	BinTree<T> *p_tree;
	T *pointer;
};

template<typename T>
bool operator!=(const tree_base_iterator<T> &lhs, const tree_base_iterator<T> &rhs)
{
	return lhs.pointer != rhs.pointer;
}

template<typename T>
bool operator==(const tree_base_iterator<T> &lhs, const tree_base_iterator<T> &rhs)
{
	return lhs.pointer == rhs.pointer;
}

template<typename T>
class tree_base_const_iterator
{
	template<typename U> friend bool operator!=(const tree_base_const_iterator<U> &lhs, const tree_base_const_iterator<U> &rhs);
	template<typename U> friend bool operator==(const tree_base_const_iterator<U> &lhs, const tree_base_const_iterator<U> &rhs);
public:
	tree_base_const_iterator(){}
	//tree_base_const_iterator(BinTree<T> *p_t, T *p) :p_tree(p_t), pointer(p){}
	tree_base_const_iterator(const BinTree<T> *p_t, const T *p) :p_tree(p_t), pointer(p){}
	const T& operator*() const
	{
		return *pointer;
	}
	const T* operator->() const
	{
		return pointer;
	}
	tree_base_const_iterator operator++()
	{
		return (*this) = tree_base_const_iterator(p_tree, p_tree->next(pointer));
	}
private:
	const BinTree<T> *p_tree;
	const T *pointer;
};

template<typename T>
bool operator!=(const tree_base_const_iterator<T> &lhs, const tree_base_const_iterator<T> &rhs)
{
	return lhs.pointer != rhs.pointer;
}

template<typename T>
bool operator==(const tree_base_const_iterator<T> &lhs, const tree_base_const_iterator<T> &rhs)
{
	return lhs.pointer == rhs.pointer;
}


template<typename key_type, typename mapped_type>
class map
{
public:
	map(){}
	map(const map &rhs) :tree(rhs.tree){}
	typedef tree_base_iterator<pair<key_type, mapped_type> > iterator;
	typedef tree_base_const_iterator<pair<key_type, mapped_type> > const_iterator;
	iterator begin(){ return iterator(&tree, tree.head()); }
	const_iterator begin() const { return const_iterator(&tree, tree.head()); }
	iterator end(){ return iterator(&tree, tree.tail()); }
	const_iterator end() const { return const_iterator(&tree, tree.tail()); }
	size_t size(){ return tree.size(); }
	void clear(){ tree.clear(); }
	mapped_type& operator[](key_type key)
	{
		if (tree.find(pair<key_type, mapped_type>(key)) != tree.tail())
			return tree.find(key)->second;
		return tree.insert(pair<key_type, mapped_type>(key))->second;
	}
	const mapped_type& operator[](key_type key) const
	{
		if (tree.find(pair<key_type, mapped_type>(key)) != tree.tail())
			return tree.find(key)->second;
		return tree.insert(pair<key_type, mapped_type>(key))->second;
	}
	iterator find(key_type key)
	{
		return iterator(&tree, tree.find(pair<key_type, mapped_type>(key)));
	}
	const_iterator find(key_type key) const
	{
		return const_iterator(&tree, tree.find(pair<key_type, mapped_type>(key)));
	}
	void erase(const key_type key)
	{
		tree.erase(pair<key_type, mapped_type>(key));
	}
	map& operator=(const map &rhs)
	{
		tree = rhs.tree;
		return *this;
	}
private:
	BinTree<pair<key_type, mapped_type> > tree;
};

#endif