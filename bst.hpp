#ifndef _bst_node
#define _bst_node

#include <iostream>
#include <memory>
#include <utility>
#include <iterator>
#include <vector>
#include "iterator.hpp"

/**
 * @file bst.hpp
 * @authors Buscaroli Elena, Valeriani Lucrezia
 * @brief Source code
 */

template < typename T >
struct node{
	std::unique_ptr<node> right;
	std::unique_ptr<node> left;
	node* parent;

	T value;

	node() {};

	explicit node(const T& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{x} { /*std::cout << " (l-value 2) ";*/ }
	node(const T& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{x} { /*std::cout << " (l-value 3) ";*/ }

	explicit node(T&& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{std::move(x)} { /*std::cout << " (r-value 2) ";*/ }
	node(T&& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{std::move(x)} { /*std::cout << " (r-value 3) ";*/ }

	explicit node(const std::unique_ptr<node>& x, node* p=nullptr): parent{p}, value{x->value}{
		if(x->right)
			right.reset(new node{x->right, this});

		if(x->left)
			left.reset(new node{x->left, this});
	}

	~node() noexcept = default;
};




template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
class bst{
	
	using pair_type = std::pair< const key_type, value_type >;
	using node_t = node< pair_type >;
	using iterator = _iterator< node_t , pair_type > ;
	using const_iterator = _iterator< node_t , const pair_type > ;

	std::unique_ptr<node_t> root;
	cmp_op op;

	// _find function
	node_t* _find(const key_type& x) {
		if(!root) { return nullptr; }
		node_t* tmp = root.get();
		while(tmp) {
			if(tmp->value.first == x) { return tmp; }
			else if( op(x, tmp->value.first) ) { tmp = tmp->left.get(); }  // if x smaller than tmp
			else if( op(tmp->value.first, x) ) { tmp = tmp->right.get(); }  // if x bigger than tmp
			else { return nullptr; }
		}
		return nullptr;
	}

	// _insert function
	template <typename O>
	std::pair<iterator, bool> _insert( O&& x){
		node_t* n{_find(x.first)};
		if (n) { return std::make_pair(iterator{n}, false); }

		auto tmp = root.get();
		while(tmp) {  //(op(tmp->value.first, x.first) || op(x.first,tmp->value.first)){ 
			if( op(tmp->value.first, x.first) ){ //my key > root
				if (tmp->right) { tmp = tmp->right.get(); }
				else {
					tmp->right = std::make_unique<node_t> (std::forward<O>(x), tmp); //tmp is the parent
					return std::make_pair(iterator{tmp->right.get()}, true); 
				}
			}
			if( op(x.first,tmp->value.first) ){ //my key < root 
				if(tmp->left){ tmp = tmp->left.get(); }
				else{
					tmp->left = std::make_unique<node_t> (std::forward<O>(x), tmp); //tmp is the parent
					return std::make_pair(iterator{tmp->left.get()}, true); 
				}
			}
		}
		root = std::make_unique<node_t> (std::forward<O>(x), nullptr); //qui il parent sarebbe nullptr
		return std::make_pair(iterator{root.get()}, true);
	}


	// _inorder function
	node_t* _inorder(node_t* x) {
		while(x->left) { x = x->left.get(); }
		return x;
	}

	// _balance recursive
	void _balance(std::vector<pair_type>& nodes, int start, int end){
		if (start < end) {
			int mid = (start + end)/2; 
			insert(nodes[mid]);
			_balance(nodes, start, mid);
			_balance(nodes, mid+1, end); 
		}
	}


	public:
		bst() noexcept = default;
		~bst() noexcept = default;

		// insert
		std::pair<iterator, bool> insert(const pair_type& x) { return _insert(x); }
		std::pair<iterator, bool> insert(pair_type&& x) { return _insert(std::move(x)); }
		
		// emplace
		template< class... Types >
		std::pair<iterator,bool> emplace(Types&&... args){ return insert(pair_type(std::forward<Types>(args)...)); }

		// clear: since the destructor are then called recursively 
		void clear() noexcept { root.reset(); }

		// begin: begin() returns an iterator to beginning while cbegin() returns a const_iterator to beginning
 		iterator begin() noexcept {
			if(!root) { return iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left) { tmp = tmp->left.get(); }
			return iterator{tmp};
		}

		const_iterator begin() const noexcept {
			if(!root) { return const_iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left) { tmp = tmp->left.get(); }
			return const_iterator{tmp};
		}

		const_iterator cbegin() const noexcept {
			if(!root) { return const_iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left) { tmp = tmp->left.get(); }
			return const_iterator{tmp};
		}

		// end
		iterator end() noexcept { return iterator{nullptr}; }
		const_iterator end() const noexcept { return const_iterator{nullptr}; }
		const_iterator cend() const noexcept { return const_iterator{nullptr}; }

		// find
		iterator find(const key_type& x) { return iterator{_find(x)}; };
		const_iterator find(const key_type& x) const { return const_iterator{_find(x)}; };

		// copy ctor
		bst(const bst& x) : op{x.op} {
			if (x.root) { root.reset(new node_t{x.root}); }
		};

		// copy assignment
		bst& operator=(const bst& x){
			root.reset();
			auto tmp = x; //copyctor
			*this = std::move(tmp); //move ass
			return *this;
		}

		// move ctor
		bst(bst&&) noexcept = default;

		// move assignment 
		bst& operator=(bst&&) noexcept = default;

		// balance
		void balance(){
			std::vector<pair_type> nodes;
			for (const auto& i : *this) { nodes.push_back(i); }
			clear();
			return(_balance(nodes, 0, nodes.size()));
		}

		// subscripting operator
		value_type& operator[](const key_type& x) {
			value_type v;
			auto i = insert(std::pair<key_type,value_type>(x, v)); // iterator pointing to node + bool 
			return (*i.first).second;
		 }

		value_type& operator[](key_type&& x) {
			value_type v;
			auto i = insert(std::pair<key_type,value_type>(x, v)); // iterator pointing to node + bool 
			return (*i.first).second;
		 }

		// put-to 
		friend
		std::ostream& operator<<(std::ostream& os, const bst& x) noexcept {
			for(const auto& i: x) {
				os << i.first << " ";
			}
			os << std::endl;
			return os;
		}

		// erase
		void erase(const key_type& x) {
			node_t* n{_find(x)};  // if found, returns an iterator to the node
			if(!n) { return; }  // if the key is not found find returns nullptr

			if( !(n->left) && !(n->right) ) {  // if leaf node
				// std::cout << "LEAF NODE" << std::endl;
				if( !(n == root.get()) ) {
					if( n == n->parent->left.get() ) { n->parent->left.reset(); }
					else { n->parent->right.reset(); }
				}
				else { root.reset(); }
			}

			else if( n->left && n->right ) {
				// std::cout << "TWO CHILDREN NODES" << std::endl;
				node_t* in = _inorder(n->right.get());
				auto v = in->value;
				bool left = false;
				erase(v.first);

				if( n == root.get() ) {
					n = new node_t{v, n->right.release(), n->left.release(), n->parent};
					root.reset(n);
					if(n->left) { n->left->parent = n; }
					if(n->right) { n->right->parent = n; }
				}
				else {
					if( n->parent->left.get() == n ) { left = true; }
					n = new node_t{v, n->right.release(), n->left.release(), n->parent};
					if(n->left) { n->left->parent = n; }
					if(n->right) { n->right->parent = n; }

					if( left ) { n->parent->left.reset(n); }
					else { n->parent->right.reset(n); }
				}
			}

			else {
				// std::cout << "ONLY ONE CHILD NODE" << std::endl;
				if( n == root.get() ) {
					if(n->right) {
						n->right->parent = nullptr;
						root.reset(n->right.release());
					}
					else {
						n->left->parent = nullptr;
						root.reset(n->left.release());
					}
				}
				else {
					if( n->parent->left.get() == n ) {
						if( n->left ) { 
							n->left->parent = n->parent;
							n->parent->left.reset(n->left.release()); 
						}
						else { 
							n->right->parent = n->parent;
							n->parent->left.reset(n->right.release());
						}
					}
					else {
						if( n->left ) { 
							n->left->parent = n->parent;
							n->parent->right.reset(n->left.release()); 
						}
						else { 
							n->right->parent = n->parent;
							n->parent->right.reset(n->right.release()); 
						}
					}
				}
			}
		}
}; // end bst class

#endif