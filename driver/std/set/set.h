#pragma once

#include "../memory/memory.h"
#include "../memory/pair.h"

// Custom Less comparator
template <typename T>
struct Less {
public:
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

// ReverseIterator implementation
template <typename Iterator>
class ReverseIterator {
public:
    using iterator_type = Iterator;
    using difference_type = typename Iterator::difference_type;
    using reference = typename Iterator::reference;
    using pointer = typename Iterator::pointer;

    ReverseIterator() : current_() {}
    explicit ReverseIterator(iterator_type x) : current_(x) {}
    ReverseIterator(const ReverseIterator& other) : current_(other.current_) {}

    iterator_type base() const { return current_; }

    reference operator*() const {
        iterator_type tmp = current_;
        --tmp;
        return *tmp;
    }

    pointer operator->() const { return &(operator*()); }

    ReverseIterator& operator++() {
        --current_;
        return *this;
    }

    ReverseIterator operator++(int) {
        ReverseIterator tmp = *this;
        --current_;
        return tmp;
    }

    ReverseIterator& operator--() {
        ++current_;
        return *this;
    }

    ReverseIterator operator--(int) {
        ReverseIterator tmp = *this;
        ++current_;
        return tmp;
    }

    bool operator==(const ReverseIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const ReverseIterator& other) const {
        return current_ != other.current_;
    }

private:
    iterator_type current_;
};

// Set class definition
template <typename T, typename Compare = Less<T>>
class Set {
public:
    // Member types
    using key_type = T;
    using value_type = T;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = MemoryAllocator<value_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = ptrdiff_t;
    using size_type = size_t;

    // Iterator class
    class Iterator;
    using iterator = Iterator;
    using const_iterator = const Iterator;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

    // Constructors
    Set();
    explicit Set(const Compare& comp);

    // Copy constructor
    Set(const Set& other);

    // Assignment operator
    Set& operator=(const Set& other);

    // Destructor
    ~Set();

    // Iterators
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    // Capacity
    bool empty() const;
    size_type size() const;
    size_type max_size() const;

    // Modifiers
    Pair<iterator, bool> insert(const value_type& value);
    iterator erase(iterator pos);
    size_type erase(const key_type& key);
    void swap(Set& other);
    void clear();
    template <typename... Args>
    Pair<iterator, bool> emplace(Args&&... args);
    template <typename... Args>
    iterator emplace_hint(iterator hint, Args&&... args);

    // Observers
    key_compare key_comp() const;
    value_compare value_comp() const;

    // Operations
    iterator find(const key_type& key);
    const_iterator find(const key_type& key) const;
    size_type count(const key_type& key) const;
    iterator lower_bound(const key_type& key);
    const_iterator lower_bound(const key_type& key) const;
    iterator upper_bound(const key_type& key);
    const_iterator upper_bound(const key_type& key) const;
    Pair<iterator, iterator> equal_range(const key_type& key);

protected:
    // Allocator functions
    struct Node* AllocateNode(const value_type& value)
    {
		Node* node = new Node(value);
		return node;
    }
    void DeallocateNode(struct Node* node)
    {
		delete node;
		node = nullptr;
    }

private:
    // Node structure
    struct Node {
        value_type data;
        Node* left;
        Node* right;
        Node* parent;

        explicit Node(const value_type& val)
            : data(val), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node* root_;
    size_type size_;
    key_compare comp_;

    // Helper functions
    void Destroy(Node* node);
    Node* CopyTree(Node* other_root, Node* parent);
    void Transplant(Node* u, Node* v);
    Node* Minimum(Node* node) const;
    Node* Maximum(Node* node) const;

public:
    // Iterator implementation
    class Iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = T;
        using reference = value_type&;
        using pointer = value_type*;

        Iterator();
        Iterator(Node* node, Node* root);
        Iterator(const Iterator& other);

        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);
        reference operator*() const;
        pointer operator->() const;
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

    private:
        Node* node_;
        Node* root_;

        friend class Set;
    };
};

// Implementations

// Constructors
template <typename T, typename Compare>
Set<T, Compare>::Set() : root_(nullptr), size_(0), comp_(Compare()) {}

template <typename T, typename Compare>
Set<T, Compare>::Set(const Compare& comp)
    : root_(nullptr), size_(0), comp_(comp) {}

template <typename T, typename Compare>
Set<T, Compare>::Set(const Set& other)
    : root_(nullptr), size_(0), comp_(other.comp_) {
    root_ = CopyTree(other.root_, nullptr);
    size_ = other.size_;
}

template <typename T, typename Compare>
Set<T, Compare>::~Set() {
    clear();
}

template <typename T, typename Compare>
Set<T, Compare>& Set<T, Compare>::operator=(const Set& other) {
    if (this != &other) {
        clear();
        comp_ = other.comp_;
        root_ = CopyTree(other.root_, nullptr);
        size_ = other.size_;
    }
    return *this;
}

