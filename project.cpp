#include <iostream>
#include <memory>  
#include <utility>
#include <iterator>

//g++ -std=c++14 project.cpp
//g++ -std=c++14 project.cpp


template < typename T >
struct node{
  std::unique_ptr<node> right;
  std::unique_ptr<node> left;
  std::unique_ptr<node> parent;

  T value;

  node() {std::cout << "creation \n";};

  //x è la mia pair
  //copy ctor
  //explicit node(const T& x): value{x} {std::cout << "l-value 1\n";}
  node(const T& x): right{nullptr}, left{nullptr}, value{x} {std::cout << "l-value 2\n";}
  //node(const T& x, node* r, node* l): right{r}, left{l}, value{x} {std::cout << "l-value 2\n";}

  explicit node(T&& x): value{std::move(x)} {std::cout << "r-value 1\n";}
  //node(T&& x, node* r, node* l): right{r}, left{l}, value{std::move(x)} {std::cout << "r-value 2" << std::endl;}

  explicit node(const std::unique_ptr<node>& x): value{x->value}{ //this is used for the copy constructor of the bst 
    if(x->right)
      right.reset(new node{x->right});

    if(x->left)
      left.reset(new node{x->left});
    
    if(x->parent)
      parent.reset(new node{x->parent});
  }

  ~node() = default;
};


template < typename T, typename node>
class _iterator {
  node* current;

  public:
    using v_type = T;
    using reference = v_type&;
    using pointer = v_type*;

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    explicit _iterator(node* p) : current{p} {}
	  ~_iterator() noexcept = default;

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
      if(current->right) {
        current = current->right.get();
        while(current->left){
          current = current->left.get();
        }
        return *this;
      }

      else if(current->parent) {
        auto tmp{*this};
        current = current->parent.get();
        while(tmp == current->right.get()){
          auto tmp{*this};
          current = current->parent.get();
        }
        return *this;
      }
      return *this;
    }
};

template < typename value_type, typename key_type, typename cmp_op=std::less<key_type> >
class bst
{
  using pair_type = std::pair< const key_type, value_type >;
  using node_t = node<pair_type>;
  using iterator = _iterator<node_t , pair_type> ;
  using const_iterator = _iterator<node_t , const pair_type> ;

  std::unique_ptr<node_t> root;

  cmp_op op;

  public:

    // insert
    //The function returns a pair of an iterator (pointing to the node) and a bool. 
    //The bool is true if a new node has been allocated, 
    //false otherwise (i.e., the key was already present in the tree). 

    //return std::make_pair(iterator{root.get()}, true);
    template <typename o> //forwarding reference for using both r and l value 
    std::pair<iterator, bool> insert(const o&& x){
      n = find(x.first)
      if (n){
         return std::make_pair(n, false)
      }
      if (!root){ // if root==nullptr
          n_tmp = new node{std::forward<o>(x)};
          root.reset(n_tmp);
          return std::make_pair(iterator{root}, true)
      }
      auto tmp=root.get();
      while(tmp->right)||(tmp->left){ //!=nullptr
        if (op(tmp->value.first, x.first)){ //my key > root
          tmp = tmp->right;
        }
        else if (op(x.first,tmp->value.first,)){ //my key < root
          tmp = tmp->left;
        }

      }
    };

    //std::pair<iterator, bool> insert(pair_type&& x);

    // Emplace
    template< class... Types >
    std::pair<iterator,bool> emplace(Types&&... args);

		// clear
    //since the destructor are then called recursively 
		void clear(){
			root.reset();
		}

    // begin
    //begin() returns an iterator to beginning 
    //while cbegin() returns a const_iterator to beginning.
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
		// iterator end();
		// const_iterator end() const;
		// const_iterator cend() const;
		iterator end()  { return iterator{nullptr}; }
		const_iterator end() const  { return const_iterator{nullptr}; }
	  const_iterator cend() const  { return const_iterator{nullptr}; }

    // find
    iterator find(const key_type& x);
    const_iterator find(const key_type& x) const;

    // balance
    void balance();

    // Subscripting operator
    value_type& operator[](const key_type& x);
    value_type& operator[](key_type&& x);

    // put-to 
    friend
    std::ostream& operator<<(std::ostream& os, const bst& x);

    // erase
    void erase(const key_type& x);

}; // end bst class


int main(){

  using type = std::pair<int, int>;

  const std::pair<int, int> p(1, 10);

  //node<type> n{}; //creation

  node<type> n1{(std::pair<int, int>(1, 10))}; //rvalue 1

  node<type> n2{p}; //lvalue 1 

  return 0;
}
