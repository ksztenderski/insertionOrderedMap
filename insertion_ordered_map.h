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
    using list_t = std::list<std::pair<K, V>>;
    using map_t = std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash>;

    std::shared_ptr<list_t> sharedList;
    std::shared_ptr<map_t> sharedMap;

    void copySharedListIfNecessary() {
        if (!sharedList.unique()) {
            sharedList = std::make_shared<list_t>(sharedList);
        }
    }

    void copySharedMapIfNecessary() {
        if (!sharedMap.unique()) {
            sharedMap = std::make_shared<map_t>(sharedMap);
        }
    }

public:
    insertion_ordered_map() : sharedList(new list_t), sharedMap(new map_t) {}

    insertion_ordered_map(insertion_ordered_map const &other) : sharedList(other.sharedList),
                                                                sharedMap(other.sharedMap) {}

    insertion_ordered_map(insertion_ordered_map &&other) noexcept {}

    insertion_ordered_map &operator=(insertion_ordered_map other) {
        sharedMap = other.sharedMap;
        sharedList = other.sharedList;
        return *this;
    }

    bool insert(K const &k, V const &v) {
        copySharedListIfNecessary();
        copySharedMapIfNecessary();

        list_t list = *sharedList;
        map_t map = *sharedMap;

        auto found = map.find(k);
        if (found == map.end()) {
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
        copySharedListIfNecessary();
        copySharedMapIfNecessary();

        auto tmp = sharedMap->find(k);
        if (tmp == sharedMap->end()) {
            throw lookup_error();
        }

        sharedList->erase(k);
    }

    void merge(insertion_ordered_map const &other) {}

    V &at(K const &k) {
        copySharedListIfNecessary();
        copySharedMapIfNecessary();

        V result;

        try {
            result = sharedMap->at(k);
        }
        catch (const std::out_of_range &e) {
            throw lookup_error();
        }

        return result->second;
    }

    V const &at(K const &k) const {
        V result;

        try {
            result = sharedMap->at(k);
        }
        catch (const std::out_of_range &e) {
            throw lookup_error();
        }

        return result->second;
    }

    V &operator[](K const &k) {
        copySharedListIfNecessary();
        copySharedMapIfNecessary();

        auto result = sharedMap->find(k);

        if (result == sharedMap->end()) {
            insert(k, new V());
        }

        return sharedMap.get()[k];
    }

    [[nodiscard]] size_t size() const {
        return sharedList->size();
    }

    [[nodiscard]] bool empty() const {
        return sharedList->empty();
    }

    void clear() {
        copySharedListIfNecessary();
        copySharedMapIfNecessary();

        sharedMap->clear();
        sharedList->clear();
    }

    bool contains(K const &k) const {
        return sharedMap->find(k) != sharedMap->end();
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
