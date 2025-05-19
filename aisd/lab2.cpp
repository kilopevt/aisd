//Проблема коллизий должна решаться согласно варианту :
//Метод цепочек

//Хеш - функция должна быть выбрана согласно варианту :
//Метод деления : h(k) = k mod m

//Задачи согласно варианту :
//Построить график зависимости количества коллизий от размера хеш - таблиц.
//Размер хэш таблиц должен изменяться от 25 до 475 с шагом 50, при количестве 
//элементов равном количеству человек в вашей группе и количестве экспериментов 100. 
//Определите размер таблицы, при котором вероятность коллизии меньше 50%.

//Дополнительные задачи :
//Сделать класс хэш - таблиц шаблонным, с возможностью работы с любым типом внутреннего контейнера, 
//например, типом из стандартной библиотеки и самописным классом. (Для метода цепочек)
#include <iostream>
#include <random>

using namespace std;

template<typename K, typename V>
class HashTable {
	struct Node {
		Node* next;
		K key;
		V value;

		Node() : next(nullptr), key(), value() {}
		Node(K key, V val) : next(nullptr), key(key), value(val) {}
	};

	Node** data;
	size_t size;
	size_t capacity;

	size_t hash(K key) {
		return key % capacity;

	}

	void clear() {
		for (size_t i = 0; i < capacity; ++i) {
			Node* cur = data[i];

			while (cur) {
				Node* tmpi = cur;
				cur = cur->next;
				delete tmpi;
			}
		}
		size = 0;
	}

public:
	//Конструктор пустой хэш таблицы заданного размера
	HashTable(size_t cap) : capacity(cap), size(0) {
		data = new Node * [capacity];

		for (size_t i = 0; i < capacity; ++i) {
			data[i] = nullptr;
		}
	}

	//Конструктор, заполняющий хэш таблицу случайными значениями согласно вашему заданию.
	HashTable(size_t table_size, size_t count) : capacity(table_size) {
		data = new Node * [capacity];

		for (size_t i = 0; i < capacity; ++i) {
			data[i] = nullptr;
		}

		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<K> key_dist(0, 10000);
		uniform_int_distribution<V> val_dist(0, 100);

		for (size_t i = 0; i < count; ++i) {
			insert(key_dist(gen), val_dist(gen));
		}
	}

	//Конструктор копирования;
	HashTable(const HashTable& other) : size(other.size), capacity(other.capacity) {
		data = new Node * [capacity];

		for (size_t i = 0; i < capacity; ++i) {
			data[i] = nullptr;
			Node* source = other.data[i];
			Node** dest = &data[i];

			while (source) {
				*dest = new Node(source->key, source->value);
				dest = &((*dest)->next);
				source = source->next;
			}
		}
	}

	//Деструктор;
	~HashTable() {
		clear();
		delete[] data;
	}
	

	//Оператор присваивания;
	HashTable& operator=(const HashTable& other) {
		if (this != &other) {
			clear();
			delete[] data;

			capacity = other.capacity;
			size = other.size;
			data = new Node * [capacity];

			for (size_t i = 0; i < capacity; ++i) {
				data[i] = nullptr;
				Node* source = other.data[i];
				Node** dest = &data[i];
				while (source) {
					*dest = new Node(source->key, source->value);
					dest = &((*dest)->next);
					source = source->next;
				}
			}
		}
		return *this;
	}

	//печать содержимого;
	void print() {
		for (size_t i = 0; i < capacity; ++i) {
			Node* cur = data[i];
			if (!cur) {
				cout << "null" << endl;
				continue;
			}

			size_t id = hash(cur->key);

			cout << i << ": ";
			while (cur) {
				cout << cur->key << ":" << cur->value << " -> ";
				cur = cur->next;
			}
			cout << "null" << endl;
		}
	}

	//вставка значения по ключу;
	bool insert(K key, const V& value) {
		size_t id = hash(key);
		Node** cur = &data[id];

		while (*cur) {
			if ((*cur)->key == key)
				return false;
			cur = &((*cur)->next);
		}

		*cur = new Node(key, value);
		++size;
		return true;
	}

