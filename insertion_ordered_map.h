#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <functional>

class lookup_error : std::exception {
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
public:
    insertion_ordered_map() {}

    insertion_ordered_map(insertion_ordered_map const &other) {}

    insertion_ordered_map(insertion_ordered_map &&other) {}

    insertion_ordered_map &operator=(insertion_ordered_map other) {}

    bool insert(K const &k, V const &v) {}

    void erase(K const &k) {}

    void merge(insertion_ordered_map const &other) {}

    V &at(K const &k) {}

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
