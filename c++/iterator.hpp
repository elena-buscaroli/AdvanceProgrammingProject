#ifndef _iterator_
#define _iterator_

#include <iostream>
#include <memory>
#include <utility>
#include <iterator>
#include <vector>
#include "bst.hpp"

/*!
	@file iterator.hpp
	@authors Buscaroli Elena, Valeriani Lucrezia
	@brief Header containing the implementation of _iterator class
*/


/*!
	@tparam node_t template for an object of type node_t.
	@tparam T template for an object of type T that is the pair_type.
*/

template < typename node_t, typename T >
class _iterator {

/*!
	@brief Raw pointer to the current node of type node_t.
*/
	node_t* current;

	public:
		using v_type = T;
		using reference = v_type&;
		using pointer = v_type*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

/*!
	@brief Constructor of new _iterator object.
	@tparam p raw pointer to a node.
*/
		explicit _iterator(node_t* p) : current{p} {}

/*!
	@brief Default deconstructor of _iterator object.
*/
		~_iterator() noexcept = default;

/*!
	@brief Dereference operator.
	@return Reference to the value stored by the node pointed by the iterator.
*/
		reference operator*() const { return current->value; }

/*!
	@brief Reference operator.
	@return Pointer to the value stored by the node pointed by the iterator.
*/
		pointer operator->() const { return &**this; }


/*!
	@brief Overloading of equality operator.
	@tparam a reference to the first iterator.
	@tparam b reference to the first iterator.
	@return Bool true if they point to the same node, false otherwise.
*/
		friend bool operator==(_iterator &a, _iterator &b) { return a.current == b.current; }


/*!
	@brief Overloading of inequality operator.
	@tparam a reference to the first iterator.
	@tparam b reference to the first iterator.
	@return Bool true if they point to different nodes, false otherwise.
*/
		friend bool operator!=(_iterator &a, _iterator &b) { return !(a == b); }


/*!
	@brief Overloading of pre-increment operator ++.
	@brief Allow for the traverse of the bst from begin() to end() so, from left to right.
	@return Reference to the iterator.
*/
		_iterator &operator++() {
			if(current->right) { //if !=nullptr 
				current = current->right.get();
				while(current->left){
					current = current->left.get();
				}
				return *this;
			}
			else if (current->parent) {
				node_t* tmp = current;
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

#endif