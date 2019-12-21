#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <functional>
#include <unordered_map>
#include <utility>
#include <list>
#include <memory>

/// Exception thrown when a key is not found in the container.
class lookup_error : std::exception {
    [[nodiscard]] const char *what() const noexcept override {
        return "lookup_error";
    }
};

/**
 * Implementation of a container with expected O(1) cost of search, insert and
 * erase as in hash map and iteration based on insertion order.
 * Container uses copy-on-write strategy.
 * @tparam K    - key type
 * @tparam V    - value type
 * @tparam Hash - hash function
 */
template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
    // List of values in insertion order.
    using list_t = std::list<std::pair<K, V>>;

    // Hash map for O(1) cost operations.
    using map_t = std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash>;

    // Shared pointers to list and hash map.
    std::shared_ptr<list_t> list;
    std::shared_ptr<map_t> map;

    // Information whether copy constructor must make copy of structures.
    bool mustBeCopied;

    /// Copies list.
    void copyList() {
        list_t newList(*list);
        list.reset();
        list = std::make_shared<list_t>(newList);
    }

    /// Copies map.
    void copyMap() {
        map_t newMap;

        for (auto itr = list->begin(); itr != list->end(); ++itr) {
            newMap.insert({itr->first, itr});
        }

        map.reset();
        map = std::make_shared<map_t>(newMap);
    }

