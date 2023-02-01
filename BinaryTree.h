//
// Created by Никита on 23.11.2022.
//

#ifndef INC_1_BINARYTREE_H
#define INC_1_BINARYTREE_H

#include <mutex>

namespace lab {
    template<typename NodeType>
    struct Node {
        NodeType data;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
    };
    template<typename BinaryType>
    class BinaryTree {
    private:
        Node<BinaryType>* root = nullptr;
        int size = 0;
        mutable std::mutex locker;

        bool _addInTree(Node<BinaryType>*& tRoot, const BinaryType& elem, Node<BinaryType>* previous);
        void _copyTree(const Node<BinaryType>* const tRoot);
        static void _symmetricalOut(std::ostream& out, const Node<BinaryType>* const tRoot);
        bool _isEqual(const Node<BinaryType>* current, const Node<BinaryType>* other) const noexcept;
        void _clearTree(Node<BinaryType>*& tRoot);
    public:
        BinaryTree() = default;
        ~BinaryTree();
        BinaryTree(const BinaryTree<BinaryType>& other);
        BinaryTree(BinaryTree<BinaryType>&& other);
        BinaryTree(const std::initializer_list<BinaryType> list);

        template<typename BinaryTypeOut> friend std::ostream& operator<<(std::ostream& out, const BinaryTree<BinaryTypeOut>& tree);

        bool add(const BinaryType);
        void copyTree(const BinaryTree<BinaryType>& _Source);

        bool deleteElement(const BinaryType) noexcept;
        void clear();

        const BinaryTree<BinaryType>* getTree() const noexcept { return this; }
        const Node<BinaryType>* getRoot() const noexcept { return root; }
        int getSize() const noexcept{ return size; }

        const Node<BinaryType>* findMin() const;
        const Node<BinaryType>* findMax() const;

        bool operator==(const BinaryTree<BinaryType>& other) const noexcept;
        bool operator!=(const BinaryTree<BinaryType>& other) const noexcept;
    };

    template<typename BinaryType>
    inline bool
        BinaryTree<BinaryType>::_addInTree(Node<BinaryType>*& tRoot, const BinaryType& elem, Node<BinaryType>* previous) {
        if (!tRoot) {
            tRoot = new Node<BinaryType>;
            tRoot->data = elem;
            tRoot->parent = previous;
            ++size;
            return true;
        }
        else {
            if (elem < tRoot->data) return _addInTree(tRoot->left, elem, tRoot);
            else if (elem > tRoot->data) return _addInTree(tRoot->right, elem, tRoot);
            else return false;
        }
    }

    template<typename BinaryType>
    inline void BinaryTree<BinaryType>::_copyTree(const Node<BinaryType>* const tRoot)
    {
        if (tRoot->left) _copyTree(tRoot->left);
        this->add(tRoot->data);
        if (tRoot->right) _copyTree(tRoot->right);
    }

    template<typename BinaryType>
    inline void BinaryTree<BinaryType>::_symmetricalOut(std::ostream& out, const Node<BinaryType>* const tRoot) {
        if (tRoot != nullptr) {
            _symmetricalOut(out, tRoot->left);
            out << tRoot->data << " ";
            _symmetricalOut(out, tRoot->right);
        }
    }

    template<typename BinaryType>
    inline bool BinaryTree<BinaryType>::_isEqual(const Node<BinaryType>* current, const Node<BinaryType>* other) const noexcept
    {
        if (!current && other || current && !other) { return false; }
        if (!current && !other) { return true; }

        if (current->data != other->data) { return false; }
        return  _isEqual(current->left, other->left) && _isEqual(current->right, other->right);
    }

    template<typename BinaryType>
    inline void BinaryTree<BinaryType>::_clearTree(Node<BinaryType>*& tRoot) {
        if (!tRoot) return;
        _clearTree(tRoot->left);
        _clearTree(tRoot->right);

        delete tRoot;
        tRoot = nullptr;
    }

    template<typename T>
    inline BinaryTree<T>::BinaryTree(const BinaryTree<T>& other) {
        std::lock_guard<std::mutex> lock(locker);
        _clearTree(&root);
        _copyTree(other.root);
    }

    template<typename T>
    inline BinaryTree<T>::BinaryTree(BinaryTree<T>&& other) {
        std::lock_guard<std::mutex> lock(locker);
        root = other.root;
        other.root = nullptr;
    }

