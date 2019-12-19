#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <functional>
#include <unordered_map>
#include <utility>
#include <list>
#include <memory>


class lookup_error : std::exception {
	[[nodiscard]] const char *what() const noexcept override {
		return "lookup_error";
	}
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {
private:
	using list_t = std::list<std::pair<K, V>>;
	using map_t = std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash>;

	std::shared_ptr<list_t> list;
	std::shared_ptr<map_t> map;

	bool must_be_copied;

	void copyList() {
		list_t newList(*list);
		list.reset();
		list = std::make_shared<list_t>(newList);
	}

	void copyMap() {
		map_t newMap;

		for (auto itr = list->begin(); itr != list->end(); ++itr) {
			newMap.insert({itr->first, itr});
		}

		map.reset();
		map = std::make_shared<map_t>(newMap);
	}

public:
	insertion_ordered_map() {
		list = std::make_shared<list_t>();
		map = std::make_shared<map_t>();
		must_be_copied = false;
	}

	insertion_ordered_map(insertion_ordered_map const &other) {
		if (other.must_be_copied) {
			list = std::make_shared<list_t>(list_t(*other.list));
			map = std::make_shared<map_t>(map_t(*other.map));
		} else {
			list = other.list;
			map = other.map;
		}
		must_be_copied = false;
	}

	insertion_ordered_map(insertion_ordered_map &&other) noexcept {
		list = std::move(other.list);
		map = std::move(other.map);
		must_be_copied = false;
	}

	insertion_ordered_map &operator=(insertion_ordered_map other) {
		map = other.map;
		list = other.list;
		return *this;
	}

	bool insert(K const &k, V const &v) {
		list_t* aux_list = list.get();
		map_t* aux_map = map.get();
		try {
			if (!list.unique()) copyList();
			if (!map.unique()) copyMap();
		} catch (std::exception& e) {
			list.reset(aux_list);
			map.reset(aux_map);
			throw e;
		}

		auto found = map->find(k);
		if (found == map->end()) {
			std::pair<K, V> list_pair = std::make_pair(k, v);
			list->push_back(list_pair);
			std::pair<K, typename std::list<std::pair<K, V>>::iterator>
					map_pair = make_pair(k, --list->end());
			try {
				map->insert(map_pair);
			} catch(std::exception& e) {
				list->pop_back();
				throw e;
			}

			return true;
		} else {
			list->splice(list->end(), *list, found->second);

			return false;
		}
	}

	void erase(K const &k) {
		list_t* aux_list = list.get();
		map_t* aux_map = map.get();
		try {
			if (!list.unique()) copyList();
			if (!map.unique()) copyMap();
		} catch(std::exception& e) {
			list.reset(aux_list);
			map.reset(aux_map);
			throw e;
		}

		auto tmp = map->find(k);
		if (tmp == map->end()) {
			throw lookup_error();
		}

		list->erase(tmp->second);
		map->erase(k);
	}

	void merge(insertion_ordered_map const &other) {
		list_t* aux_list = list.get();
		map_t* aux_map = map.get();
		try {
			if (!list.unique()) copyList();
			if (!map.unique()) copyMap();
		} catch(std::exception& e) {
			list.reset(aux_list);
			map.reset(aux_map);
			throw e;
		}

		for (auto item : *other.list) {
			insert(item.first, item.second);
		}
	}

	V &at(K const &k) {
		list_t* aux_list = list.get();
		map_t* aux_map = map.get();
		try {
			if (!list.unique()) copyList();
			if (!map.unique()) copyMap();
		} catch(std::exception& e) {
			list.reset(aux_list);
			map.reset(aux_map);
			throw e;
		}
		must_be_copied = true;

		try {
			return map->at(k)->second;
		}
		catch (const std::out_of_range& e) {
			throw lookup_error();
		}
	}

	V const &at(K const &k) const {
		try {
			return map->at(k)->second;
		}
		catch (const std::out_of_range& e) {
			throw lookup_error();
		}
	}

	V &operator[](K const &k) {
		list_t* aux_list = list.get();
		map_t* aux_map = map.get();
		try {
			if (!list.unique()) copyList();
			if (!map.unique()) copyMap();
		} catch(std::exception& e) {
			list.reset(aux_list);
			map.reset(aux_map);
			throw e;
		}
		must_be_copied = true;

		if (map->find(k) == map->end()) {
			insert(k, V());
		}

		return map->at(k)->second;
	}

	[[nodiscard]] size_t size() const {
		return list->size();
	}

	[[nodiscard]] bool empty() const {
		return list->empty();
	}

	void clear() {
		list_t* aux_list = list.get();
		map_t* aux_map = map.get();
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
		} catch(std::exception& e) {
			list.reset(aux_list);
			map.reset(aux_map);
			throw e;
		}
	}

	bool contains(K const &k) const {
		return map->find(k) != map->end();
	}

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

	iterator begin() const {
		iterator iterator;
		iterator.itr = list->cbegin();
		return iterator;
	}

	iterator end() const {
		iterator iterator;
		iterator.itr = list->cend();
		return iterator;
	}
};

#endif //INSERTION_ORDERED_MAP_H