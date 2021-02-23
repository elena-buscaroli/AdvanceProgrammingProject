#ifndef _iterator_
#define _iterator_

#include <iostream>
#include <memory>
#include <utility>
#include <iterator>
#include <vector>
#include "bst.hpp"

/**
 * @file iterator.hpp
 * @authors Buscaroli Elena, Valeriani Lucrezia
 * @brief Source code
 */


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

#endif