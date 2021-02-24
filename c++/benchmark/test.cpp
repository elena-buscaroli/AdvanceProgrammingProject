#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <chrono>
#include <random>
#include <algorithm>
#include <vector>

#include "bst.hpp"

template<typename T>
void generate_tree(T& tree, size_t size) {
	std::vector<int> v(size);
	
	std::cout << v.size() << '\n';

	for (size_t i=0; i<size; ++i) { 
		v[i]=i;}
	
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(v.begin(), v.end(),g);

	for (auto p : v){
		tree.insert(std::pair<int, int> {p,p});}

	return; 
}

template<typename T>
void test(T& tree, size_t size, std::ofstream& f) {
	f << size << '\t';
	auto c = 0;

	auto start = std::chrono::high_resolution_clock::now();

	for(size_t i=0; i<size; i++) {  // for each node in the tree
		tree.find(i);
		c++;
	}

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	auto final = microseconds/(double)size;

	f << final << "\t";

	f << std::endl;

	return;
}


int main() {

	bst<int, int> bst_tree;
	bst<int, int> bst_balanced_tree;
	std::map<int, int> map_tree;
	std::unordered_map<int, int> unmap_tree;

	std::vector<size_t> s = {100, 200, 400, 600, 800, 1000, 2000, 4000, 6000, 8000, 10000, 20000, 40000, 60000, 80000, 100000, 200000, 400000, 600000, 800000,1000000, 2000000, 4000000, 6000000, 8000000};

	std::ofstream f1("bst.csv");
	std::ofstream f2("bst_bal.csv");
	std::ofstream f3("map.csv");
	std::ofstream f4("unmap.csv");

	for(auto size : s) {
		generate_tree(bst_tree, size);
		bst_balanced_tree = bst_tree;
		bst_balanced_tree.balance();
		generate_tree(map_tree, size);
		generate_tree(unmap_tree, size);

		test(bst_tree, size, f1);
		test(bst_balanced_tree, size, f2);
		test(map_tree, size, f3);
		test(unmap_tree, size, f4);
	}
	
	f1.close();
	f2.close();
	f3.close();
	f4.close();

	return 0;
}