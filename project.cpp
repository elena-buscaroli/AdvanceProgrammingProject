#include <iostream>
#include <memory>
#include <utility>
#include <iterator>
#include <vector>


template < typename T >
struct node{
	std::unique_ptr<node> right;
	std::unique_ptr<node> left;
	node* parent;

	T value;

	node() {};

	explicit node(const T& x): right{nullptr}, left{nullptr}, value{x} {std::cout << " (l-value 1) ";}
	node(const T& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{x} {std::cout << " (l-value 2) ";}
	node(const T& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{x} {std::cout << " (l-value 3) ";}

	explicit node(T&& x): right{nullptr}, left{nullptr}, value{std::move(x)} {std::cout << " (r-value 1) ";}
	node(T&& x, node* p=nullptr): right{nullptr}, left{nullptr}, parent{p}, value{std::move(x)} {std::cout << " (r-value 2) ";}
	node(T&& x, node* r, node* l, node* p=nullptr): right{r}, left{l}, parent{p}, value{std::move(x)} {std::cout << " (r-value 3) ";}

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
			if(current->right) { //if !=nullptr 
				current = current->right.get();
				while(current->left){
					current = current->left.get();
				}
				return *this;
			}
			else if (current->parent) {
				node* tmp = current;
				current = current->parent;
				while(current && tmp == current->right.get()) {
					tmp = current;
					current = current->parent;
				}
				return *this;
			}
			current = nullptr;
			return *this;
		}
};



template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
class bst{
	
	using pair_type = std::pair< const key_type, value_type >;
	using node_t = node< pair_type >;
	using iterator = _iterator< node_t , pair_type > ;
	using const_iterator = _iterator< node_t , const pair_type > ;

	// public:
	std::unique_ptr<node_t> root;
	cmp_op op;

	// private:

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
		void clear(){ root.reset(); }

		// begin: begin() returns an iterator to beginning while cbegin() returns a const_iterator to beginning
 		iterator begin() {
			if(!root) { return iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left.get()) { tmp = tmp->left.get(); }
			return iterator{tmp};
		}

		const_iterator begin() const {
			if(!root) { return const_iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left.get()) { tmp = tmp->left.get(); }
			return const_iterator{tmp};
		}

		const_iterator cbegin() const {
			if(!root) { return const_iterator{nullptr}; }
			node_t* tmp = root.get();
			while(tmp->left.get()) { tmp = tmp->left.get(); }
			return const_iterator{tmp};
		}

		// end
		iterator end() { return iterator{nullptr}; }
		const_iterator end() const { return const_iterator{nullptr}; }
		const_iterator cend() const { return const_iterator{nullptr}; }

		// find
		iterator find(const key_type& x) { return iterator{_find(x)}; };
		const_iterator find(const key_type& x) const { return const_iterator{_find(x)}; };

