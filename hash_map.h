#include <vector>
#include <list>
#include <utility>
#include <stdexcept>

// Hashtable with separate chaining
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
  public:
    static constexpr size_t EXPANSION_COEFFICIENT = 2;

    using iterator =
      typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator =
      typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

  private:
    std::vector<std::list<iterator>> table_;
    std::list<std::pair<const KeyType, ValueType>> elements_;
    size_t nElements_, table_size_;
    Hash hasher_;

    // Checks whether load factor equals 1. If so, multiplies table size by 2.
    void expand() {
        if (nElements_ != table_size_) {
            return;
        }
        table_size_ *= EXPANSION_COEFFICIENT;
        table_ = std::vector<std::list<iterator>>(table_size_);
        for (auto i = elements_.begin(); i != elements_.end(); ++i) {
            size_t h = hasher_(i->first) % table_size_;
            table_[h].push_back(i);
        }
    }

  public:
    // Costructor used by default.
    HashMap(Hash hasher = Hash()) :
        nElements_(0),
        table_size_(1),
        hasher_(hasher) {
        table_ = std::vector<std::list<iterator>>(table_size_);
    }

    // Constructor by iterators.
    template<typename Iterator>
    HashMap(Iterator l, Iterator r, Hash hasher = Hash()) :
        nElements_(0),
        table_size_(1),
        hasher_(hasher) {
        table_ = std::vector<std::list<iterator>>(table_size_);
        while (l != r) {
            insert(*l++);
        }
    }

    // Costructor by std::initializer_list.
    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> a,
            Hash hasher = Hash()) : HashMap(a.begin(), a.end(), hasher) {}

    // Returns the number of elements.
    size_t size() const {
        return nElements_;
    }

    // Checks whether hashmap is empty.
    bool empty() const {
        return nElements_ == 0;
    }

    // Returns the hash function.
    Hash hash_function() const {
        return hasher_;
    }

    // Inserts an element if it's not found.
    void insert(const std::pair<const KeyType, ValueType>& a) {
        if (find(a.first) != end()) {
            return;
        }
        size_t h = hasher_(a.first) % table_size_;
        ++nElements_;
        elements_.push_back(a);
        table_[h].push_back(--elements_.end());
        expand();
    }

    // Erases an element.
    void erase(const KeyType& a) {
        size_t h = hasher_(a) % table_size_;
        for (auto i = table_[h].begin(); i != table_[h].end(); ++i) {
            if ((*i)->first == a) {
                elements_.erase(*i);
                table_[h].erase(i);
                --nElements_;
                break;
            }
        }
    }

    // Returns begin interator.
    iterator begin() {
        return elements_.begin();
    }

    // Returns end iterator.
    iterator end() {
        return elements_.end();
    }

    // Returns const begin iterator
    const_iterator begin() const {
        return elements_.cbegin();
    }

    // Returns const end iterator
    const_iterator end() const {
        return elements_.cend();
    }

    // Finds an element and returns an iterator to it.
    iterator find(const KeyType& a) {
        size_t h = hasher_(a) % table_size_;
        for (auto& iter : table_[h]) {
            if (iter->first == a) {
                return iter;
            }
        }
        return end();
    }

    // Finds an element and returns a const iterator to it.
    const_iterator find(const KeyType& a) const {
        size_t h = hasher_(a) % table_size_;
        for (const auto& iter : table_[h]) {
            if (iter->first == a) {
                return iter;
            }
        }
        return end();
    }

    // Gives access to an element by key, if doesn't exist, creates it.
    ValueType& operator[](const KeyType& i) {
        if (find(i) == end()) {
            insert({i, ValueType()});
        }
        return find(i)->second;
    }

    // Gives access to an element by key, if doesn't exist, throws exception.
    const ValueType& at(const KeyType& i) const {
        if (find(i) == end()) {
            throw std::out_of_range("");
        }
        return find(i)->second;
    }

    // Fully clears a hashmap.
    void clear() {
        while (!elements_.empty()) {
            table_[hasher_(elements_.back().first) % table_size_].clear();
            elements_.pop_back();
        }
        nElements_ = 0;
        table_size_ = 1;
    }

    // Copies a hashmap to another.
    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        for (const auto& element : other) {
            insert(element);
        }
        hasher_ = other.hasher_;
        return *this;
    }
};
