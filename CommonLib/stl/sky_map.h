#include "stl_pair.h"

namespace sky
{
	template <class T, class V>
	struct node
	{
		T key;
		V value;
		struct node<T, V> *left;
		struct node<T, V> *right;
		struct node<T, V> *father;
		int height;
	};

	template <class T, class V>
	class map
	{
	public:
		typedef T key_type;
		typedef pair<T, V> value_type;
		typedef ptrdiff_t difference_type;
		typedef size_t size_type;
		//typedef typename  map<T, V>:: iterator reference;
		struct node<T, V>  *root;
		size_t size1;
		//default constructor
		map()
		{
			root = NULL;
			size1 = 0;
		}

		//copy constructor
		map(const map<T, V>  &x);

		//insert
		bool insert(pair<T, V> c)
		{
			int t = insert_node(&root, c.first, c.second);
			if (t == 1) {
				size1++;
			}
			return t;
		}

		//iterator
		class iterator
		{
		public:
			struct node<T, V> *it;
			struct node<T, V> *root1;
			pair<T, V> s;
			iterator()
			{
				it = NULL;
			}

			void operator = (typename map<T, V>::iterator nit)
			{
				it = nit.it;
				root1 = nit.root1;
			}

			pair<T, V>* operator-> () {
				//	pair<T, V> s;
				s.first = it->key;
				s.second = it->value;
				return (&s);
			}

			pair<T, V> operator * ()
			{
				//pair<T, V> s;
				s.first = it->key;
				s.second = it->value;
				return s;
			}

			bool operator != (typename map<T, V>::iterator nit)
			{
				if (nit.it == it) {
					return false;
				}
				else {
					return true;
				}
			}

			iterator operator ++(int x) {
				map<T, V>::iterator r = (*this);
				++(*this);
				return r;
			}
			iterator  operator ++() {
				T key = it->key;
				node<T, V> *temp = NULL;
				int flag = 0;
				node<T, V> *p = NULL;
				temp = find_it(root1, key, flag, &p);
				//cout << "key = " << p -> key << endl;
				iterator it1;
				it1.it = p;
				it1.root1 = root1;
				*this = it1;
				if (flag != 2) {
					it1.it = NULL;
					it1.root1 = root1;
					*this = it1;
				}
				//cout << "this = " << (*this).it -> value;
				return (*this);
			}
			node<T, V> *find_it(node<T, V> *root1, T data, int &flag, node<T, V> **p) {
				if (root1 != NULL) {
					// cout << " data = " << data << " flag =  " << flag << endl;
					find_it(root1->left, data, flag, &(*p));
					if (data == root1->key) {
						//    cout << "flag = 1 at " << root1 -> key << endl;
						flag = 1;
					}
					else if (flag == 1) {
						//  cout << "flag = 2 at " << root1 -> key << endl;
						flag = 2;
						(*p) = root1;
						return root1;
					}
					find_it(root1->right, data, flag, &(*p));
				}
			}
			iterator operator --(int x) {
				map<T, V>::iterator r = (*this);
				--(*this);
				return r;
			}
			iterator operator --() {
				T key = it->key;
				node<T, V> *temp = NULL;
				int flag = 0;
				node<T, V> *p = NULL;
				temp = find_it_minus(root1, key, flag, &p);
				iterator it1;
				it1.it = p;
				it1.root1 = root1;
				*this = it1;
				return (*this);
			}
			node<T, V> *find_it_minus(node<T, V> *root1, T data, int &flag, node<T, V> **p) {
				if (root1 != NULL) {

					find_it_minus(root1->right, data, flag, &(*p));
					if (data == root1->key) {
						flag = 1;
					}
					else if (flag == 1) {
						flag = 2;
						(*p) = root1;
						return root1;
					}
					find_it_minus(root1->left, data, flag, &(*p));
				}
			}

		};
		size_t size() const {
			return size1;
		}
		iterator begin()const
		{
			iterator its;
			its.it = root;
			its.root1 = root;
			if (root == NULL) {
				return its;
			}

			while (its.it->left != NULL) {
				its.it = its.it->left;
			}

			return its;
		}

		iterator end() const
		{
			iterator itn;
			itn.it = NULL;
			itn.root1 = root;
			return itn;
		}
		void clear() {
			size1 = 0;
			delete (root);
			root = NULL;
		}

		void erase(iterator it);
		bool empty();
		bool count(T key);
		iterator find(T key);
		V& operator [] (T a);
		iterator lower_bound(T key);
		iterator upper_bound(T key);
		pair<typename map<T, V>::iterator, typename map<T, V>::iterator> equal_range(T key);
		iterator rend() const;
		iterator rbegin() const;
		bool delete_node(node<T, V> **p, T key);
		void swap(map<T, V> &m);
		node<T, V>* search(node<T, V> *p, T key);
		void preorder(node<T, V> *p);
		int max(int a, int b);
		bool insert_node(node<T, V> **p, T key, V value);
		node<T, V>* rotate_right(node<T, V> *y);
		node<T, V>* rotate_left(node<T, V> *x);
		node<T, V>* my_fun(node<T, V> *r, node<T, V> *p, T i);
		int difference_of_heights(node<T, V> *n);
		int height(node<T, V> *s);
		size_t erase(T key);
	};
	//////////////class declaration ends ///////////////////////////////

