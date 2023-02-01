//
// Created by Никита on 23.11.2022.
//

#ifndef INC_1_MAP_H
#define INC_1_MAP_H

#pragma once

#include <exception>
#include <sstream>
#include <fstream>
#include "BinaryTree.h"
#include "Pair.h"

namespace lab {
    template<typename KeyType, typename ValueType>
    class Map;

    template<typename KeyType, typename ValueType>
    class _BinaryTreeIterator {
    private:
        friend class Map<KeyType, ValueType>;

        const Node<Pair<KeyType, ValueType>>* current = nullptr;
        const Node<Pair<KeyType, ValueType>>* tree = nullptr;

    public:
        _BinaryTreeIterator(const Node<Pair<KeyType, ValueType>>* cur,
            const Node<Pair<KeyType, ValueType>>* tr)
            : current(cur), tree(tr) {}

        _BinaryTreeIterator() = default;
        _BinaryTreeIterator(const _BinaryTreeIterator<KeyType, ValueType>& other) : current(other.current), tree(other.tree) {}

        _BinaryTreeIterator& operator++();
        _BinaryTreeIterator operator++(int);

        _BinaryTreeIterator operator--();
        _BinaryTreeIterator operator--(int);

        bool operator==(const _BinaryTreeIterator& other);
        bool operator!=(const _BinaryTreeIterator& other);

        const Pair<KeyType, ValueType>& operator*(); // return current->data
    };

    template<typename KeyType, typename ValueType>
    inline _BinaryTreeIterator<KeyType, ValueType>& _BinaryTreeIterator<KeyType, ValueType>::operator++()
    {
        // Текущее положение *end()* 
        if (!current) {
            current = tree;
            if (!current) throw std::exception("tree is empty");
            while (current->left) current = current->left;
        }
        else {
            // Самое маленькое значение соседнего элемента
            if (current->right) {
                current = current->right;
                while (current->left) current = current->left;
            }
            // ?
            else {
                Node<Pair<KeyType, ValueType>>* par;
                par = current->parent;
                while (par && current == par->right) {
                    current = par;
                    par = par->parent;
                }
                current = par;
            }
        }
        return *this;
    }

    template<typename KeyType, typename ValueType>
    inline _BinaryTreeIterator<KeyType, ValueType> _BinaryTreeIterator<KeyType, ValueType>::operator++(int) {
        auto saved = *this;
        operator++();
        return saved;
    }

    template<typename KeyType, typename ValueType>
    inline _BinaryTreeIterator<KeyType, ValueType> _BinaryTreeIterator<KeyType, ValueType>::operator--()
    {
        if (!current) {
            current = tree->root;
            if (!current) throw std::exception("tree Iteratorator operator--: tree empty");
            while (current->right) current = current->right;
        }
        else if (current->left) {
            current = current->left;
            while (current->right) current = current->right;
        }
        else {
            Node<Pair<KeyType, ValueType>>* par;
            par = current->parent;
            while (par && current == par->left) {
                current = par;
                par = par->parent;
            }
            current = par;
        }
        return *this;
    }

    template<typename KeyType, typename ValueType>
    inline _BinaryTreeIterator<KeyType, ValueType> _BinaryTreeIterator<KeyType, ValueType>::operator--(int)
    {
        auto saved = *this;
        operator--();
        return saved;
    }

    template<typename KeyType, typename ValueType>
    inline bool _BinaryTreeIterator<KeyType, ValueType>::operator==(const _BinaryTreeIterator& other) {
        return tree == other.tree && current == other.current;
    }

    template<typename KeyType, typename ValueType>
    inline bool _BinaryTreeIterator<KeyType, ValueType>::operator!=(const _BinaryTreeIterator& other) {
        return tree != other.tree || current != other.current;
    }

    template<typename KeyType, typename ValueType>
    inline const Pair<KeyType, ValueType>& _BinaryTreeIterator<KeyType, ValueType>::operator*() {
        if (!current) throw std::exception("null pointer dereference");
        return current->data;
    }

//_________________________________________________________MAP______________________________________________________

    template<typename KeyType, typename ValueType>
    class Map {
    private:
        BinaryTree<Pair<KeyType, ValueType>>* rootMap = nullptr;
        mutable std::mutex locker;
    public:
        using Iterator = _BinaryTreeIterator<KeyType, ValueType>;

        Map();
        ~Map();

        Map(const Map<KeyType, ValueType>& other);
        Map(Map<KeyType, ValueType>&& other);
        Map(const std::initializer_list<Pair<KeyType, ValueType>> list);

        // DEBUG
        void Print() {
            std::cout << *rootMap;
        }

        inline Iterator begin() const noexcept { 
            std::lock_guard<std::mutex> lock(locker); 
            return Iterator(rootMap->findMin(), rootMap->getRoot()); 
        }

        inline Iterator end() const noexcept{ 
            std::lock_guard<std::mutex> lock(locker);
            return Iterator(nullptr, rootMap->getRoot()); 
        }

