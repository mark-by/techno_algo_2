#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

template<class T>
struct HashFunc;

template<>
struct HashFunc<std::string> {
    size_t operator()(const std::string &data);
};

struct FindResult {
    bool result;
    size_t idx;

    FindResult(bool result, size_t idx)
            : result(result), idx(idx) {}
};

template<class Key, class Hash = HashFunc<Key>>
class Set {
    enum CellType {
        empty, taken, deleted
    };

public:
    Set();

    bool insert(const Key &key);

    bool earse(const Key &key);

    bool isContain(const Key &key);

private:
    void grow();

    bool isLoaded();

    FindResult find(const Key &key);

    size_t doubleHash(const Key &key, size_t idx);

    Hash hash;
    size_t size;
    std::vector<Key> cells;
    std::vector<std::uint8_t> cellDescription;
};

void test(Set<std::string> &hashtable);

int main(int args, char **argv) {
    Set<std::string> set;
    test(set);
    return 0;
}

size_t HashFunc<std::string>::operator()(const std::string &data) {
    size_t hash = 0;

    for (auto symbol : data) {
        hash = (hash * 59 + symbol);
    }
    return hash;
}

template<class Key, class Hash>
Set<Key, Hash>::Set() : size(0) {};

template<class Key, class Hash>
bool Set<Key, Hash>::insert(const Key &key) {
    if (cells.empty() || isLoaded()) {
        grow();
    }

    if (isContain(key)) {
        return false;
    }

    size_t i = 0;
    size_t idx = doubleHash(key, i++);
    while (cellDescription[idx] == taken && i < cells.capacity()) {
        idx = doubleHash(key, i++);
    }
    cells[idx] = key;
    cellDescription[idx] = taken;
    size += 1;
    return true;
}

template<class Key, class Hash>
bool Set<Key, Hash>::earse(const Key &key) {
    FindResult result = find(key);
    if (!result.result) {
        return false;
    }
    cellDescription[result.idx] = deleted;
    size--;
    return true;
}

template<class Key, class Hash>
bool Set<Key, Hash>::isContain(const Key &key) {
    return find(key).result;
}

template<class Key, class Hash>
void Set<Key, Hash>::grow() {
    if (cells.empty()) {
        cells.resize(8);
        cellDescription.resize(8, empty);
        return;
    }

    size_t capacity = cells.capacity();
    std::vector<Key> tempCells = cells;
    cells.clear();
    std::vector<std::uint8_t> tempDescription = cellDescription;
    cellDescription.clear();
    cells.resize(capacity * 2);
    cellDescription.resize(capacity * 2, empty);
    size = 0;
    size_t idx = 0;
    for (auto type : tempDescription) {
        if (type == taken) {
            insert(tempCells[idx]);
        }
        idx++;
    }
}

template<class Key, class Hash>
bool Set<Key, Hash>::isLoaded() {
    return size * 4 >= cells.capacity() * 3;
}

template<class Key, class Hash>
size_t Set<Key, Hash>::doubleHash(const Key &key, size_t idx) {
    return (hash(key) + idx * (hash(key) * 2 + 1)) % cells.capacity();
}

template<class Key, class Hash>
FindResult Set<Key, Hash>::find(const Key &key) {
    if (cells.empty()) {
        return {false, 0};
    }
    size_t i = 0;
    size_t idx = doubleHash(key, i++);
    while (cellDescription[idx] == taken && i < cells.capacity())
    { //попадаем в цикл, если первая проба привела к занятой ячейке
        if (cells[idx] == key)
        { // если занятая ячейка - наша искомая
            return {true, idx}; // то возвращаем положительный ответ с индексом этой ячейки
        }
        idx = doubleHash(key, i++); //иначе делаем следующий шаг
    }
    if (i > 1 || cellDescription[idx] == empty)
    { // если мы вышли из цикла и ничего не нашли, или первая проба привела к пустой ячейке
        return {false, 0}; // то возвращаем отрицательный ответ
    }
    while (cellDescription[idx] == deleted && i < cells.capacity())
    { // если первая проба привела к удаленной ячейке
        idx = doubleHash(key, i++); // делаем шаг, пока не закончатся удаленные ячейки или не превысим количество проб
    }
    if (cellDescription[idx] == taken && cells[idx] == key)
    { //если после удаленных ячеек нашлась занятая с нашим ключом
        return {true, idx};
    }
    return {false, 0};
}

void test(Set<std::string> &hashtable) {
    char operation = '\0';
    std::string key;

    while (std::cin >> operation >> key) {
        bool result = false;
        switch (operation) {
            case '+':
                result = hashtable.insert(key);
                break;
            case '-':
                result = hashtable.earse(key);
                break;
            case '?':
                result = hashtable.isContain(key);
                break;
            default:
                break;
        }
        std::cout << (result ? "OK" : "FAIL") << std::endl;
    }
}