	//вставка или присвоение значения по ключу.
	void insert_or_assign(K key, V& value) {
		size_t id = hash(key);
		Node** cur = &data[id];

		while (*cur) {
			if ((*cur)->key == key) {
				(*cur)->value == value;
				return;
			}
			cur = &((*cur)->next);
		}

		*cur = new Node(key, value);
		++size;
	}

	//проверка наличия элемента по значению;
	bool contains(V& value) {
		for (size_t i = 0; i < capacity; ++i) {
			Node* cur = data[i];

			while (cur) {
				if (cur->value == value) {
					return true;
				}
				cur = cur->next;
			}
		}
		return false;
	}

	//поиск элемента по ключу;
	V* search(K key) {
		size_t id = hash(key);
		Node* cur = data[id];

		while (cur) {
			if (cur->key == key) {
				return &(cur->value);
			}
			cur = cur->next;
		}
		return nullptr;
	}

	//удаление элемента по ключу;
	bool erase(K key) {
		size_t id = hash(key);
		Node** cur = data[id];

		while (*cur) {
			if ((*cur)->key == key) {
				Node* to_delete = *cur;
				*cur = (*cur)->next;
				delete to_delete;
				--size;
				return true;
			}
			cur = &((*cur)->next);
		}
		return false;
	}

	//возвращает количество элементов, у которых значение хэш - функции совпадает с переданным.
	int count(K key) {
		size_t id = hash(key);
		size_t count = 0;
		Node* cur = data[id];

		while (cur) {
			++count;
			cur = cur->next;
		}
		return count;
	}

	// Количество коллизий (корзин с более чем 1 элементом)
	size_t collisions_count() const {
		size_t collision = 0;
		for (size_t i = 0; i < capacity; ++i) {
			if (data[i] && data[i]->next) {
				++collision;
			}
		}
		return collision;
	}
};

void analyze_collisions(size_t group_size) {
	size_t experiments = 100;
	const size_t num_sizes = 10;
	size_t table_sizes[num_sizes] = { 25, 75, 125, 175, 225, 275, 325, 375, 425, 475 };

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> key_dist(0, 10000);
	uniform_int_distribution<int> val_dist(0, 100);

	cout << "Analyze collisions from group for " << group_size << " elements\n";
	cout << "Table size | Average collisions | Collisions probability \n";
	cout << "---------------------------------------------------------\n";

	for (size_t i = 0; i < num_sizes; ++i) {
		size_t table_size = table_sizes[i];
		size_t total_collisions = 0;

		for (size_t i = 0; i < experiments; ++i) {
			HashTable<int, int> ht(table_size);

			// Генерация уникальных ключей
			bool* used_keys = new bool[10001]();
			size_t inserted = 0;

			while (inserted < group_size) {
				int key = key_dist(gen);

				if (!used_keys[key]) {
					used_keys[key] = true;
					ht.insert(key, val_dist(gen));
					inserted++;
				}
			}
			delete[] used_keys;

			if (ht.collisions_count() > 0) {
				++total_collisions;
			}
		}

		double avg_collisions = (double)total_collisions / experiments;
		double collision_prob = (avg_collisions) * 100;

		printf("%-10zu | %-18.2f | %-18.2f%%\n",
			table_size, avg_collisions, collision_prob);

		//if (collision_prob < 50.0) {
		//	cout << "\nBest table sizes: " << table_size
		//		<< " (collisions probability: " << collision_prob << "%)\n" << endl;
		//	return;
		//}
		//else {
		//	cout << "\nNot found suitable table size in range 25-475 with step 50\n" << endl;
		//	return;
		//}
	}
}

int main() {
	HashTable<int, string> ht(4);
	ht.insert(1, "One");
	ht.insert(11, "Eleven");
	ht.insert(21, "Twenty-One");

	cout << "Hash Table Contents:" << endl;
	ht.print();


	HashTable<int, string> ht2(ht);
	ht2.print();
	//for (size_t i = 0; i < 1001; i += 100) {
	//	analyze_collisions(i);
	//}

	analyze_collisions(23);
	return 0;
}