        bool insert(const Pair<KeyType, ValueType> element); // Принимает пару
        bool emplace(const KeyType key, const ValueType value); // Принимает значения

        bool erase(const KeyType key) noexcept; // <-- + перегрузка Pair<KeyType, ValueType> + перегрузка (Iterator, Iterator)
        void clear() noexcept;

        const Node<Pair<KeyType, ValueType>>& at(const KeyType) const;
        const Node<Pair<KeyType, ValueType>> value(const KeyType, const ValueType) const noexcept;
        const Pair<KeyType, ValueType>& operator[](int index) const noexcept;

        inline int size() const noexcept { return rootMap->getSize(); }
        std::string keys() const noexcept;
        std::string toJSON();

        bool operator==(const Map<KeyType, ValueType>&) const noexcept;
        bool operator!=(const Map<KeyType, ValueType>&) const noexcept;
    };

    template<typename KeyType, typename ValueType>
    inline Map<KeyType, ValueType>::Map() {
        std::lock_guard<std::mutex> lock(locker);
        rootMap = new BinaryTree<Pair<KeyType, ValueType>>;
    }

    template<typename KeyType, typename ValueType>
    inline Map<KeyType, ValueType>::~Map() { 
        std::lock_guard<std::mutex> lock(locker);
        delete rootMap; 
    }

    template<typename KeyType, typename ValueType>
    inline Map<KeyType, ValueType>::Map(const Map<KeyType, ValueType>& other)
    {
        rootMap->copyTree(other.rootMap);
    }

    template<typename KeyType, typename ValueType>
    inline Map<KeyType, ValueType>::Map(Map<KeyType, ValueType>&& other)
    {
        rootMap->copyTree(other.rootMap);
        other.rootMap->clear();
    }

    template<typename KeyType, typename ValueType>
    inline Map<KeyType, ValueType>::Map(const std::initializer_list<Pair<KeyType, ValueType>> list)
    {
        for (auto elem : list) this->insert(elem);
    }

    template<typename KeyType, typename ValueType>
    inline bool Map<KeyType, ValueType>::insert(const Pair<KeyType, ValueType> element) {
        std::lock_guard<std::mutex> lock(locker);
        return rootMap->add(element);
    }

    template<typename KeyType, typename ValueType>
    inline bool Map<KeyType, ValueType>::emplace(const KeyType key, const ValueType value) {
        std::lock_guard<std::mutex> lock(locker);
        return rootMap->add(Pair<KeyType, ValueType>(key, value));
    }

    template<typename KeyType, typename ValueType>
    inline bool Map<KeyType, ValueType>::erase(const KeyType key) noexcept{
        std::lock_guard<std::mutex> lock(locker);
        return rootMap->deleteElement(Pair<KeyType, ValueType>(key));
    }
    template<typename KeyType, typename ValueType>
    inline void Map<KeyType, ValueType>::clear() noexcept
    {
        std::lock_guard<std::mutex> lock(locker);
        rootMap->clear();
    }
    template<typename KeyType, typename ValueType>
    inline const Node<Pair<KeyType, ValueType>>& Map<KeyType, ValueType>::at(const KeyType key) const
    {
        for (auto elem : *this) if (elem.first == key) return elem;
        throw std::exception("value not found");
    }
    template<typename KeyType, typename ValueType>
    inline const Node<Pair<KeyType, ValueType>> Map<KeyType, ValueType>::value(const KeyType key, const ValueType defvalue) const noexcept
    {
        for (auto elem : *this) if (elem.first == key) return elem;
        return Node<Pair<KeyType, ValueType>>(key, defvalue);
    }
    template<typename KeyType, typename ValueType>
    inline const Pair<KeyType, ValueType>& Map<KeyType, ValueType>::operator[](int index) const noexcept
    {
        auto iter = (*this).begin();
        for (size_t i = 0; i < index; ++i) ++iter;
        return *iter;
    }
    template<typename KeyType, typename ValueType>
    inline std::string Map<KeyType, ValueType>::keys() const noexcept
    {
        std::string res;
        for (auto elem : *this) res += elem.first + " ";
        return res;
    }
    template<typename KeyType, typename ValueType>
    inline std::string Map<KeyType, ValueType>::toJSON() 
    {
        std::ostringstream outFlow;
        outFlow << "{\n\trootMap: {\n";
        for (auto elem : *this) outFlow << "\t\t\"" << elem.first << "\":\"" << elem.second << "\"\n";
        outFlow << "\t}\n}";
        return outFlow.str();
    }
    template<typename KeyType, typename ValueType>
    inline bool Map<KeyType, ValueType>::operator==(const Map<KeyType, ValueType>& other) const noexcept
    {
        if (rootMap->getSize() != other.rootMap->getSize()) return false;
        return rootMap == other.rootMap;
    }
    template<typename KeyType, typename ValueType>
    inline bool Map<KeyType, ValueType>::operator!=(const Map<KeyType, ValueType>& other) const noexcept
    {
        return rootMap != other.rootMap;
    }
}

#endif //INC_1_MAP_H
