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


	// FIND
	std::cout << "\nFind function\n"
						<< "tree before find() -> " << tree << '\n'
						<< "find(3) - present -> " << (*tree.find(3)).first << '\n'
						<< "find(25) - not present -> " << &(*tree.find(25)) << '\n'
						<< "tree.end() -> " << &(*tree.end()) << std::endl;


	// EMPLACE
	std::cout << "\nEmplace function\n"
						<< "tree before emplace() -> " << tree << '\n';
	tree.emplace(2, 2);
	std::cout << "emplace(2, 2) -> " << tree << '\n'
						<< "tree.find(2) -> (" << (*tree.find(2)).first << ", " << (*tree.find(2)).second << ")" << '\n';

	int l{9};
	tree.emplace(l, 2);
	std::cout << "emplace(9, 2) -> " << tree << '\n'
						<< "find(9) -> (" << (*tree.find(9)).first << ", " << (*tree.find(9)).second << ")" << '\n';

	tree.emplace(std::make_pair(15,7));
	std::cout << "emplace(15, 7) -> " << tree << '\n'
						<< "tree.find(15) -> (" << (*tree.find(15)).first << ", " << (*tree.find(15)).second << ")" << std::endl;


	// INSERT
	std::cout << "\nInsert function\n"
						<< "tree before insert() -> " << tree << '\n'
						<< "boolean output of insert(11, 12) -> " << tree.insert(std::pair<int, int> {11, 12}).second << '\n'
						<< "tree after insert(11, 12) -> " << tree << '\n'
						<< "boolean output of insert(3, 12) - already present -> " << tree.insert(std::pair<int, int> {3, 12}).second << '\n'
						<< "tree after inserting (3, 12) -> " << tree << std::endl;


	// SUBSCRIPTING 
	std::cout << "\nSubscripting operator\n"
						<< "tree before subscripting -> " << tree << '\n'
						<< "tree[5] - present -> " << tree[5] << '\n'
						<< "tree[17] - not present -> " << tree[17] << '\n';
	tree[5] = 6;
	std::cout << "tree[5]=6 -> " << tree[5] << '\n';

	tree[17] = 20;
	std::cout << "tree[17]=20 -> " << tree[17] << std::endl; 


	// COPY
	std::cout << "\nCopy semantics\n"
						<< "tree before copy -> " << tree << '\n';

	bst<int,int> t1 = tree;
	bst<int,int> t2{tree};
	std::cout << "tree after tree1=tree; tree2{tree} -> " << tree << '\n'
						<< "tree1=tree -> " << t1 << '\n'
						<< "tree2{tree} -> " << t2 << '\n';

	t1.insert(std::pair<int, int> {20,12});
	t2.insert(std::pair<int, int> {22,12});
	tree.insert(std::pair<int, int> {16,12});
	std::cout << "tree.insert({16, 12}) -> " << tree << '\n'
						<< "tree1.insert({20, 12}) -> " << t1 << '\n'
						<< "tree2.insert({22, 12}) -> " << t2 << std::endl;


	// ERASE
	std::cout << "\nErase function\n"
						<< "tree before erase() -> " << tree << '\n';
	tree.erase(8);
	std::cout << "erase(8) - root -> " << tree << '\n';
	tree.erase(5);
	std::cout << "erase(5) - leaf node -> " << tree << '\n';
	tree.erase(13);
	std::cout << "erase(13) - with only one child node -> " << tree << std::endl;


	// BALANCE
	std::cout << "\nBalance function\n"
						<< "tree before balance() -> " << tree << '\n'
						<< "before balance: tree.begin() -> " << &(*tree.begin()) << "\ttree.end() -> " << &(*tree.end()) << '\n';
	tree.balance();
	std::cout << "after balance: tree.begin() -> " << &(*tree.begin()) << "\ttree.end() -> " << &(*tree.end()) << '\n'
						<< "tree after balance() -> " << tree << std::endl;


	// MOVE
	std::cout << "\nMove semantics\n"
						<< "tree before move -> " << tree << '\n';

	bst<int,int> tree1{};
	bst<int,int> tree2{std::move(tree)}; 
	std::cout << "tree2{std::move(tree)} -> " << tree2 << '\n'
						<< "tree after move -> " << tree << '\n';

	tree1 = std::move(tree2); 
	std::cout << "tree1 = std::move(tree2) -> " << tree1 << '\n'
						<< "tree2 after move -> " << tree2 << std::endl;


	// CLEAR
	std::cout << "\nClear function\n"
						<< "tree1 before clear() -> " << tree1 << '\n';
	tree1.clear();
	std::cout << "tree1.clear() -> " << tree1 << std::endl;


	// BEGIN AND END
	std::cout << "\nBegin and End functions\n";
	bst<int,int> tree3{};

	std::cout << "New empty tree -> " << tree3 << '\n'
						<< "address of pair{1,12} after insert({1, 12}) -> " << &(*tree3.insert(std::pair<int, int> {1,12}).first) << '\n';
	
	tree3.insert(std::pair<int, int> {2,12});
	tree3.insert(std::pair<int, int> {6,12});
	tree3.insert(std::pair<int, int> {3,12});

	std::cout << "New tree after adding pairs {2,12}, {6, 12}, {3, 12} -> " << tree3 << '\n'
						<< "tree.begin() -> " << &(*tree3.begin()) << '\t'
						<< "tree.end() -> " << &(*tree3.end()) << '\n';

	tree3.erase(1);
	std::cout << "tree.erase(1) -> " << tree3 << '\n'
						<< "tree.begin() -> " << &(*tree3.begin()) << '\t'
						<< "tree.end() -> " << &(*tree3.end()) << std::endl;

	return 0;
}
