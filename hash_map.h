#include <vector>
#include <list>
#include <utility>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    public:
    using iterator =
    typename std::list<std::pair <const KeyType, ValueType>>::iterator;
    using const_iterator =
    typename std::list<std::pair <const KeyType, ValueType>>::const_iterator;

    private:
    const size_t TABLE_SIZE = 8e5;
    std::vector<std::list<iterator>> table_;
    std::list<std::pair<const KeyType, ValueType>> elements_;
    size_t nElements_;
    Hash hasher_;

    public:
    HashMap(Hash hasher = Hash()) :
        table_(std::vector<std::list<iterator>>(TABLE_SIZE)),
        nElements_(0),
        hasher_(hasher) {}

    template<typename It>
    HashMap(It l, It r, Hash hasher = Hash()) :
        table_(std::vector<std::list<iterator>>(TABLE_SIZE)),
        nElements_(0),
        hasher_(hasher) {
        while (l != r)
            insert(*l++);
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> a,
            Hash hasher = Hash()) : HashMap(a.begin(), a.end(), hasher) {}

    size_t size() const {
        return nElements_;
    }

    bool empty() const {
        return nElements_ == 0;
    }

    Hash hash_function() const {
        return hasher_;
    }

    void insert(const std::pair<const KeyType, ValueType>& a) {
        size_t h = hasher_(a.first) % TABLE_SIZE;
        for (const auto& i : table_[h])
            if (i -> first == a.first)
                return;
        ++nElements_;
        elements_.push_back(a);
        table_[h].push_back(--elements_.end());
    }

    void erase(const KeyType& a) {
        size_t h = hasher_(a) % TABLE_SIZE;
        for (auto i = table_[h].begin(); i != table_[h].end(); ++i) {
            if ((*i) -> first == a) {
                elements_.erase(*i);
                table_[h].erase(i);
                --nElements_;
                break;
            }
        }
    }

    iterator begin() {
        return elements_.begin();
    }

    iterator end() {
        return elements_.end();
    }

    const_iterator begin() const {
        return elements_.cbegin();
    }

    const_iterator end() const {
        return elements_.cend();
    }

    iterator find(const KeyType& a) {
        size_t h = hasher_(a) % TABLE_SIZE;
        for (auto& i : table_[h])
            if (i -> first == a)
                return i;
        return end();
    }

    const_iterator find(const KeyType& a) const {
        size_t h = hasher_(a) % TABLE_SIZE;
        for (const auto& i : table_[h])
            if (i -> first == a)
                return i;
        return end();
    }

    ValueType& operator[](const KeyType& i) {
        if (find(i) == end())
            insert({i, ValueType()});
        return find(i) -> second;
    }

    const ValueType& at(const KeyType& i) const {
        if (find(i) == end())
            throw std::out_of_range("");
        return find(i) -> second;
    }

    void clear() {
        while (!elements_.empty()) {
            table_[hasher_(elements_.back().first) % TABLE_SIZE].clear();
            elements_.pop_back();
        }
        nElements_ = 0;
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other)
            return *this;
        clear();
        for (const auto& i : other)
            insert(i);
        nElements_ = other.nElements_;
        hasher_ = other.hasher_;
        return *this;
    }
};
