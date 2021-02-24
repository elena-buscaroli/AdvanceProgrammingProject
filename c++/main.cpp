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
	tree.insert(std::pair<int, int> {8,12}); 
	tree.insert(std::pair<int, int> {3,12}); 
	tree.insert(std::pair<int, int> {10,12}); 
	tree.insert(std::pair<int, int> {6,12});  
	tree.insert(std::pair<int, int> {1,12}); 
	tree.insert(std::pair<int, int> {14,12}); 
	tree.insert(std::pair<int, int> {13,12}); 
	tree.insert(std::pair<int, int> {4,12}); 
	tree.insert(std::pair<int, int> {7,12}); 
	tree.insert(std::pair<int, int> {5,12}); 
	std::cout << "\ntree -> " << tree << std::endl;


	// EMPLACE
	std::cout << "\nEmplace function\n";
	std::cout << "tree before emplace() -> " << tree << std::endl;
	tree.emplace(2, 2);
	std::cout << "emplace(2, 2) -> " << tree << std::endl;
	std::cout << "tree.find(2) -> (" << (*tree.find(2)).first << ", " << (*tree.find(2)).second << ")" << std::endl;

	int l{9};
	tree.emplace(l, 2);
	std::cout << "emplace(9, 2) -> " << tree << std::endl;
	std::cout << "find(9) -> (" << (*tree.find(9)).first << ", " << (*tree.find(9)).second << ")" << std::endl;

	tree.emplace(std::make_pair(15,7));
	std::cout << "emplace(15, 7) -> " << tree << std::endl;
	std::cout << "tree.find(15) -> (" << (*tree.find(15)).first << ", " << (*tree.find(15)).second << ")" << std::endl;


	// INSERT
	std::cout << "\nInsert function\n";
	std::cout << "tree before insert() -> " << tree << std::endl;

	std::cout << "boolean output of insert(11, 12) -> " << tree.insert(std::pair<int, int> {11, 12}).second << std::endl;
	std::cout << "tree after insert(11, 12) -> " << tree << std::endl;

	std::cout << "boolean output of insert(3, 12) - already present -> " << tree.insert(std::pair<int, int> {3, 12}).second << std::endl; 
	std::cout << "tree after inserting (3, 12) -> " << tree << std::endl;


	// FIND
	std::cout << "\nFind function\n";
	std::cout << "tree before find() -> " << tree << std::endl;

	std::cout << "find(3) - present -> " << (*tree.find(3)).first << std::endl;
	std::cout << "find(25) - not present -> " << &(*tree.find(25)) << std::endl;
	std::cout << "tree.end() -> " << &(*tree.end()) << std::endl;


	// SUBSCRIPTING 
	std::cout << "\nSubscripting operator\n";
	std::cout << "tree before subscripting -> " << tree << std::endl;

	std::cout << "tree[5] - present -> " << tree[5] << std::endl;
	std::cout << "tree[17] - not present -> " << tree[17] << std::endl; 
	tree[5] = 6;
	std::cout << "tree[5]=6 -> " << tree[5] << std::endl;

	tree[17] = 20;
	std::cout << "tree[17]=20 -> " << tree[17] << std::endl; 


	// COPY
	std::cout << "\nCopy semantics\n";
	std::cout << "tree before copy -> " << tree << std::endl;

	bst<int,int> t1 = tree;
	bst<int,int> t2{tree};
	std::cout << "tree after tree1=tree; tree2{tree} -> " << tree << '\n';
	std::cout << "tree1=tree -> " << t1 << std::endl;
	std::cout << "tree2{tree} -> " << t2 << std::endl;

	t1.insert(std::pair<int, int> {20,12});
	t2.insert(std::pair<int, int> {22,12});
	tree.insert(std::pair<int, int> {16,12});
	std::cout << "tree.insert({16, 12}) -> " << tree << std::endl;
	std::cout << "tree1.insert({20, 12}) -> " << t1 << std::endl;
	std::cout << "tree1.insert({22, 12}) -> " << t2 << std::endl;


	// ERASE
	std::cout << "\nErase function\n";
	std::cout << "tree before erase() -> " << tree << std::endl;
	tree.erase(8);
	std::cout << "erase(8) - root -> " << tree << std::endl;
	tree.erase(5);
	std::cout << "erase(5) - leaf node -> " << tree << std::endl;
	tree.erase(13);
	std::cout << "erase(13) - with only one child node -> " << tree << std::endl;


	// BALANCE
	std::cout << "\nBalance function\n";
	std::cout << "tree before balance() -> " << tree << std::endl;
	std::cout << "before balance: tree.begin() -> " << &(*tree.begin()) << "\ttree.end() -> " << &(*tree.end()) << '\n';
	tree.balance();
	std::cout << "after balance: tree.begin() -> " << &(*tree.begin()) << "\ttree.end() -> " << &(*tree.end()) << std::endl;
	std::cout << "tree after balance() -> " << tree << std::endl;


	// MOVE
	std::cout << "\nMove semantics\n";
	std::cout << "tree before move -> " << tree << std::endl;

	bst<int,int> tree1{};
	bst<int,int> tree2{std::move(tree)}; 
	std::cout << "tree2{std::move(tree)} -> " << tree2 << std::endl;
	std::cout << "tree after move -> " << tree << std::endl;

	tree1 = std::move(tree2); 
	std::cout << "tree1 = std::move(tree2) -> " << tree1 << std::endl;
	std::cout << "tree2 after move -> " << tree2 << std::endl;


	// CLEAR
	std::cout << "\nClear function\n";
	std::cout << "tree1 before clear() -> " << tree1 << std::endl;
	tree1.clear();
	std::cout << "tree1.clear() -> " << tree1 << std::endl;


	// BEGIN AND END
	std::cout << "\nBegin and End functions\n";
	bst<int,int> tree3{};
	std::cout << "New empty tree -> " << tree3 << std::endl;
	std::cout << "insert({1, 12}) -> " << &(*tree3.insert(std::pair<int, int> {1,12}).first) << std::endl;
	tree3.insert(std::pair<int, int> {2,12});
	tree3.insert(std::pair<int, int> {6,12});
	tree3.insert(std::pair<int, int> {3,12});

	std::cout << "New tree after adding pairs {2,12}, {6, 12}, {3, 12} -> " << tree3 << std::endl;
	std::cout << "tree.begin() -> " << &(*tree3.begin()) << '\t';
	std::cout << "tree.end() -> " << &(*tree3.end()) << std::endl;

	tree3.erase(1);
	std::cout << "tree.erase(1) -> " << tree3 << std::endl;
	std::cout << "tree.begin() -> " << &(*tree3.begin()) << '\t';
	std::cout << "tree.end() -> " << &(*tree3.end()) << std::endl;

	return 0;
}
