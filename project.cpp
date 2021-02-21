#include <iostream>
#include <memory>  
#include <utility>
#include <iterator>
#include <vector>

//g++ -std=c++14 project.cpp

template < typename T >
struct node{
	std::unique_ptr<node> right;
	std::unique_ptr<node> left;
	node* parent;

	T value;

	node() {std::cout << "creation \n";};

	// x è la mia pair
	explicit node(const T& x): right{nullptr}, left{nullptr}, value{x} {std::cout << "l-value 1\n";}
	node(const T& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{x} {std::cout << "l-value 2\n";}
	node(const T& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{x} {std::cout << "l-value 2\n";}
	// explicit node(const T& x): value{x} {std::cout << "l-value 1\n";}
	// node(const T& x, node* p, std::unique_ptr<node>& l=nullptr, std::unique_ptr<node>& r=nullptr): parent{p}, value{x}, right{r}, left{l} {std::cout << "l-value 2\n";}

	explicit node(T&& x): right{nullptr}, left{nullptr}, value{std::move(x)} {std::cout << "r-value 1\n";}
	node(T&& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{std::move(x)} {std::cout << "r-value 2\n";}
	node(T&& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{std::move(x)} {std::cout << "r-value 2\n";}
	// explicit node(T&& x): value{std::move(x)} {std::cout << "r-value 1\n";}
	// node(T&& x, node* p, std::unique_ptr<node>& l=nullptr, std::unique_ptr<node>& r=nullptr): parent{p},  value{std::move(x)}, right{r}, left{l} {std::cout << "r-value 2\n";}

	// explicit node(const std::unique_ptr<node>& x, node* p=nullptr): value{x->value}, parent{p}{ //this is used for the copy constructor of the bst 
	explicit node(const std::unique_ptr<node>& x, node* p=nullptr): parent{p}, value{x->value}{ //this is used for the copy constructor of the bst 
		if(x->right)
			right.reset(new node{x->right, this});

		if(x->left)
			left.reset(new node{x->left, this});
	}

	~node() = default;
};


// template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
// template < typename T >
// template < typename O >
// class bst< value_type, key_type, cmp_op >::_iterator {
//	 using pair_type = std::pair< const key_type, value_type >;
//	 using node = typename node< T >;
template < typename node, typename T >
class _iterator {
	node* current;

	public:
		using v_type = T;
		using reference = v_type&;
		using pointer = v_type*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		explicit _iterator(node* p) : current{p} {}
		~_iterator() = default;

		//dereference operator 
		reference operator*() const { return current->value; }
		pointer operator->() const { return &**this; }

		// boolean operator 
		friend 
		bool operator==(_iterator &a, _iterator &b) { return a.current == b.current; }
		friend 
		bool operator!=(_iterator &a, _iterator &b) { return !(a == b); }

		//pre-increment operator
		_iterator &operator++() {
			if(current->right) { //if !=nullptr 
				current = current->right.get();
				while(current->left){
					current = current->left.get();
				}
				return *this;
			}
			else if (current->parent){
				node* tmp = current;
				current = current->parent;
				while(current && tmp == current->right.get()) {
					tmp = current;
					current = current->parent;
				}
				return *this;
			}
			else{
				return *this;
			}
		}
};



template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
class bst{
	
	using pair_type = std::pair< const key_type, value_type >;
	using node_t = node< pair_type >;
	using iterator = _iterator< node_t , pair_type > ;
	using const_iterator = _iterator< node_t , const pair_type > ;

	std::unique_ptr<node_t> root;
	cmp_op op;


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

	template <typename O>
	std::pair<iterator, bool> _insert( O&& x){
		// auto n = find(x.first);
		// if (n){
		// 	 return std::make_pair(n, false);
		// }
		auto tmp=root.get();
		while(tmp) {  //(op(tmp->value.first, x.first) || op(x.first,tmp->value.first)){ //scegliere altra guardia facile tipo tmp 
			if (op(tmp->value.first, x.first)){ //my key > root
				if (tmp->right){
					tmp = tmp->right.get();
				}
				else {
					tmp->right = std::make_unique<node_t>(std::forward<O>(x),tmp); //tmp is the parent
					std::cout<<"make node right \n";
					return std::make_pair(iterator{tmp->right.get()}, true); 
				}
			}
			if (op(x.first,tmp->value.first)){ //my key < root 
				if (tmp->left){
					tmp = tmp->left.get();
				}
				else{
					tmp->left = std::make_unique<node_t>(std::forward<O>(x),tmp); //tmp is the parent
					std::cout<<"make node left \n";
					return std::make_pair(iterator{tmp->left.get()}, true); 
				}
			}
		}
		root = std::make_unique<node_t>(std::forward<O>(x),nullptr); //qui il parent sarebbe nullptr
		std::cout<<"make root \n";
		return std::make_pair(iterator{root.get()}, true);
	}

	node_t* _inorder(node_t* x) {
		while(x->left) { x = x->left.get(); }
		return x;
	}

		//_balance recursive
		void _balance(std::vector<pair_type>& nodes, int start, int end){
			if (start < end) {
			 int mid = (start + end)/2; 
			 insert(nodes[mid]);
			_balance(nodes, start, mid);                                                                                                                                     
    		_balance(nodes, mid+1, end); 
			}  
		}


		// void _erase(node_t* r, const key_type& x) {
		// 	// node_t* par = nullptr;
		// 	// node_t* curr = r;
		// 	node_t* n{_find(x)};  // if found, returns an iterator to the node
		// 	if(!n) { return; }  // if the key is not found find returns nullptr
			
		// 	if( !(n->left) && !(n->right) ) {  // if leaf node
		// 		if( !(n == r) ) {
		// 			if( n == n->parent->left.get() ) { n->parent->left.reset(nullptr); }
		// 			if( n == n->parent->right.get() ) { n->parent->right.reset(nullptr); }
		// 		}
		// 		else { r = nullptr; }
		// 	}

		// 	else if( r->left && r->right ) {
		// 		node_t* in = _inorder(n->right.get());
		// 		auto v = in->value.first;
		// 		_erase(r, in->value.first);
		// 		n = new node_t{v};
		// 	}

		// 	else {
		// 		if( n->parent->left.get() == n ) { 
		// 			node_t* child = new node_t{n->left}; 
		// 			if( n == r ) { r = std::make_unique<node_t> (child); }
		// 			else { n->parent->left = std::make_unique<node_t> (child); }
		// 		}
		// 		if( n->parent->right.get() == n ) { 
		// 			node_t* child = new node_t{n->right}; 
		// 			if( n == r ) { r = std::make_unique<node_t> (child); }
		// 			else { n->parent->right = std::make_unique<node_t> (child); }
		// 		}
		// 	free(n);
		// 	}
		// }

	public:
		bst() noexcept=default;
		~bst() noexcept=default;
		// insert
		// The function returns a pair of an iterator (pointing to the node) and a bool. 
		// The bool is true if a new node has been allocated, 
		template <typename O> //forwarding reference for using both r and l value 
		std::pair<iterator, bool> insert( O&& x){ return _insert(x);};
		
		// std::pair<iterator, bool> insert(pair_type&& x);
		// emplace
		template< class... Types >
		std::pair<iterator,bool> emplace(Types&&... args);

		// clear
		// since the destructor are then called recursively 
		void clear(){
			root.reset();
		}

		// begin
		// begin() returns an iterator to beginning while cbegin() returns a const_iterator to beginning
 		iterator begin()  {
			if(!root)
				return iterator{nullptr};
			node_t* tmp = root.get();
			while(tmp->left.get()) 
				tmp = tmp->left.get();
			return iterator{tmp};
		}

		const_iterator begin() const {
			if(!root)
				return const_iterator{nullptr};
			node_t* tmp = root.get();
			while(tmp->left.get()) 
				tmp = tmp->left.get();
			return const_iterator{tmp};
		}

		const_iterator cbegin() const  {
			if(!root)
				return const_iterator{nullptr};
			node_t* tmp = root.get();
			while(tmp->left.get()) 
				tmp = tmp->left.get();
			return const_iterator{tmp};
		}

		// end
		iterator end()  { return iterator{nullptr}; }
		const_iterator end() const  { return const_iterator{nullptr}; }
		const_iterator cend() const  { return const_iterator{nullptr}; }

		// find
		iterator find(const key_type& x) {
			return iterator{_find(x)};
		};
		const_iterator find(const key_type& x) const {
			return const_iterator{_find(x)};
		};

		//copy semantics --> deep copy
		//copy ctor
		bst(const bst& x) : op{x.op} {
			if (x.root){
				root.reset(new node_t{x.root});
			}
		};

		//copy assignment
		bst& operator=(const bst& x){
			root.reset();
			auto tmp = x; //copyctor
			*this = std::move(tmp); //move ass
			return *this;
		}

		//move semantics 
		//move ctor
		bst(bst&&)=default;
		//move assignment 
		bst& operator=(bst&&)=default;

		// balance
		void balance(){
			// create ordered vector
			std::vector<pair_type> nodes;
			for (const auto& i : *this){
					nodes.push_back(i);
				}
			// for (unsigned long  i=0; i<nodes.size();i++){
			// 	std::cout<<nodes[i].first<<nodes[i].second<<'\n';
			// }
			//clear the old tree
			clear(); 
			// construct the new balanced tree
			return(_balance(nodes,0,nodes.size()));
		} 

		// subscripting operator
		value_type& operator[](const key_type& x){
			value_type v{};
			auto i = insert(std::pair<key_type,value_type>(x,v)); // iterator pointing to node + bool 
			return (*i.first).second;
		 }

		value_type& operator[](key_type&& x){
			value_type v{};
			auto i = insert(std::pair<key_type,value_type>(x,v)); // iterator pointing to node + bool 
			return (*i.first).second;
		 }

		// put-to 
		friend
		std::ostream& operator<<(std::ostream& os, const bst& x){
			for(const auto& i: x) { // se l'iterator funziona 
				os << i.first << " ";
			}
			os << std::endl;
			return os;
		}

		// node_t* _inorder(node_t* x) {
		// 	while(x->left) { x = x->left.get(); }
		// 	return x;
		// }

		// erase
		
		void _erase(node_t* r, const key_type& x) {
			// node_t* par = nullptr;
			// node_t* curr = r;
			node_t* n{_find(x)};  // if found, returns an iterator to the node
			if(!n) { return; }  // if the key is not found find returns nullptr

			std::cout << "prova" << std::endl;

			if( !(n->left) && !(n->right) ) {  // if leaf node
				std::cout << "LEAF NODE" << std::endl;

				if( !(n == r) ) {
					if( n == n->parent->left.get() ) { n->parent->left.reset(nullptr); }
					if( n == n->parent->right.get() ) { n->parent->right.reset(nullptr); }
				}
				else { 
					r = nullptr; 
				}
				// free(n);
			}

			else if( n->left && n->right ) {
				std::cout << "TWO CHILDREN NODES" << std::endl;
				node_t* in = _inorder(n->right.get());
				auto v = in->value.first;
				_erase(r, in->value.first);
				n = new node_t{in->value, n->right.get(), n->left.get(), n->parent};
			}

			else {  // only one child
				std::cout << "prova1" << std::endl;
				
				if( n==r ) {
					// r = nullptr;
					std::cout << "prova" << std::endl;
					n->right->parent = nullptr;
					r = new node_t{n->right};
					std::cout << r->value.first << std::endl;
					// std::cout << n->right->value.first << std::endl;
					// std::cout << n->right->parent->value.first << std::endl;
				}

				else {
					if( n->parent->left.get() == n ) {
						n->parent->left.release();

						if( n->left ) { 
							n->left->parent = n->parent;
							n->parent->left.reset(n->left.release()); 
						}
						else { 
							n->right->parent = n->parent;
							n->parent->left.reset(n->right.release());
						}
					}

					else {  // if ( n->parent->right.get() == n ) {
						std::cout << "prova" << std::endl;
						n->parent->right.release();

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
				// free(n);
			}
		}

		void erase(const key_type& x) {
			_erase(root.get(), x);
			return;
		}

		// void erase(const key_type& x) {
		// 	if(!root) { return; }  // if there is no a root
		// 	node_t* n{_find(x)};  // if found, returns an iterator to the node
		// 	if(!n) { return; }  // if the key is not found find returns nullptr
		// 	if( !(n->left) && !(n->right) ) {  // if leaf node
		// 		if( !(n == root.get()) ) {
		// 			if( n == n->parent->left.get() ) { n->parent->left.reset(nullptr); }
		// 			if( n == n->parent->right.get() ) { n->parent->right.reset(nullptr); }
		// 		}
		// 		else { root.reset(nullptr); }
		// 	}
		// 	else if( n->left && n->right ) {  // if two children nodes
		// 		node_t* in = _inorder(n->right.get());
		// 		auto v = in->value;
		// 		std::cout << "n " << n->value.first << " - in " << in->value.first << std::endl;
		// 		std::cout << "n->right " << n->right->value.first << " - n->left " << n->left->value.first << '\n' << std::endl;
		// 		if( !(in==root.get()) && in == in->parent->left.get() ) { 
		// 			if( in->right ) { in->parent->left = std::make_unique<node_t> (in->right); }
		// 			else if( in->left ) { in->parent->left = std::make_unique<node_t> (in->left); }
		// 		}
		// 		else if( !(in==root.get()) && in == in->parent->right.get() ) {
		// 			if( in->right ) { in->parent->right = std::make_unique<node_t> (in->right); }
		// 			else if( in->left ) { in->parent->right = std::make_unique<node_t> (in->left); }
		// 		}
		// 		std::cout << "n->right " << n->right->value.first << " - n->left " << n->left->value.first << '\n' << std::endl;
		// 		if( !(n->right.get() == in) && !(n->left.get() == in) ) { n = new node_t{in->value, n->right.get(), n->left.get(), n->parent}; }
		// 		else if( n->right.get() == in ) {
		// 			n->parent->right = std::make_unique<node_t> (n->right);
		// 			n = new node_t{in->value, nullptr, n->left.get(), n->parent}; 
		// 		}
		// 		else if( n->left.get() == in ) { 
		// 			n->parent->left = std::make_unique<node_t> (n->left);
		// 			n = new node_t{in->value, n->right.get(), nullptr, n->parent};
		// 		}
		// 	}
		// 	// n has only one child node
		// 	else if( ((n->right) && !(n->left)) || (!(n->right) && (n->left)) ) {
		// 		node_t* child;
		// 		if( !(n == root.get()) ) {
		// 			if( n == n->parent->left.get() ) {  // if n is a left node
		// 				if(n->left) {
		// 					child = new node_t{n->left->value, nullptr, nullptr, n->parent};
		// 					n->parent->left.reset(child);
		// 					// n->parent->left = child;
		// 					// n->parent->left = std::make_unique<node_t> (n->left);
		// 					// n->left->parent = n->parent;
		// 					// n->left.release();
		// 				}
		// 				else {  // if n->right exists
		// 					child = new node_t{n->right->value, nullptr, nullptr, n->parent};
		// 					n->parent->right.reset(child);
		// 					// n->parent->left = std::make_unique<node_t> (n->right);
		// 					// n->right->parent = n->parent;
		// 					// n->parent = nullptr;
		// 					// n->right.release();
		// 				}
		// 			}
		// 			if( n == n->parent->right.get() ) {  // if n is a right node
		// 				if(n->left) {  // if n->left exists
		// 					n->parent->right = std::make_unique<node_t> (n->left);
		// 					n->left.release();
		// 				}
		// 				else {  // if n->right exists
		// 					n->parent->right = std::make_unique<node_t> (n->right);
		// 					n->right.release();
		// 				}
		// 				return;
		// 			}
		// 		}
		// 		else {
		// 			root.release();
		// 			if( n->right ) { root.reset(new node_t{n->right}); }
		// 			if( n->left ) { root.reset(new node_t{n->left}); }
		// 			return;
		// 		}
		// 	}
		// }
}; // end bst class



// template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
// node* bst< value_type, key_type, cmp_op >::inorder(const node* x) {
// 	while(x->left){
// 		x = x->left:
// 	}
// 	return x;
// }

// template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
// void bst< value_type, key_type, cmp_op >::erase(const key_type& x) {
// 	if(!root) { return; }  // if there is no a root

// 	node_t* n{_find(x)};  // if found, returns an iterator to the node

// 	if(!n) { return; }  // if the key is not found find returns nullptr

// 	// n is a leaf -> reset the node to nullptr
// 	if( !(n->right) && !(n->left) ) {
// 		if( n == n->parent->left.get() ) { n->parent->left.reset(nullptr); }
// 		if( n == n->parent->right.get() ) { n->parent->right.reset(nullptr); }
// 		n.reset(nullptr); 
// 	}

// 	// n is the root -> not necessary?
// 	if(n == root.get()) {
// 		root.relase();  // root releases the ownership
// 		if( !(n->right->left) && (n->right->right) ) {  // if right node does not have a left node it becomes the root
// 			n->right->left.reset(n->left);
// 			n->right->parent = nullptr;
// 		}
// 		else if( !(n->left->right) && (n->left->left) ) {  // if left node does not have a right node it becomes the root
// 			n->left->right.reset(n->right);
// 			n->left->parent = nullptr;
// 		}
// 	}

// 	// if n has two children nodes
// 	if( n->left && n->right ) {  // if both right and left have two children nodes
// 		auto in = inorder(n->right);  // to find the minimum element in the right subtree of n
// 		n->value = in->value;  // copy the value of the inorder element in n
// 		in->parent->left.release();
// 		in.reset(nullptr);  // remove the "in" node
// 	}

// 	// n has only one child node
// 	if( ((n->right) && !(n->left)) || (!(n->right) && (n->left)) ) {
// 		if( n == n->parent->left.get() ) {  // if n is a left node
// 			if(n->left) {  // if n->left exists
// 				n->parent->left.reset(n->left);
// 				n->left.release();
// 			}
// 			if(n->right) {  // if n->right exists
// 				n->parent->left.reset(n->right);
// 				n->right.release();
// 			}
// 		}

// 		if( n == n->parent->right.get() ) {  // if n is a right node
// 			if(n->left) {  // if n->left exists
// 				n->parent->right.reset(n->left);
// 				n->left.release();
// 			}
// 			if(n->right) {  // if n->right exists
// 				n->parent->right.reset(n->right);
// 				n->right.release();
// 			}
// 		}
// 	}
// }




// // Find a given key. If the key is present, returns an iterator to the proper node, end() otherwise.
// // the tree is ordered by keys
// template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
// template < typename O >
// O bst< value_type, key_type, cmp_op >::find(const key_type& x) {
// 	cmp_op op;

// 	if(!root)
// 		return O{nullptr};

// 	if(root->value.first == x) { return O{root}}
// 	else if(op(root->value.first, x)) {  // True if the first element is smaller than the second one
// 		auto tmp{*this};
// 		while(op(tmp->value.first, x)) {  // until all the elements are lower than x
// 			++tmp;
// 		}

// 		if(tmp->value.first == x) { return O{tmp}; }
// 		else { return end(); }
// 	}

// 	else {
// 		O tmp = begin();  // in this case tmp is an iterator or const_iterator
// 		while(op(x, tmp->value.first)) {  // until all the elements are lower than k
// 			++tmp;
// 		}

// 		if(tmp->value.first == x) { return tmp; }
// 		else { return end(); }
// 	}
// }




int main(){

	// using type = std::pair<int, int>;

	// const std::pair<int, int> p(1, 10);
	// // node<type> n{}; //creation
	// node<type> n1{(std::pair<int, int>(1, 10))}; //rvalue 1
	// node<type> n2{p}; //lvalue 1 

	// std::pair<int, int> p(1, 10);

	// node<std::pair<int, int>> n1{};
	// node<std::pair<int, int>> n2{p};
	// node<std::pair<int, int>> n3{std::pair<int, int> {10,12}};

	using type = std::pair<int, int>;
	std::pair<int, int> p{1, 10};
	using node_t = node<type>;

	bst<int,int> tree{};
	// tree.insert(p);
	// tree.insert(std::pair<int, int> {2,12});
	// tree.insert(std::pair<int, int> {6,12});
	// tree.insert(std::pair<int, int> {3,12});
	// std::cout << tree.root << std::endl;
	// tree.find(6);

	// bst<int, int> t1{tree};
	// std::cout << tree.root->value.first << std::endl;

	// t1.insert(std::pair<int, int> {5,12});
	// t1.insert(std::pair<int, int> {7,12});
	// tree.insert(std::pair<int, int> {10,12});
	// std::cout << t1 << std::endl;

	// t1.erase(3);
	// std::cout << t1 << std::endl;
	
	// auto c = t1._find(3);
	// std::cout << c->value.first << '\n';

	tree.insert(std::pair<int, int> {8,12}); //root
	tree.insert(std::pair<int, int> {3,12}); //left
	tree.insert(std::pair<int, int> {10,12}); //right
	tree.insert(std::pair<int, int> {6,12}); //right 
	tree.insert(std::pair<int, int> {1,12}); //left
	tree.insert(std::pair<int, int> {14,12}); //right
	tree.insert(std::pair<int, int> {13,12}); //left
	tree.insert(std::pair<int, int> {4,12}); //left
	tree.insert(std::pair<int, int> {7,12}); //right
	tree.insert(std::pair<int, int> {5,12}); //right

	std::cout << tree << std::endl;
	tree.erase(4);
	std::cout << tree << std::endl;

	return 0;
}

