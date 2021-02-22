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
			// if( !(current->parent) ) { return *this; }

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
			node_t* n{_find(x.first)};
			if (n){ //if n != nullptr 
				std::cout<<"already there \n";
					return std::make_pair(iterator{n}, false);
			}
		auto tmp = root.get();
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
		root = std::make_unique<node_t> (std::forward<O>(x), nullptr); //qui il parent sarebbe nullptr
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
		//insert
		//template <typename O> //forwarding reference for using both r and l value 
		std::pair<iterator, bool> insert(const pair_type& x){
			return _insert(x);
		}

		std::pair<iterator, bool> insert(pair_type&& x){
			return _insert(std::move(x));
		}
		
		// emplace
		template< class... Types >
		std::pair<iterator,bool> emplace(Types&&... args){
		return insert(pair_type(std::forward<Types>(args)...));}

		// clear: since the destructor are then called recursively 
		void clear(){
			root.reset();
		}

		// begin: begin() returns an iterator to beginning while cbegin() returns a const_iterator to beginning
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
		
		void _erase(const key_type& x) {

			node_t* n{_find(x)};  // if found, returns an iterator to the node
			if(!n) { return; }  // if the key is not found find returns nullptr

			std::cout << "prova" << std::endl;

			if( !(n->left) && !(n->right) ) {  // if leaf node
				std::cout << "LEAF NODE" << std::endl;

				if( !(n == root.get()) ) {
					if( n == n->parent->left.get() ) { 
						n->parent->left.reset(nullptr);
						n->parent = nullptr;
					}
					if( n == n->parent->right.get() ) { 
						n->parent->right.reset(nullptr); 
						n->parent = nullptr;
					}
				}
				else { 
					// r = nullptr; 
					root.reset(nullptr);
				}
			}

			else if( n->left && n->right ) {
				std::cout << "\n\nTWO CHILDREN NODES" << std::endl;
				node_t* in = _inorder(n->right.get());
				auto v = in->value;
				bool left = false;

				std::cout << "in " << in->value.first << std::endl;
				std::cout << "roor " << root->value.first << '\n' << std::endl;

				_erase(in->value.first);

				if( n == root.get() ) {
					root.release();
					n = new node_t{v, n->right.get(), n->left.get(), n->parent};
					root.reset(n);
					if(n->left) { n->left->parent = n; }
					if(n->right) { n->right->parent = n; }
				}

				else {
					if( n->parent->left.get() == n ) { left = true; n->parent->left.release(); }
					else { n->parent->right.release(); }

					n = new node_t{v, n->right.get(), n->left.get(), n->parent};

					if(n->left) { n->left->parent = n; }
					if(n->right) { n->right->parent = n; }

					if( left ) { n->parent->left.reset(n); }
					else { n->parent->right.reset(n); }
				}
			}

			else {  // only one child
				std::cout << "ONLY ONE CHILD NODE" << std::endl;
				if( n == root.get() ) {
					std::cout << "Root node" << std::endl;
					if(n->right) {
						n->right->parent = nullptr;
						auto m = n->right.release();
						root.reset(m);
					}
					else {
						n->right->parent = nullptr;
						auto m = n->left.release();
						root.reset(m);
					}
				}

				else {
					if( n->parent->left.get() == n ) {
						std::cout << "left node" << std::endl;
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

					else {  // if ( n->parent->right.get() == n )
						std::cout << "right node" << std::endl;
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
			}
		}

		void erase(const key_type& x) {
			_erase(x);
			return;
		}

}; // end bst class




int main(){

	using type = std::pair<int, int>;
	std::pair<int, int> p{1, 10};
	using node_t = node<type>;


// EXAMPLE 1
	// bst<int,int> tree{};
	// tree.insert(std::pair<int, int> {1,12});
	// tree.insert(std::pair<int, int> {2,12});

	// std::cout << tree << std::endl;

	// tree.erase(1);
	// std::cout << tree << std::endl;


	// tree.insert(std::pair<int, int> {2,12});
	// tree.insert(std::pair<int, int> {6,12});
	// tree.insert(std::pair<int, int> {3,12});
	// tree.find(6);

	// bst<int, int> t1{tree};

	// t1.insert(std::pair<int, int> {5,12});
	// t1.insert(std::pair<int, int> {7,12});
	// tree.insert(std::pair<int, int> {10,12});
	// std::cout << t1 << std::endl;

	// t1.erase(1);
	// std::cout << t1 << std::endl;

// EXAMPLE 2
	bst<int,int> tree{};
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
	tree.erase(5);
	std::cout << tree << std::endl;

// TEST
	//test emplace 
	// tree.emplace(1,2);
	// int l{9};
	// tree.emplace(l,2);
	// std::cout<<tree<<'\n';

	//insert
	//std::cout<<tree.insert(std::pair<int, int> {3,12}).second<<'\n'; // 1 inserito
	//std::cout<<tree.insert(std::pair<int, int> {3,12}).second<<'\n'; // 0 already there
	// std::cout<<&(*tree.insert(p).first)<<'\n';
	// std::cout<<&(*tree.insert(std::pair<int, int> {2,12}).first)<<'\n';
	// std::cout<<&(*tree.insert(std::pair<int, int> {3,12}).first)<<'\n';

	// //find
	// std::cout<<&(*tree.find(3))<<'\n';

	//subscribing 
	// std::cout<<tree[3]<<'\n'; // return 12 
	// std::cout<<tree[5]<<'\n'; //insertion return 0

	//copy
	// bst<int,int> t1 = tree; //copy assignment
	// bst<int,int> t2{tree}; //copy ctor
	
	// t1.insert(std::pair<int, int> {5,12});
	// t2.insert(std::pair<int, int> {6,12});
	// tree.insert(std::pair<int, int> {7,12});

	// std::cout<<"tree \n"<< tree<<'\n';
	// std::cout<<"t1 \n"<< t1<<'\n';
	// std::cout<<"t2 \n"<< t2<<'\n';


	//begin and end 
	//std::cout<<&(*tree.begin())<<'\n';
	//std::cout<<&(*tree.end())<<'\n'; //???

	//move????
	// bst<int,int> t{};
	// t.insert(std::pair<int, int> {8,11}); //root
    // t.insert(std::pair<int, int> {3,12}); //left

	// bst<int,int> t1{};
	// bst<int,int> t2{std::move(t)}; //move ctor?
	// t1 = std::move(t); //move assignment?

	return 0;
}

