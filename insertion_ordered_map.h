#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <functional>
#include <unordered_map>
#include <utility>


class lookup_error : std::exception {
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
	struct Node {
		K key;
		V value;
		Node *prev, *next;
	} typedef Node;

	Node *first = nullptr, *last = nullptr;

	void add_new_Node() {
		Node* ret = new Node;
		ret->prev = last;
		ret->next = nullptr;
		last = ret;
	}

	std::unordered_map<K, Node*> map;


public:
    insertion_ordered_map() {
    	add_new_Node();
    	first = last;
    }

    insertion_ordered_map(insertion_ordered_map const &other) {}

    insertion_ordered_map(insertion_ordered_map &&other) {}

    insertion_ordered_map &operator=(insertion_ordered_map other) {}

    bool insert(K const &k, V const &v) {
    	last->key = k;
    	last->value = v;
    	auto res = map.emplace(std::make_pair(k, last));
    	if (res.second) {
    		add_new_Node();
    	}
    	else {
    		Node* node = res.first->second;
    		node->prev->next = node->next;
    		node->next->prev = node->prev;
    		last->next = node;
    		node->prev = last;
    		last = node;
    	}
    	return res.second;
    }

    void erase(K const &k) {}

    void merge(insertion_ordered_map const &other) {}

    V &at(K const &k) {
    	return map.at(k)->value;
    }

    V const &at(K const &k) const {}

    V &operator[](K const &k) {}

    size_t size() const {}

    bool empty() const {}

    void clear() {}

    bool contains(K const &k) const {}

    /** Klasę iteratora o nazwie iterator oraz metody begin i end, pozwalające
        przeglądać zbiór kluczy w kolejności ich wstawienia. Iteratory mogą być
        unieważnione przez dowolną operację modyfikacji zakończoną powodzeniem.
        Iterator powinien udostępniać przynajmniej następujące operacje:
        - konstruktor bezparametrowy i kopiujący
        - operator++ prefiksowy
        - operator== i operator!=
        - operator* (dereferencji)
        Wszystkie operacje w czasie O(1). Przejrzenie całego słownika w czasie O(n)*/
    class iterator {
    };

};

#endif //INSERTION_ORDERED_MAP_H