public:
    /// Default constructor.
    insertion_ordered_map() {
        list = std::make_shared<list_t>();
        map = std::make_shared<map_t>();
        mustBeCopied = false;
    }

    /// Copy constructor - COW. Containers share structures until one is modified.
    insertion_ordered_map(insertion_ordered_map const &other) {
        if (other.mustBeCopied) {
            list = std::make_shared<list_t>(list_t(*other.list));
            copyMap();
        } else {
            list = other.list;
            map = other.map;
        }
        mustBeCopied = false;
    }

    /// Move constructor.
    insertion_ordered_map(insertion_ordered_map &&other) noexcept {
        list = std::move(other.list);
        map = std::move(other.map);
        mustBeCopied = false;
    }

    /// Assignment operator.
    insertion_ordered_map &operator=(insertion_ordered_map other) {
        map = other.map;
        list = other.list;
        return *this;
    }

    /**
     * @brief Inserts key @p k with mapped value @p v.
     * Element is inserted if its key is not equivalent to the key of any other
     * element already in the container otherwise value of element with
     * equivalent key doesn't change but the element is moved to the end of
     * iteration order.
     * @param k - key of element
     * @param v - value of element
     * @return @p true if element was successfully inserted, @p false if element
     * with the equivalent key was already in the container.
     */
    bool insert(K const &k, V const &v) {
        list_t *aux_list = list.get();
        map_t *aux_map = map.get();
        try {
            if (!list.unique()) copyList();
            if (!map.unique()) copyMap();
        } catch (std::exception &e) {
            list.reset(aux_list);
            map.reset(aux_map);
            throw e;
        }

        mustBeCopied = false;

        auto found = map->find(k);
        if (found == map->end()) {
            std::pair<K, V> list_pair = std::make_pair(k, v);
            list->push_back(list_pair);
            try {
                std::pair<K, typename std::list<std::pair<K, V>>::iterator>
                        map_pair = make_pair(k, --list->end());
                map->insert(map_pair);
            } catch (std::exception &e) {
                list->pop_back();
                throw e;
            }

            return true;
        } else {
            list->splice(list->end(), *list, found->second);

            return false;
        }
    }

    /**
     * @brief Erases element with key @p k.
     * @param k - key
     * @throws lookup_error when there was no element with key @p k.
     */
    void erase(K const &k) {
        list_t *aux_list = list.get();
        map_t *aux_map = map.get();
        try {
            if (!list.unique()) copyList();
            if (!map.unique()) copyMap();
            mustBeCopied = false;
        } catch (std::exception &e) {
            list.reset(aux_list);
            map.reset(aux_map);
            throw e;
        }

        auto tmp = map->find(k);
        if (tmp == map->end()) {
            throw lookup_error();
        }

        map->erase(k);
        list->erase(tmp->second);
    }

    /**
     * @brief Inserts copies of all elements form container @p other to *this.
     * Values of elements with equivalent keys already in the container don't
     * change. Elements from @p other are inserted at the end, keeping their order.
     * @param other - container to merge with *this
     */
    void merge(insertion_ordered_map const &other) {
        list_t *aux_list = list.get();
        map_t *aux_map = map.get();
        try {
            copyList();
            copyMap();

            for (auto item : *other.list) {
                insert(item.first, item.second);
            }

            mustBeCopied = false;
        } catch (std::exception &e) {
            list.reset(aux_list);
            map.reset(aux_map);
            throw e;
        }
    }

    /**
     * Returns a reference to the mapped value of element with key @p k
     * in the container.
     * @param k - key
     * @return reference to the element with key @k.
     * @throws lookup_error when there was no element with key @p k.
     */
    V &at(K const &k) {
        list_t *aux_list = list.get();
        map_t *aux_map = map.get();
        bool mustBeCopiedBefore = mustBeCopied;

        try {
            if (!list.unique()) copyList();
            if (!map.unique()) copyMap();
        } catch (std::exception &e) {
            list.reset(aux_list);
            map.reset(aux_map);
            throw e;
        }
        mustBeCopied = true;

        try {
            return map->at(k)->second;
        }
        catch (const std::out_of_range &e) {
            mustBeCopied = mustBeCopiedBefore;
            throw lookup_error();
        }
    }

    /**
     * Returns a const reference to the mapped value of element with key @p k
     * in the container.
     * @param k - key
     * @return const reference to the element with key @k.
     */
    V const &at(K const &k) const {
        try {
            return map->at(k)->second;
        }
        catch (const std::out_of_range &e) {
            throw lookup_error();
        }
    }

    /**
     * Returns a reference to the mapped value of element with key @p k
     * in the container. If @k does not match the key of any element in
     * the container, the function inserts a new element with that key and
     * default value of @p V.
     * @param k - key
     * @return reference to the element with key @k.
     */
    V &operator[](K const &k) {
        list_t *aux_list = list.get();
        map_t *aux_map = map.get();
        bool mustBeCopiedBefore = mustBeCopied;

        try {
            if (!list.unique()) copyList();
            if (!map.unique()) copyMap();

            if (map->find(k) == map->end()) {
                insert(k, V{});
            }

            mustBeCopied = true;

            return map->at(k)->second;
        } catch (std::exception &e) {
            list.reset(aux_list);
            map.reset(aux_map);
            mustBeCopied = mustBeCopiedBefore;
            throw e;
        }
    }

    /// Returns the number of elements in the container.
    [[nodiscard]] size_t size() const noexcept {
        return list->size();
    }

    /**
     * Returns a bool value indicating whether the container is empty
     * @return @p true if container is empty, @p false otherwise.
     */
    [[nodiscard]] bool empty() const noexcept {
        return list->empty();
    }

    /// Removes all elements from the container.
    void clear() {
        list_t *aux_list = list.get();
        map_t *aux_map = map.get();

        try {
            if (!list.unique()) {
                list.reset();
                list = std::make_shared<list_t>();
            } else {
                list->clear();
            }

            if (!map.unique()) {
                map.reset();
                map = std::make_shared<map_t>();
            } else {
                map->clear();
            }
            mustBeCopied = false;
        } catch (std::exception &e) {
            list.reset(aux_list);
            map.reset(aux_map);
            throw e;
        }
    }

    /**
     * Returns a bool value indicating whether the container stores element with
     * @p k key,
     * @param k - key
     * @return @p true if element with key @k was found in the container,
     * @p false otherwise.
     */
    bool contains(K const &k) const {
        return map->find(k) != map->end();
    }

    /// Iterator class for getting order of elements in the container.
    class iterator {
        typename list_t::const_iterator itr;

    public:
        friend insertion_ordered_map;

        iterator() = default;

        iterator(iterator const &other) : itr(other.itr) {}

        iterator &operator++() {
            ++itr;
            return *this;
        }

        bool operator==(const iterator &rhs) const {
            return itr == rhs.itr;
        }

        bool operator!=(const iterator &rhs) const {
            return !(rhs == *this);
        }

        const std::pair<K, V> &operator*() const {
            return *itr;
        }

        const std::pair<K, V> *operator->() const {
            return &(*itr);
        }
    };

    /// Returns an iterator pointing to the first element in the container.
    iterator begin() const noexcept {
        iterator iterator;
        iterator.itr = list->cbegin();
        return iterator;
    }

    /// Returns an iterator referring to the past-the-end element in the container.
    iterator end() const noexcept {
        iterator iterator;
        iterator.itr = list->cend();
        return iterator;
    }
};

#endif //INSERTION_ORDERED_MAP_H