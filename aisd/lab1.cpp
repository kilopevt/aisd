


#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

class BinaryTree {
	struct Node {
		int _key;
		Node* _left;
		Node* _right;

		Node(int key) : _key(key), _left(nullptr), _right(nullptr) {}
	};

	Node* _root;

	void destroy(Node* node) {
		if (node) {
			destroy(node->_left);
			destroy(node->_right);
			delete node;
		}
	}

	Node* insert(Node* node, int key) {
		if (!node)
			return new Node(key);

		if (key < node->_key)
			node->_left = insert(node->_left, key);
		else if (key > node->_key)
			node->_right = insert(node->_right, key);

		return node;
	}

	bool contains(Node* node, int key) {
		if (!node)
			return false;

		if (key == node->_key)
			return true;

		return (key < node->_key) ? contains(node->_left, key) : contains(node->_right, key);
	}

	Node* erase(Node* node, int key) {
		if (!node)
			return nullptr;

		if (key < node->_key) {
			node->_left = erase(node->_left, key);
		}
		else if (key > node->_key) {
			node->_right = erase(node->_right, key);
		}
		else {
			//if node don't have child or have 1 child
			if (!node->_left) {
				Node* temp = node->_right;
				delete node;
				return temp;
			}
			else if (!node->_right) {
				Node* temp = node->_left;
				delete node;
				return temp;
			}

			//if node have 2 children
			Node* temp = minValueNode(node->_right);
			node->_key = temp->_key;
			node->_right = erase(node->_right, temp->_key);
		}
		return node;
	}

	//helper function to delete a node with 2 children
	Node* minValueNode(Node* node) {
		Node* current = node;

		while (current && current->_left)
			current = current->_left;

		return current;
	}

	void print(Node* node) {
		if (node) {
			print(node->_left);
			cout << node->_key << " ";
			print(node->_right);
		}
	}

	Node* copy(Node* node) {
		if (node == nullptr)
			return nullptr;

		Node* new_node = new Node(node->_key);
		new_node->_left = copy(node->_left);
		new_node->_right = copy(node->_right);

		return new_node;
	}

	int size(Node* node) {
		if (!node)
			return 0;

		return 1 + size(node->_left) + size(node->_right);
	}

	void toVector(Node* node, vector<int>& vec) {
		if (!node) return;

		toVector(node->_left, vec);
		vec.push_back(node->_key);
		toVector(node->_right, vec);
	}

public:
	BinaryTree() : _root(nullptr) {}

	//copy constructor
	BinaryTree(const BinaryTree& other) {
		_root = copy(other._root);
	}

	//destructor
	~BinaryTree() {
		destroy(_root);
	}

	//assignment operator
	BinaryTree& operator=(const BinaryTree& other) {
		if (this != &other) {
			destroy(_root);
			_root = nullptr;
			_root = copy(other._root);
		}
		return *this;
	}

	//print content
	void print() {
		print(_root);
		cout << endl;
	}

	//element presence check
	bool contains(int key) {
		return contains(_root, key);
	}

	//insert element
	bool insert(int key) {
		if (!contains(key)) {
			_root = insert(_root, key);
			return true;
		}
		return false;
	}

	//delete element
	bool erase(int key) {
		if (contains(key)) {
			_root = erase(_root, key);
			return true;
		}
		return false;
	}

	int size() {
		size(_root);
	}

	void toVector(vector<int>& vec) {
		toVector(_root, vec);
	}
};

//Вариант 4: для заданного std::vector<int> верните новый std::vector<int>, 
//содержащий все неповторяющиеся элементы (для вектора {3 2 2 4 2} результат 
//должен быть {3 4} )

vector<int> getUniqueElements(const vector<int>& vec) {
	BinaryTree tree;

	for (size_t i = 0; i < vec.size(); ++i) {
		int num = vec[i];
		int count = 0;

		for (size_t j = 0; j < vec.size(); ++j) {
			if (vec[j] == num)
				count++;
		}

		if (count == 1)
			tree.insert(vec[i]);
	}

	vector<int> unique;

	tree.toVector(unique);

	return unique;
}

//---------------------------------------------------------------------------------------------------------

//pseudo-random number generator
size_t lcg() {
	static size_t x = 0;
	x = (1021 * x + 24631) % 116640;
	return x;
}

//generate unique random numbers and fill the tree
void fillTreeWithUniqueRandomNumbers(BinaryTree& tree, size_t count) {
	vector<int> unique_numbers;

	while (unique_numbers.size() < count) {
		int num = lcg() % 100000;

		unique_numbers.push_back(num);
		tree.insert(num);
	}
}

//average time to fill a tree
double measureFillTime(size_t count, size_t trials) {
	double total_time = 0;

	for (size_t i = 0; i < trials; ++i) {
		BinaryTree tree;

		auto start = chrono::high_resolution_clock::now();
		fillTreeWithUniqueRandomNumbers(tree, count);
		auto end = chrono::high_resolution_clock::now();
		total_time += chrono::duration<double, milli>(end - start).count();
	}
	return total_time / trials;
}