	template <class T, class V>
	size_t map<T, V>::erase(T key)
	{
		if (this->delete_node(&(root), key)) {
			size1--;
		}

		return size1;
	}
	template <class T, class V>
	map<T, V>::map(const map<T, V> &m)
	{
		iterator it;
		it = m.begin();
		root = NULL;
		size1 = 0;
		for (it = m.begin(); it != m.end(); it++) {
			this->insert(pair<T, V>((*it).first, (*it).second));
		}
	}

	template <class T, class V>
	void map<T, V>::swap(map<T, V> &m)
	{
		node<T, V> *p = NULL;
		p = root;
		this->root = m.root;
		m.root = p;
	}

	template <class T, class V>
	typename map<T, V>::iterator map<T, V>::rend() const
	{
		return (this->begin());
	}

	template <class T, class V>
	typename map<T, V>::iterator map<T, V>::rbegin() const
	{
		return (this->end());
	}

	template <class T, class V>
	pair<typename map<T, V>::iterator, typename map<T, V>::iterator> map<T, V>::equal_range(T key)
	{
		pair<iterator, iterator> p;
		p.first = this->lower_bound(key);
		p.second = this->upper_bound(key);
		return p;
	}
	template <class T, class V>
	typename map<T, V>::iterator map<T, V>::upper_bound(T key)
	{
		iterator it;
		it.root1 = root;
		node <T, V> *n = NULL;
		n = my_fun(root, root, key);
		it.it = n;
		it++;
		return it;
	}
	template <class T, class V>
	typename map<T, V>::iterator map<T, V>::lower_bound(T key)
	{
		iterator it;
		it.root1 = root;
		node<T, V> *n = NULL;

		n = my_fun(root, root, key);
		if (n->key == key) {
			it.it = n;
		}
		else {
			it.it = n;
			it++;

		}
		return it;
	}

	template <class T, class V>
	node<T, V>* map<T, V>::my_fun(node<T, V> *r, node<T, V> *p, T x)
	{
		if (r == NULL) {
			return p;
		}
		else if (r->key == x) {
			return r;
		}
		else if (r->key < x) {
			my_fun(r->right, r, x);
		}
		else {
			my_fun(r->left, r, x);
		}

	}

	template <class T, class V>
	V& map<T, V>::operator [](T a)
	{
		T y;
		y = a;
		iterator it;
		node<T, V> *s = NULL;
		s = search(root, a);
		if (s != NULL) {
			return (s->value);
		}
		else {
			V c;
			this->insert(pair<T, V>(a, c));
			s = search(root, a);
			return (s->value);
		}
	}

	template <class T, class V>
	typename map<T, V>::iterator map<T, V>::find(T key)
	{
		map<T, V>::iterator it1;
		it1.root1 = root;
		node<T, V> *p = NULL;
		p = search(root, key);
		it1.it = p;
		return it1;

	}
	template <class T, class V>
	bool map<T, V>::count(T key)
	{
		node<T, V> *p = NULL;
		p = search(root, key);
		if (p == NULL) {
			return false;
		}
		else {
			return true;
		}
	}
	template <class T, class V>
	node<T, V>* map<T, V>::search(node<T, V> *p, T key)
	{
		if (p == NULL) {
			return NULL;
		}
		else if (p->key == key) {
			return p;
		}
		else if (p->key < key) {
			return (search(p->right, key));
		}
		else {
			return (search(p->left, key));
		}

	}

	template <class T, class V>
	bool map<T, V>::empty()
	{
		if (root == NULL) {
			return true;
		}
		else {
			return false;
		}
	}
	template <class T, class V>
	void map<T, V>::erase(iterator it1)
	{
		T key;
		key = it1.it->key;
		if (this->delete_node(&root, key)) {
			size1--;
		}

	}

	template <class T, class V>
	int map<T, V>::max(int a, int b)
	{
		if (a > b) {
			return a;
		}
		return b;
	}

	template <class T, class V>
	int map<T, V>::height(node<T, V> *s)
	{
		if (s != NULL) {
			return (s->height);
		}
		return 0;
	}

