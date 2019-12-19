#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <functional>
#include <unordered_map>
#include <utility>
#include <list>
#include <memory>


class lookup_error : std::exception {
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
    struct map_entity {
        std::list<std::pair<K, V>> list;
        std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash> map;

        map_entity(map_entity const &other) : list(other.list), map(other.map) {}

        bool insert(K const &k, V const &v) {
        }

        typename std::list<std::pair<K, V>>::iterator find(K const &k) {
            auto tmp = map.find(k);
            if (tmp == map.end()) {
                throw lookup_error();
            }

            return tmp;
        }

        void erase(K const &k) {
            auto tmp = find(k);

            list.erase(tmp);
            map.erase(tmp.first);
        }


        void clear() {
            map.clear();
            list.clear();
        }
    };

    std::shared_ptr<map_entity> sharedPtr;

    void copySharedPtrIfNecessary() {
        if (sharedPtr.use_count() > 1) {
            auto tmp = *sharedPtr;
            sharedPtr.reset();
            sharedPtr(new map_entity(tmp));
        }
    }

public:
    insertion_ordered_map() : sharedPtr(new map_entity) {}

    insertion_ordered_map(insertion_ordered_map const &other) : sharedPtr(other.sharedPtr) {}

    insertion_ordered_map(insertion_ordered_map &&other) noexcept: sharedPtr(other.sharedPtr) {
        other.sharedPtr.reset();
    }

    insertion_ordered_map &operator=(insertion_ordered_map other) {
        sharedPtr = other.sharedPtr;
        return *this;
    }

    bool insert(K const &k, V const &v) {
    	if (!sharedPtr.unique()) {
    		sharedPtr = std::make_shared<map_entity>(sharedPtr);
    	}

    	std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash>
    	        map = sharedPtr.get();
    	std::list<std::pair<K, V>> list = sharedPtr.get();
    	auto found = map->find(k);
    	if (found == map->end()) {
    		std::pair<K, V> list_pair = make_pair(k, v);
    		list.push_back(list_pair);
    		std::pair<K, typename std::list<std::pair<K, V>>::iterator>
    		        map_pair = make_pair(k, list.end()--);
    		map.insert(map_pair);

    		return true;
    	}
    	else {
    		list.splice(list.end(), list, found);

    		return false;
    	}
    }

    void erase(K const &k) {
        copySharedPtrIfNecessary();

        sharedPtr->erase(k);
    }

    void merge(insertion_ordered_map const &other) {}

    V &at(K const &k) {
        copySharedPtrIfNecessary();
        auto tmp = sharedPtr->find(k);

        return tmp->second;
    }

    V const &at(K const &k) const {
        auto tmp = sharedPtr->find(k);

        return tmp->second;
    }

    V &operator[](K const &k) {
        copySharedPtrIfNecessary();
        auto tmp = sharedPtr->map->find(k);

        if (tmp == sharedPtr->map.end()) {
            sharedPtr.insert(k, new V());
        }

        return sharedPtr->map[k];
    }

    [[nodiscard]] size_t size() const {
        return sharedPtr->list.size();
    }

    [[nodiscard]] bool empty() const {
        return sharedPtr->list.empty();
    }

    void clear() {
        copySharedPtrIfNecessary();

        sharedPtr->clear();
    }

    bool contains(K const &k) const {
        return sharedPtr->map.find(k) != sharedPtr->map.end();
    }

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