//average search time
double measureSearchTime(BinaryTree& tree, size_t trials) {
	double total_time = 0;

	for (size_t i = 0; i < trials; ++i) {
		int random_number = lcg() % 100000; 

		auto start = chrono::high_resolution_clock::now();
		tree.contains(random_number);
		auto end = chrono::high_resolution_clock::now();
		total_time += chrono::duration<double, milli>(end - start).count();
	}
	return total_time / trials;
}

//average insertion and deletion time
double measureInsertDeleteTime(size_t count, size_t trials) {
	BinaryTree tree;
	fillTreeWithUniqueRandomNumbers(tree, count);

	double total_insert_time = 0;
	double total_delete_time = 0;

	//insert
	for (size_t i = 0; i < trials; ++i) {
		int random_number = lcg() % 100000; 

		auto start = chrono::high_resolution_clock::now();
		tree.insert(random_number);
		auto end = chrono::high_resolution_clock::now();
		total_insert_time += chrono::duration<double, milli>(end - start).count();
	}

	//delete
	for (size_t i = 0; i < trials; ++i) {
		int random_number = lcg() % 100000;

		auto start = chrono::high_resolution_clock::now();
		tree.erase(random_number);
		auto end = chrono::high_resolution_clock::now();
		total_delete_time += chrono::duration<double, milli>(end - start).count();
	}

	return (total_insert_time + total_delete_time) / (2 * trials);
}

//--------------------------------------------------------------------------------------------

int main() {
	BinaryTree tree;
	tree.insert(30);
	tree.insert(20);
	tree.insert(50);
	tree.insert(14);
	tree.insert(45);
	tree.insert(80);
	tree.insert(80); //repeat add 80
	tree.insert(9);
	tree.insert(16);
	tree.insert(47);
	tree.insert(84);
	tree.insert(15);
	/*
							30
							|
				20<---------+--------->50
				|					   |
		 14<----+			    45<----+---->80
		 |						|			 |
	 9<--+-->16					+-->47		 +-->84
			 |
		 15<-+
	*/ 

	//test assignment operator
	BinaryTree tree1 = tree;

	//print tree and copy of tree
	tree.print(); // 9 14 15 16 20 30 45 47 50 80 84
	tree1.print();// 9 14 15 16 20 30 45 47 50 80 84

	//test contains
	int num = 2;
	if (tree.contains(num))
		cout << num << " contains in tree" << endl;
	else
		cout << num << " not contains in tree" << endl;

	num = 20;
	if (tree.contains(num))
		cout << num << " contains in tree" << endl;
	else
		cout << num << " not contains in tree" << endl;


	//test erase
	tree.erase(45);
	tree.print(); // 9 14 15 16 20 30 47 50 80 84

	//---------------------------------------------------------------------

	//test the task
	vector<int> numbers = { 1, 2, 3, 2, 4, 5, 5, 6 };

	for (int i = 0; i < numbers.size(); ++i) {
		cout << numbers[i] << " ";
	}
	cout << endl;

	vector<int> unique_numbers = getUniqueElements(numbers);

	cout << "Unique elements: ";
	for (int num = 0; num < unique_numbers.size(); ++num)
		cout << unique_numbers[num] << " "; //1 3 4 6
	cout << std::endl;

	//-------------------------------------------------------------------------

	//test the time
	cout << "Average fill time for 1000 unique numbers: " << measureFillTime(1000, 100) << " ms" << endl;
	cout << "Average fill time for 10000 unique numbers: " << measureFillTime(10000, 100) << " ms" << endl; 
	cout << "Average fill time for 100000 unique numbers: " << measureFillTime(100000, 100) << " ms\n" << endl; 

	BinaryTree tree1000, tree10000, tree100000; 
	fillTreeWithUniqueRandomNumbers(tree1000, 1000); 
	fillTreeWithUniqueRandomNumbers(tree10000, 10000); 
	fillTreeWithUniqueRandomNumbers(tree100000, 100000); 

	cout << "Average search time in 1000 elements: " << measureSearchTime(tree1000, 1000) << " ms" << endl; 
	cout << "Average search time in 10000 elements: " << measureSearchTime(tree10000, 1000) << " ms" << endl; 
	cout << "Average search time in 100000 elements: " << measureSearchTime(tree100000, 1000) << " ms\n" << endl; 

	cout << "Average insert and delete time for 1000 elements: " << measureInsertDeleteTime(1000, 1000) << " ms" << endl; 
	cout << "Average insert and delete time for 10000 elements: " << measureInsertDeleteTime(10000, 1000) << " ms" << endl; 
	cout << "Average insert and delete time for 100000 elements: " << measureInsertDeleteTime(100000, 1000) << " ms\n" << endl; 

	return 0;
}