// Iterators
template <typename T, typename Compare>
typename Set<T, Compare>::iterator Set<T, Compare>::begin() {
    return iterator(Minimum(root_), root_);
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_iterator Set<T, Compare>::begin() const {
    return const_iterator(Minimum(root_), root_);
}

template <typename T, typename Compare>
typename Set<T, Compare>::iterator Set<T, Compare>::end() {
    return iterator(nullptr, root_);
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_iterator Set<T, Compare>::end() const {
    return const_iterator(nullptr, root_);
}

template <typename T, typename Compare>
typename Set<T, Compare>::reverse_iterator Set<T, Compare>::rbegin() {
    return reverse_iterator(end());
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_reverse_iterator Set<T, Compare>::rbegin() const {
    return const_reverse_iterator(end());
}

template <typename T, typename Compare>
typename Set<T, Compare>::reverse_iterator Set<T, Compare>::rend() {
    return reverse_iterator(begin());
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_reverse_iterator Set<T, Compare>::rend() const {
    return const_reverse_iterator(begin());
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_iterator Set<T, Compare>::cbegin() const {
    return begin();
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_iterator Set<T, Compare>::cend() const {
    return end();
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_reverse_iterator Set<T, Compare>::crbegin() const {
    return rbegin();
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_reverse_iterator Set<T, Compare>::crend() const {
    return rend();
}

// Capacity
template <typename T, typename Compare>
bool Set<T, Compare>::empty() const {
    return size_ == 0;
}

template <typename T, typename Compare>
typename Set<T, Compare>::size_type Set<T, Compare>::size() const {
    return size_;
}

template <typename T, typename Compare>
typename Set<T, Compare>::size_type Set<T, Compare>::max_size() const {
    return static_cast<size_type>(-1) / sizeof(Node);
}

// Modifiers
template <typename T, typename Compare>
Pair<typename Set<T, Compare>::iterator, bool> Set<T, Compare>::insert(
    const value_type& value) {
    Node* y = nullptr;
    Node* x = root_;

    while (x != nullptr) {
        y = x;
        if (comp_(value, x->data)) {
            x = x->left;
        }
        else if (comp_(x->data, value)) {
            x = x->right;
        }
        else {
            // Duplicate key, insertion failed
            return MakePair(iterator(x, root_), false);
        }
    }

    Node* z = AllocateNode(value);
    z->parent = y;

    if (y == nullptr) {
        root_ = z;
    }
    else if (comp_(z->data, y->data)) {
        y->left = z;
    }
    else {
        y->right = z;
    }

    ++size_;
    return MakePair(iterator(z, root_), true);
}

template <typename T, typename Compare>
typename Set<T, Compare>::iterator Set<T, Compare>::erase(iterator pos) {
    if (pos == end()) {
        return pos;
    }

    Node* z = pos.node_;
    iterator next = pos;
    ++next;

    if (z->left == nullptr) {
        Transplant(z, z->right);
    }
    else if (z->right == nullptr) {
        Transplant(z, z->left);
    }
    else {
        Node* y = Minimum(z->right);
        if (y->parent != z) {
            Transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        Transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
    }

    DeallocateNode(z);
    --size_;

    return next;
}

template <typename T, typename Compare>
typename Set<T, Compare>::size_type Set<T, Compare>::erase(const key_type& key) {
    iterator it = find(key);
    if (it != end()) {
        erase(it);
        return 1;
    }
    return 0;
}

template <typename T, typename Compare>
void Set<T, Compare>::swap(Set& other) {
    Swap(root_, other.root_);
    Swap(size_, other.size_);
    Swap(comp_, other.comp_);
}

template <typename T, typename Compare>
void Set<T, Compare>::clear() {
    Destroy(root_);
    root_ = nullptr;
    size_ = 0;
}

template <typename T, typename Compare>
template <typename... Args>
Pair<typename Set<T, Compare>::iterator, bool> Set<T, Compare>::emplace(Args&&... args) {
    value_type value(args...);
    return insert(value);
}

template <typename T, typename Compare>
template <typename... Args>
typename Set<T, Compare>::iterator Set<T, Compare>::emplace_hint(iterator, Args&&... args) {
    value_type value(args...);
    return insert(value).first;
}

// Observers
template <typename T, typename Compare>
typename Set<T, Compare>::key_compare Set<T, Compare>::key_comp() const {
    return comp_;
}

template <typename T, typename Compare>
typename Set<T, Compare>::value_compare Set<T, Compare>::value_comp() const {
    return comp_;
}

// Operations
template <typename T, typename Compare>
typename Set<T, Compare>::iterator Set<T, Compare>::find(const key_type& key) {
    Node* node = root_;
    while (node != nullptr) {
        if (comp_(key, node->data)) {
            node = node->left;
        }
        else if (comp_(node->data, key)) {
            node = node->right;
        }
        else {
            return iterator(node, root_);
        }
    }
    return end();
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_iterator Set<T, Compare>::find(const key_type& key) const {
    Node* node = root_;
    while (node != nullptr) {
        if (comp_(key, node->data)) {
            node = node->left;
        }
        else if (comp_(node->data, key)) {
            node = node->right;
        }
        else {
            return const_iterator(node, root_);
        }
    }
    return end();
}

template <typename T, typename Compare>
typename Set<T, Compare>::size_type Set<T, Compare>::count(const key_type& key) const {
    return find(key) != end() ? 1 : 0;
}

template <typename T, typename Compare>
typename Set<T, Compare>::iterator Set<T, Compare>::lower_bound(const key_type& key) {
    Node* node = root_;
    Node* result = nullptr;
    while (node != nullptr) {
        if (!comp_(node->data, key)) {
            result = node;
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    return iterator(result, root_);
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_iterator Set<T, Compare>::lower_bound(const key_type& key) const {
    Node* node = root_;
    Node* result = nullptr;
    while (node != nullptr) {
        if (!comp_(node->data, key)) {
            result = node;
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    return const_iterator(result, root_);
}

template <typename T, typename Compare>
typename Set<T, Compare>::iterator Set<T, Compare>::upper_bound(const key_type& key) {
    Node* node = root_;
    Node* result = nullptr;
    while (node != nullptr) {
        if (comp_(key, node->data)) {
            result = node;
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    return iterator(result, root_);
}

template <typename T, typename Compare>
typename Set<T, Compare>::const_iterator Set<T, Compare>::upper_bound(const key_type& key) const {
    Node* node = root_;
    Node* result = nullptr;
    while (node != nullptr) {
        if (comp_(key, node->data)) {
            result = node;
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    return const_iterator(result, root_);
}

template <typename T, typename Compare>
Pair<typename Set<T, Compare>::iterator, typename Set<T, Compare>::iterator>
Set<T, Compare>::equal_range(const key_type& key) {
    return MakePair(lower_bound(key), upper_bound(key));
}

// Helper functions
template <typename T, typename Compare>
void Set<T, Compare>::Destroy(Node* node) {
    if (node != nullptr) {
        Destroy(node->left);
        Destroy(node->right);
        DeallocateNode(node);
    }
}

template <typename T, typename Compare>
typename Set<T, Compare>::Node* Set<T, Compare>::CopyTree(Node* other_root, Node* parent) {
    if (other_root == nullptr) {
        return nullptr;
    }
    Node* node = AllocateNode(other_root->data);
    node->parent = parent;
    node->left = CopyTree(other_root->left, node);
    node->right = CopyTree(other_root->right, node);
    return node;
}

template <typename T, typename Compare>
void Set<T, Compare>::Transplant(Node* u, Node* v) {
    if (u->parent == nullptr) {
        root_ = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v != nullptr) {
        v->parent = u->parent;
    }
}

template <typename T, typename Compare>
typename Set<T, Compare>::Node* Set<T, Compare>::Minimum(Node* node) const {
    while (node && node->left != nullptr) {
        node = node->left;
    }
    return node;
}

template <typename T, typename Compare>
typename Set<T, Compare>::Node* Set<T, Compare>::Maximum(Node* node) const {
    while (node && node->right != nullptr) {
        node = node->right;
    }
    return node;
}

// Iterator implementation
template <typename T, typename Compare>
Set<T, Compare>::Iterator::Iterator() : node_(nullptr), root_(nullptr) {}

template <typename T, typename Compare>
Set<T, Compare>::Iterator::Iterator(Node* node, Node* root)
    : node_(node), root_(root) {}

template <typename T, typename Compare>
Set<T, Compare>::Iterator::Iterator(const Iterator& other)
    : node_(other.node_), root_(other.root_) {}

template <typename T, typename Compare>
typename Set<T, Compare>::Iterator& Set<T, Compare>::Iterator::operator++() {
    if (node_ != nullptr) {
        if (node_->right != nullptr) {
            node_ = Minimum(node_->right);
        }
        else {
            Node* parent = node_->parent;
            while (parent != nullptr && node_ == parent->right) {
                node_ = parent;
                parent = parent->parent;
            }
            node_ = parent;
        }
    }
    return *this;
}

template <typename T, typename Compare>
typename Set<T, Compare>::Iterator Set<T, Compare>::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename T, typename Compare>
typename Set<T, Compare>::Iterator& Set<T, Compare>::Iterator::operator--() {
    if (node_ == nullptr) {
        node_ = Maximum(root_);
    }
    else if (node_->left != nullptr) {
        node_ = Maximum(node_->left);
    }
    else {
        Node* parent = node_->parent;
        while (parent != nullptr && node_ == parent->left) {
            node_ = parent;
            parent = parent->parent;
        }
        node_ = parent;
    }
    return *this;
}

template <typename T, typename Compare>
typename Set<T, Compare>::Iterator Set<T, Compare>::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

template <typename T, typename Compare>
typename Set<T, Compare>::Iterator::reference Set<T, Compare>::Iterator::operator*() const {
    return node_->data;
}

template <typename T, typename Compare>
typename Set<T, Compare>::Iterator::pointer Set<T, Compare>::Iterator::operator->() const {
    return &(node_->data);
}

template <typename T, typename Compare>
bool Set<T, Compare>::Iterator::operator==(const Iterator& other) const {
    return node_ == other.node_;
}

template <typename T, typename Compare>
bool Set<T, Compare>::Iterator::operator!=(const Iterator& other) const {
    return node_ != other.node_;
}
