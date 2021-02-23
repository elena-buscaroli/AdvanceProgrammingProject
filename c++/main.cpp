#include <iostream>
#include <memory>
#include <utility>
#include <iterator>
#include <vector>
#include "bst.hpp"
#include "iterator.hpp"

/*!
	@file main.cpp
	@authors Buscaroli Elena, Valeriani Lucrezia
	@brief Source code
*/

int main(){

// TEST
	bst<int, int> tree{};
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


	// BALANCE
	std::cout << "\n\ntree.begin() before balance -> " << &(*tree.begin()) << '\t';
	std::cout << "tree.end() before balance -> " << &(*tree.end()) << '\n';
	tree.balance();
	std::cout << "\ntree.begin() after balance -> " << &(*tree.begin()) << '\t';
	std::cout << "tree.end() after balance -> " << &(*tree.end()) << std::endl;
	std::cout << "Tree after BALANCE -> " << tree << std::endl;


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