    template<typename T>
    inline BinaryTree<T>::BinaryTree(const std::initializer_list<T> list) {
        locker.lock();
        _clearTree(&root);
        locker.unlock();
        for (auto elem : list) add(elem);
    }

    template<typename T>
    inline BinaryTree<T>::~BinaryTree() {
        std::lock_guard<std::mutex> lock(locker);
        _clearTree(root);
    }

    template<typename BinaryTypeOut>
    std::ostream& operator<<(std::ostream& out, const BinaryTree<BinaryTypeOut>& tree) {
        // std::lock_guard<std::mutex> lock(tree.locker);  //<---------- ?
        if (tree.root) tree._symmetricalOut(out, tree.root);
        return out;
    }

    template<typename BinaryType>
    inline bool BinaryTree<BinaryType>::add(const BinaryType elem) {
        std::lock_guard<std::mutex> lock(locker);
        return _addInTree(root, elem, nullptr);
    }

    template<typename BinaryType>
    inline void BinaryTree<BinaryType>::copyTree(const BinaryTree<BinaryType>& _Source)
    {
        this->clear();
        this->_copyTree(_Source);
    }

    template<typename BinaryType>
    inline bool BinaryTree<BinaryType>::deleteElement(const BinaryType elem) noexcept {
        std::lock_guard<std::mutex> lock(locker);
        Node<BinaryType>** ptr = &(root);

        while (*ptr) {
            if (elem == (*ptr)->data) break;
            else if (elem > (*ptr)->data) ptr = &((*ptr)->right);
            else if (elem < (*ptr)->data) ptr = &((*ptr)->left);
        }

        // not found
        if (!(*ptr)) {
            return false;
        }

        // ни одного следующего элемента после
        if (!((*ptr)->left) && !((*ptr)->right)) {
            delete (*ptr);
            (*ptr) = nullptr;
        }
        // с двух сторон есть элементы
        else if (((*ptr)->left) && ((*ptr)->right)) {
            Node<BinaryType>** maxEl = &((*ptr)->left);
            if (!((*maxEl)->right)) {
                if ((*maxEl)->left) {
                    Node<BinaryType>* nextPosition = (*maxEl)->left;
                    (*ptr)->data = (*maxEl)->data;
                    delete (*maxEl);
                    *maxEl = nextPosition;
                    nextPosition->parent = *ptr;
                }
                else {
                    (*ptr)->data = (*maxEl)->data;
                    delete (*maxEl);
                    *maxEl = nullptr;
                }
            }
            else {
                while ((*maxEl)->right) maxEl = &((*maxEl)->right);
                (*ptr)->data = (*maxEl)->data;
                (*ptr)->parent = (*maxEl)->parent;
                delete (*maxEl);
                (*maxEl) = nullptr;
            }
        }
        // с одной стороны элемент
        else {
            Node<BinaryType>* temp = ((*ptr)->left ? (*ptr)->left : (*ptr)->right);
            temp->parent = (*ptr)->parent;
            delete (*ptr);
            *ptr = temp;
        }

        --size;
        return true;
    }

    template<typename BinaryType>
    inline void BinaryTree<BinaryType>::clear() {
        std::lock_guard<std::mutex> lock(locker);
        _clearTree(root);
        size = 0;
    }

    template<typename BinaryType>
    inline const Node<BinaryType>* BinaryTree<BinaryType>::findMin() const
    {
        std::lock_guard<std::mutex> lock(locker);
        if (!root) return nullptr;
        Node<BinaryType>* temp = root;
        while (temp->left) temp = temp->left;
        return temp;
    }

    template<typename BinaryType>
    inline const Node<BinaryType>* BinaryTree<BinaryType>::findMax() const {
        std::lock_guard<std::mutex> lock(locker);
        if (!root) return nullptr;
        Node<BinaryType>* temp = root;
        while (temp->right) temp = temp->right;
        return temp;
    }

    template<typename BinaryType>
    inline bool BinaryTree<BinaryType>::operator==(const BinaryTree<BinaryType>& other) const noexcept
    {
        std::lock_guard<std::mutex> lock(locker);
        bool result = _isEqual(root, other.root);
        return result;
    }

    template<typename BinaryType>
    inline bool BinaryTree<BinaryType>::operator!=(const BinaryTree<BinaryType>& other) const noexcept
    {
        std::lock_guard<std::mutex> lock(locker);
        bool result = !_isEqual(root, other.root);
        return result;
    }

} // namespace lab

#endif //INC_1_BINARYTREE_H