		//copy semantics --> deep copy
		//copy ctor
		bst(const bst& x) : op{x.op} {
			if (x.root) { root.reset(new node_t{x.root}); }
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
		bst(bst&&) = default;

		//move assignment 
		bst& operator=(bst&&) = default;

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
		std::ostream& operator<<(std::ostream& os, const bst& x){
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
				std::cout << "LEAF NODE" << std::endl;

				if( !(n == root.get()) ) {
					if( n == n->parent->left.get() ) {
						n->parent->left.reset(nullptr);
						n->parent = nullptr;
					}
					else {
						n->parent->right.reset(nullptr); 
						n->parent = nullptr;
					}
				}
				else { root.reset(nullptr); }
			}

			else if( n->left && n->right ) {
				std::cout << "TWO CHILDREN NODES" << std::endl;
				node_t* in = _inorder(n->right.get());
				auto v = in->value;
				bool left = false;
				erase(in->value.first);
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

}; // end bst class




int main(){

// EXAMPLE 1
	// bst<int,int> tree{};
	// std::cout << "First pair added -> " << &(*tree.insert(std::pair<int, int> {1,12}).first) << std::endl;
	// std::cout << "Second pair added -> " << &(*tree.insert(std::pair<int, int> {2,12}).first) << std::endl;
	// tree.insert(std::pair<int, int> {6,12});
	// tree.insert(std::pair<int, int> {3,12});

	// std::cout << "\nTree -> " << tree << std::endl;
	// std::cout << "End -> " << &(*tree.end()) << '\t';
	// std::cout << "Begin -> " << &(*tree.begin()) << std::endl;

	// tree.erase(1);
	// std::cout << "\nTree after erase -> " << tree << std::endl;
	// std::cout << "Begin after erase ->" << &(*tree.begin()) << '\t';
	// std::cout << "End after erase ->" << &(*tree.end()) << std::endl;

	// tree.find(6);

	// bst<int, int> t1{tree};
	// t1.insert(std::pair<int, int> {5,12});
	// t1.insert(std::pair<int, int> {7,12});
	// tree.insert(std::pair<int, int> {10,12});
	// std::cout << t1 << std::endl;

	// t1.erase(1);
	// std::cout << t1 << std::endl;


// TEST

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
	std::cout << "\nTree -> " << tree << std::endl;


	// EMPLACE
	std::cout << "\nTree  before emplace() -> " << tree << std::endl;
	tree.emplace(2, 2);
	std::cout << "Tree after emplace(2, 2) -> " << tree << std::endl;
	std::cout << "Searching for key 2 -> (" << (*tree.find(2)).first << ", " << (*tree.find(2)).second << ")" << std::endl;

	int l{9};
	tree.emplace(l, 2);
	std::cout << "Tree after emplace(9, 2) -> " << tree << std::endl;
	std::cout << "Searching for key 9 -> (" << (*tree.find(9)).first << ", " << (*tree.find(9)).second << ")" << std::endl;

	tree.emplace(std::make_pair(15,7));
	std::cout << "Tree after emplace(15, 7) -> " << tree << std::endl;
	std::cout << "Searching for key 15 -> (" << (*tree.find(15)).first << ", " << (*tree.find(15)).second << ")" << std::endl;


	// INSERT
	std::cout << "\nTree before INSERT() -> " << tree << std::endl;

	std::cout << "insert(11, 12) -> " << tree.insert(std::pair<int, int> {11, 12}).second << std::endl; // 1 inserito
	std::cout << "Tree after insert(11, 12) -> " << tree << std::endl;

	std::cout << "insert(3, 12) - already present -> " << tree.insert(std::pair<int, int> {3, 12}).second << std::endl; // 0 already there
	std::cout << "Tree after inserting (3, 12) -> " << tree << std::endl;


	// FIND
	std::cout << "\nTree before FIND() -> " << tree << std::endl;

	std::cout << "find(3) - present -> " << (*tree.find(3)).first << std::endl;
	std::cout << "find(25) - not present -> " << &(*tree.find(25)) << std::endl;
	std::cout << "End address -> " << &(*tree.end()) << std::endl;


	// SUBSCRIPTING 
	std::cout << "\nTree before SUBSCRIPTING -> " << tree << std::endl;

	std::cout << "tree[3] - present -> " << tree[3] << std::endl; // return 12 
	std::cout << "tree[17] - not present -> " << tree[5] << std::endl; //insertion return 0
	tree[5] = 6;
	std::cout << "tree[5] = 6 -> " << tree[5] << std::endl;

	tree[17] = 20;
	std::cout << "tree[17] = 20 -> " << tree[17] << std::endl; //insertion return 0


	// COPY
	std::cout << "\nTree before COPY() -> " << tree << std::endl;

	bst<int,int> t1 = tree;
	bst<int,int> t2{tree};
	std::cout << "tree after tree1 = tree; tree2{tree} -> " << tree << '\n';
	std::cout << "tree1 = tree -> " << t1 << '\n';
	std::cout << "tree2{tree} -> " << t2 << std::endl;

	t1.insert(std::pair<int, int> {20,12});
	t2.insert(std::pair<int, int> {22,12});
	tree.insert(std::pair<int, int> {16,12});
	std::cout << "tree.insert({16, 12}) -> " << tree << '\n';
	std::cout << "tree1.insert({20, 12}) -> " << t1 << '\n';
	std::cout << "tree1.insert({22, 12}) -> " << t2 << std::endl;


	// ERASE
	std::cout << "\nTree before ERASE() -> " << tree << std::endl;

	tree.erase(8);
	std::cout << "erase(8) - root -> " << tree << std::endl;

	tree.erase(5);
	std::cout << "erase(5) - leaf node -> " << tree << std::endl;

	tree.erase(13);
	std::cout << "erase(13) - with only one child node -> " << tree << std::endl;


	// MOVE
	std::cout << "\nTree before MOVE -> " << tree << std::endl;

	bst<int,int> tree1{};
	bst<int,int> tree2{std::move(tree)}; //move ctor?
	std::cout << "tree2{std::move(tree)} -> " << tree2 << std::endl;
	std::cout << "tree after move -> " << tree << std::endl;

	tree1 = std::move(tree2); //move assignment?
	std::cout << "tree1 = std::move(tree2) -> " << tree1 << std::endl;
	std::cout << "tree2 after move -> " << tree2 << std::endl;


	// CLEAR
	std::cout << "\nTree before CLEAR() -> " << tree << std::endl;
	tree.clear();
	std::cout << "Tree after clear() -> " << tree << std::endl;


	// BEGIN AND END
	bst<int,int> tree3{};
	std::cout << "\nNew tree -> " << tree3 << std::endl;
	std::cout << "insert({1, 12}) -> " << &(*tree3.insert(std::pair<int, int> {1,12}).first) << std::endl;
	std::cout << "insert({2, 12}) -> " << &(*tree3.insert(std::pair<int, int> {2,12}).first) << std::endl;
	tree3.insert(std::pair<int, int> {6,12});
	tree3.insert(std::pair<int, int> {3,12});

	std::cout << "New Tree after adding pairs {6, 12}, {3, 12} -> " << tree << std::endl;
	std::cout << "tree.begin() -> " << &(*tree.begin()) << '\t';
	std::cout << "tree.end() -> " << &(*tree.end()) << std::endl;

	tree.erase(1);
	std::cout << "tree.erase(1) -> " << tree << std::endl;
	std::cout << "tree.begin() ->" << &(*tree.begin()) << '\t';
	std::cout << "tree.end() ->" << &(*tree.end()) << std::endl;

	return 0;
}
