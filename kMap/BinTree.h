#ifndef BINTREE_H
#define BINTREE_H

template<typename T>
class BinTree
{
public:
	BinTree() :p_data(NULL), parent(NULL), left(NULL), right(NULL), length(0){}
	BinTree(BinTree *a_parent) :parent(a_parent), left(NULL), right(NULL), p_data(NULL), length(0){}
	BinTree(T *p) :p_data(p), parent(NULL), left(NULL), right(NULL), length(0){}
	BinTree(const BinTree &rhs)
	{
		if (rhs.p_data)
		{
			p_data = new T;
			*p_data = *rhs.p_data;
		}
		else
			p_data = NULL;
		parent = NULL;
		left = build(this, rhs.left);
		right = build(this, rhs.right);
	}
	~BinTree()
	{
		if (parent == NULL)
			clear();
	}
	size_t size(){ return length; }
	T* head()
	{ 
		BinTree *p;
		if ((p = minimum(this)) != NULL)
			return p->p_data;
		else
			return NULL;
	}
	const T* head()const
	{
		const BinTree *p;
		if ((p = minimum(this)) != NULL)
			return p->p_data;
		else
			return NULL;
	}
	T* tail(){ return NULL; }
	const T* tail()const { return NULL; }
	T* next(T *t)
	{
		BinTree *p;
		BinTree *tmp = search(this, *t);
		if ((p = successor(tmp)) != NULL)
			return p->p_data;
		else
			return NULL;
	}
	const T* next(const T *t) const
	{
		const BinTree *p;
		const BinTree *tmp = search(this, *t);
		if ((p = successor(tmp)) != NULL)
			return p->p_data;
		else
			return NULL;
	}
	void clear()
	{
		if (left)
		{
			left->clear();
			delete left;
		}
		if (right)
		{
			right->clear();
			delete right;
		}
		if (p_data) delete p_data;
		p_data = NULL;
		length = 0;
	}
	T* insert(const T &t)
	{
		return __insert(t)->p_data;
	}
	T* find(const T &t)
	{
		BinTree *p;
		if ((p = search(this, t)) != NULL)
			return p->p_data;
		else
			return NULL;
	}
	const T* find(const T &t) const
	{
		const BinTree *p;
		if ((p = search(this, t)) != NULL)
			return p->p_data;
		else
			return NULL;
	}
	void erase(const T &t)
	{
		BinTree *p = search(this, t);
		if (p && p->p_data) __delete(p);
	}
	/*
	void walk(BinTree *t)
	{
	visit_vector.clear;
	if (t->p_data != NULL)
	{
	walk(t->left);
	visit_vector.push_back(t->left);
	walk(t->right);
	}
	}*/
	BinTree* build(BinTree *parent, BinTree *obj)
	{
		if (obj == NULL) return NULL;
		else
		{
			BinTree *t = new BinTree;
			if (obj->p_data)
			{
				t->p_data = new T;
				*(t->p_data) = *(obj->p_data);
			}
			t->parent = parent;
			if (obj->left)
				t->left = build(t, obj->left);
			else
				t->left = NULL;
			if (obj->right)
				t->right = build(t, obj->right);
			else
				t->right = NULL;
			return t;
		}
	}
	BinTree& operator=(const BinTree &rhs)
	{
		if (p_data != NULL) delete p_data;
		if (rhs.p_data)
		{
			p_data = new T;
			*p_data = *rhs.p_data;
		}
		else
			p_data = NULL;
		parent = NULL;
		if (left != NULL) delete left;
		left = build(this, rhs.left);
		if (right != NULL) delete right;
		right = build(this, rhs.right);
		return *this;
	}
	BinTree* __insert(const T &t)
	{
		BinTree *x = this;
		while (x->p_data != NULL)
		{
			if (t == *(x->p_data))
			{
				*(x->p_data) = t;
				return x;
			}
			if (t < *(x->p_data)) x = x->left;
			else x = x->right;
		}
		x->p_data = new T;
		*(x->p_data) = t;
		x->left = new BinTree(x);
		x->right = new BinTree(x);
		++length;
		return x;
	}
	BinTree *minimum(BinTree *sub)
	{
		while (sub->p_data != NULL)
			sub = sub->left;
		return sub->parent;
	}
	const BinTree *minimum(const BinTree *sub) const
	{
		while (sub->p_data != NULL)
			sub = sub->left;
		return sub->parent;
	}
	BinTree *maximum(BinTree *sub)
	{
		while (sub->p_data != NULL)
			sub = sub->right;
		return sub->parent;
	}
	const BinTree *maximum(const BinTree *sub) const
	{
		while (sub->p_data != NULL)
			sub = sub->right;
		return sub->parent;
	}
	BinTree *search(BinTree *sub, const T &t)
	{
		if (sub == NULL) return NULL;
		if (sub->p_data == NULL || (*sub->p_data) == t) return sub;
		if (t < *(sub->p_data))
			return search(sub->left, t);
		else
			return search(sub->right, t);
	}
	const BinTree *search(const BinTree *sub, const T &t) const
	{
		if (sub->p_data == NULL || (*sub->p_data) == t) return sub;
		if (t < *(sub->p_data))
			return search(sub->left, t);
		else
			return search(sub->right, t);
	}
	BinTree *successor(BinTree *sub)
	{
		if (sub->right->p_data != NULL) return minimum(sub->right);
		BinTree *y = sub->parent;
		while (y != NULL  && sub == y->right)
		{
			sub = y;
			y = y->parent;
		}
		return y;
	}
	const BinTree *successor(const BinTree *sub) const
	{
		if (sub->right->p_data != NULL) return minimum(sub->right);
		BinTree *y = sub->parent;
		while (y != NULL  && sub == y->right)
		{
			sub = y;
			y = y->parent;
		}
		return y;
	}
	void __delete(BinTree *sub)
	{
		BinTree *x, *y;
		if (sub->left->p_data == NULL || sub->right->p_data == NULL) y = sub;
		else y = successor(sub);
		if (y->left->p_data != NULL)
			x = y->left;
		else
			x = y->right;
		T tmp = *(y->p_data);
		if (y->parent == NULL)
		{
			if (x != y->left && y->left->p_data == NULL) delete y->left;
			if (x != y->right && y->right->p_data == NULL) delete y->right;
			y->p_data = x->p_data;
			y->left = x->left;
			y->right = x->right;
			if (y->left) y->left->parent = y;
			if (y->right) y->right->parent = y;
			if (y != sub)
				*(sub->p_data) = tmp;
			delete x;
			--length;
			return;
		}
		else
		if (y == y->parent->left) y->parent->left = x;
		else y->parent->right = x;
		if (x != NULL) x->parent = y->parent;
		if (y != sub)
			*(sub->p_data) = tmp;
		delete y;
		--length;
	}
	BinTree *parent, *left, *right;
	T *p_data;
	size_t length;
	//std::vector<T> visit_vector;
};

#endif