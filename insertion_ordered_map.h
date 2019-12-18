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
    };

    std::shared_ptr<map_entity> sharedPtr;

public:
    insertion_ordered_map() : sharedPtr(new map_entity) {}

    insertion_ordered_map(insertion_ordered_map const &other) : sharedPtr(other.sharedPtr) {}

    insertion_ordered_map(insertion_ordered_map &&other) noexcept: sharedPtr(other.sharedPtr) {
        other.sharedPtr.reset();
    }

    insertion_ordered_map &operator=(insertion_ordered_map other) {}

    bool insert(K const &k, V const &v) {
    }

    void erase(K const &k) {
        auto value = sharedPtr.get()->list.find(k);
        if (value == sharedPtr.get()->map.end()) {
            throw lookup_error();
        }
        else {
            sharedPtr.get()->list.erase(value.second);
            sharedPtr.get()->map.erase(value.first);
        }
    }

    void merge(insertion_ordered_map const &other) {}

    V &at(K const &k) {
        return sharedPtr.get()->map.at(k)->value;
    }

    V const &at(K const &k) const {}

    V &operator[](K const &k) {}

    [[nodiscard]] size_t size() const {
        return sharedPtr.get()->list.size();
    }

    [[nodiscard]] bool empty() const {
        return sharedPtr.get()->list.empty();
    }

    void clear() {
        if (sharedPtr.use_count() > 0) {

        }
        sharedPtr.get()->map.clear();
        sharedPtr.get()->list.clear();
    }

    bool contains(K const &k) const {
        return sharedPtr.get()->map.find(k) != sharedPtr.get()->map.end();
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
