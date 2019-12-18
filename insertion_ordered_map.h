#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <functional>
#include <list>

class lookup_error : std::exception {
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
    std::list<std::pair<K, V>> list;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash> unorderedMap;

public:
    insertion_ordered_map() = default;

    insertion_ordered_map(insertion_ordered_map const &other) {}

    insertion_ordered_map(insertion_ordered_map &&other) noexcept {}

    insertion_ordered_map &operator=(insertion_ordered_map other) {}

    bool insert(K const &k, V const &v) {}

    void erase(K const &k) {}

    void merge(insertion_ordered_map const &other) {}

    V &at(K const &k) {}

    V const &at(K const &k) const {}

    V &operator[](K const &k) {}

    [[nodiscard]] size_t size() const {}

    [[nodiscard]] bool empty() const {}

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
