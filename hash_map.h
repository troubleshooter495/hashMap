#pragma once

#include <list>
#include <vector>
#include <iterator>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

    explicit HashMap(Hash hasher = Hash()) : hasher(hasher), iters(16) {};

    template<class IteratorType>
    HashMap(IteratorType begin, IteratorType end, Hash hasher = Hash()) : hasher(hasher), iters(16) {
        for (auto &&iter = begin; iter != end; iter++) {
            insert(*iter);
        }
    };

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, Hash hasher = Hash()) :
            HashMap(list.begin(), list.end()) {};

    uint64_t size() const {
        return elements_num;
    };

    bool empty() const {
        return elements_num == 0;
    };

    Hash hash_function() const {
        return hasher;
    };

    iterator insert(std::pair<KeyType, ValueType> to_insert) {
        const uint64_t hashed_key = hasher(to_insert.first);
        if (find(to_insert.first) != data.end()) {
            return find(to_insert.first);
        }
        if (iters[hashed_key % iters.size()].size() == bucket_size) {
            expand();
        }
        data.push_back(to_insert);
        iters[hashed_key % iters.size()].push_back(std::prev(data.end(), 1));
        elements_num++;
        return std::prev(data.end(), 1);
    };

    void erase(KeyType key) {
        const uint64_t hashed_key = hasher(key);
        auto possible_iter = iters[hashed_key % iters.size()].begin();
        for (const auto iter : iters[hashed_key % iters.size()]) {
            if (iter->first == key) {
                iters[hashed_key % iters.size()].erase(possible_iter);
                data.erase(iter);
                elements_num--;
                break;
            }
            possible_iter++;
        }
    };

    iterator begin() noexcept {
        return data.begin();
    }

    iterator end() noexcept {
        return data.end();
    }

    const_iterator begin() const noexcept {
        return data.begin();
    }

    const_iterator end() const noexcept {
        return data.end();
    }

    iterator find(KeyType key) noexcept {
        const uint64_t hashed_key = hasher(key);
        for (const auto &iter : iters[hashed_key % iters.size()]) {
            if (iter->first == key) {
                return iter;
            }
        }
        return data.end();
    };

    const_iterator find(KeyType key) const noexcept {
        const uint64_t hashed_key = hasher(key);
        for (const auto &iter : iters[hashed_key % iters.size()]) {
            if (iter->first == key) {
                return iter;
            }
        }
        return data.end();
    };

    ValueType &operator[](const KeyType key) {
        auto found = insert({key, {}});
        return found->second;
    };

    const ValueType &at(KeyType &&key) const {
        const auto &&found = find(key);
        if (found == data.end()) {
            throw std::out_of_range("out of range");
        }
        return found->second;
    }

    void clear() {
        iters.clear();
        iters.resize(default_size);
        data.clear();
        elements_num = 0;
    }

    HashMap &operator=(const HashMap &to_compare) noexcept {
        if (this == &to_compare) {
            return *this;
        }
        clear();
        for (const auto &iter : to_compare) {
            insert(iter);
        }
        return *this;
    };

private:
    Hash hasher;
    std::list<std::pair<const KeyType, ValueType>> data;
    std::vector<std::list<iterator>> iters;
    static constexpr uint64_t default_size = 16;
    static constexpr uint64_t bucket_size = 4;
    uint64_t elements_num = 0;

    void expand() {
        const uint64_t new_size = iters.size() * 2;
        iters.clear();
        iters.resize(new_size);
        for (auto &&iter = data.begin(); iter != data.end(); ++iter) {
            iters[hasher(iter->first) % iters.size()].push_back(iter);
        }
    };
};