	template <typename T, class V>
	bool map<T, V>::insert_node(node<T, V> **p, T key, V value)
	{
		//cout << "in the insert function and key =  " << key << endl;
		int diff;
		if ((*p) == NULL) {
			(*p) = new node<T, V>();
			(*p)->key = key;
			(*p)->value = value;
			(*p)->left = NULL;
			(*p)->right = NULL;
			(*p)->height = 1; // since, its a leaf node
			return true;
		}
		else if ((*p)->key == key) {
			//	cout << "same key" << endl;
			return false;
		}
		else if ((*p)->key < key) {
			insert_node(&((*p)->right), key, value);
		}
		else if ((*p)->key > key) {
			insert_node(&((*p)->left), key, value);
		}
		(*p)->height = max(height((*p)->left), height((*p)->right)) + 1;
		diff = difference_of_heights((*p));
		//cout << "diff  = " << diff << "of " << (*p) -> key << endl;
		if (diff > 1 && (key < (*p)->left->key)) {
			//RATATE RIGHT
		//	cout << "rotate right case" << endl;
			(*p) = rotate_right(*p);
		}
		else if (diff < -1 && (key > (*p)->right->key)) {
			//LEFT ROTATE
		  //  	cout << "rotate left case" << endl;
			(*p) = rotate_left(*p);
		}
		else if (diff > 1 && (key > (*p)->left->key)) {
			//LEFT RIGHT ROTATE
	//	cout << "rotate left right case" << endl;
			(*p)->left = rotate_left((*p)->left);
			(*p) = rotate_right((*p));
		}
		else if (diff < -1 && (key < (*p)->right->key)) {
			//RIGHT LEFT
	//	cout << "rotate right left case\n";
			(*p)->right = rotate_right(*p);
			(*p) = rotate_left(*p);
		}
		return true;
	}

	template <typename T, class V>
	int map<T, V>::difference_of_heights(node<T, V> *n)
	{
		if (n != NULL) {
			return (height(n->left) - height(n->right));
		}
		return 0;
	}

	template <typename T, class V>
	node<T, V>* map<T, V>::rotate_right(node<T, V> *y)
	{
		//cout << "rotating right of " << y -> key << endl;
		node<T, V> *x = y->left;
		node<T, V> *z = x->right;

		y->left = z;
		x->right = y;

		y->height = max(height(y->left), height(y->right)) + 1;
		x->height = max(height(x->left), height(x->right)) + 1;

		return x;
	}

	template <typename T, class V>
	node<T, V>* map<T, V>::rotate_left(node<T, V> *x)
	{
		//cout << "rotating left " << x -> key << endl;
		node<T, V> *y = x->right;
		node<T, V> *z = y->left;

		y->left = x;
		x->right = z;

		y->height = max(height(y->left), height(y->right)) + 1;
		x->height = max(height(x->left), height(x->right)) + 1;

		return y;
	}

	template <typename T, class V>
	void map<T, V>::preorder(node<T, V> *p)
	{
		if (p != NULL) {
			cout << " " << p->key;
			preorder(p->left);
			preorder(p->right);
		}
	}

	template <class T, class V>
	bool map<T, V>::delete_node(node<T, V> **p, T key)
	{
		//cout << "deleting " << key << endl;
		if ((*p) == NULL) {
			//cout << "key mismatch" << endl;
			return false;
		}
		else if ((*p)->key < key) {
			//	cout << "moving rigt of " << (*p) -> key << endl;
			delete_node(&((*p)->right), key);
		}
		else if ((*p)->key > key) {
			//	cout << "moving left of " << (*p) -> key << endl;
			delete_node(&((*p)->left), key);
		}
		else {
			if ((*p)->right == NULL && (*p)->left == NULL) {
				//cout << "no node case\n";
				(*p) = NULL;
			}
			else if ((*p)->right == NULL && (*p)->left != NULL) {
//				cout << "one left child case" << endl;
				(*p) = (*p)->left;
			}
			else if ((*p)->left == NULL && (*p)->right != NULL) {
				//cout << "onoe right caswe\n";
				(*p) = (*p)->right;
			}
			else {
				//cout << "two child case" << endl;
				node<T, V> *q = (*p)->right;
				if (q->left == NULL) {
//					cout << "case of left = NULL\n";
					(*p)->key = q->key;
					(*p)->right = q->right;
				}
				else {
					//cout << "other case\n";
					while (q->left->left != NULL) {
						q = q->left;
					}
					(*p)->key = q->left->key;
					q->left = q->left->right;
				}
			}
		}
		if ((*p) != NULL) {
			(*p)->height = max(height((*p)->left), height((*p)->right)) + 1;
			int diff;
			diff = difference_of_heights((*p));
			if (diff > 1) {
				int l_balance;
				l_balance = difference_of_heights((*p)->left);
				if (l_balance >= 0) {
					(*p) = rotate_right((*p));
				}
				else {
					(*p)->left = rotate_left((*p)->left);
					(*p) = rotate_right((*p));
				}
			}
			else if (diff < -1) {
				int r_balance;
				r_balance = difference_of_heights((*p)->right);
				if (r_balance <= 0) {
					(*p) = rotate_left((*p));
				}
				else {
					(*p)->right = rotate_right((*p)->right);
					(*p) = rotate_left(*p);
				}
			}
		}
	}

	template <class T, class V>
	bool operator==(const map<T, V>&x, const map<T, V>& k)
	{
		if (x.size() != k.size()) {
			return false;
		}
		else {
			typename map<T, V>::iterator it, it1;
			for (it = x.begin(), it1 = k.begin(); it != x.end(); it++, it1++) {
				if ((*it).first != (*it1).first || (*it).second != (*it1).second) {
					return false;
				}
			}
			return true;
		}
	}


	template <class T, class V>
	bool operator!=(const map<T, V>& x, const map<T, V>& k)
	{
		return (!(x == k));
	}
};