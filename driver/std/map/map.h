#ifndef MAP_H_
#define MAP_H_

#include "../memory/memory.h"
#include "../memory/pair.h"
#include "../iterator/iterator.h"

// Map class definition
template <typename Key, typename T, typename Compare = Less<Key>>
class Map {
private:
    // Node structure for the Red-Black Tree
    struct Node {
        Pair<const Key, T> data;
        bool is_red;
        Node* parent;
        Node* left;
        Node* right;

        explicit Node(const Key& key, const T& value)
            : data(key, value), is_red(true), parent(nullptr), left(nullptr), right(nullptr) {}
    };

    Node* root_;
    Node* nil_;  // Sentinel node for leaf nodes
    size_t size_;
    Compare compare_;

    // Helper functions for Red-Black Tree operations
    void LeftRotate(Node* x);
    void RightRotate(Node* y);
    void InsertFixup(Node* z);
    void DeleteFixup(Node* x);
    void Transplant(Node* u, Node* v);
    Node* Minimum(Node* node) const;
    Node* Maximum(Node* node) const;
    Node* Successor(Node* node) const;
    Node* Predecessor(Node* node) const;
    void Clear(Node* node);
    Node* Copy(Node* node, Node* parent);

public:
    // Type definitions
    using key_type = Key;
    using mapped_type = T;
    using value_type = Pair<const Key, T>;
    using KeyCompare = Compare;

    class ValueCompare {
    protected:
        Compare comp;
        explicit ValueCompare(Compare c) : comp(c) {}

    public:
        bool operator()(const value_type& lhs, const value_type& rhs) const {
            return comp(lhs.first, rhs.first);
        }
    };

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    // Iterator class
    class Iterator : public IteratorBase<value_type, BidirectionalIteratorTag> {
    private:
        Node* node_;
        const Map* map_;

    public:
        using value_type = value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = BidirectionalIteratorTag;

        Iterator() : node_(nullptr), map_(nullptr) {}
        Iterator(Node* node, const Map* map) : node_(node), map_(map) {}

        reference operator*() const { return node_->data; }
        pointer operator->() const { return &(node_->data); }

        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);

        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }

        friend class Map;
    };

    using iterator = Iterator;
    using const_iterator = Iterator;  // For simplicity, use the same iterator
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

    // Constructors
    Map();
    explicit Map(const KeyCompare& comp);
    template <class InputIterator>
    Map(InputIterator first, InputIterator last, const KeyCompare& comp = KeyCompare());
    Map(const Map& other);
    Map(Map&& other);
    Map& operator=(const Map& other);
    Map& operator=(Map&& other);

    // Destructor
    ~Map();

    // Iterators
    iterator Begin() const;
    iterator End() const;
    reverse_iterator RBegin() const;
    reverse_iterator REnd() const;
    const_iterator CBegin() const;
    const_iterator CEnd() const;
    const_reverse_iterator CRBegin() const;
    const_reverse_iterator CREnd() const;

    // Capacity
    bool Empty() const;
    size_type Size() const;
    size_type MaxSize() const;

    // Element access
    T& operator[](const Key& key);

    // Modifiers
    Pair<iterator, bool> Insert(const Key& key, const T& val);
    Pair<iterator, bool> Insert(const value_type& val);
    iterator Erase(const_iterator position);
    size_type Erase(const key_type& key);
    iterator Erase(const_iterator first, const_iterator last);
    void Swap(Map& other);
    void Clear();

    // Observers
    KeyCompare KeyComp() const;
    ValueCompare ValueComp() const;

    // Operations
    iterator Find(const key_type& key) const;
    size_type Count(const key_type& key) const;
    iterator LowerBound(const key_type& key) const;
    iterator UpperBound(const key_type& key) const;
    Pair<iterator, iterator> EqualRange(const key_type& key) const;
};

// Implementation details

// Constructors

template <typename Key, typename T, typename Compare>
Map<Key, T, Compare>::Map() : size_(0), compare_() {
    nil_ = new Node(Key(), T());
    nil_->is_red = false;
    nil_->left = nil_->right = nil_->parent = nil_;
    root_ = nil_;
}

template <typename Key, typename T, typename Compare>
Map<Key, T, Compare>::Map(const KeyCompare& comp) : size_(0), compare_(comp) {
    nil_ = new Node(Key(), T());
    nil_->is_red = false;
    nil_->left = nil_->right = nil_->parent = nil_;
    root_ = nil_;
}

template <typename Key, typename T, typename Compare>
template <class InputIterator>
Map<Key, T, Compare>::Map(InputIterator first, InputIterator last, const KeyCompare& comp)
    : Map(comp) {
    for (; first != last; ++first) {
        Insert(*first);
    }
}

template <typename Key, typename T, typename Compare>
Map<Key, T, Compare>::Map(const Map& other) : Map(other.compare_) {
    if (other.root_ != other.nil_) {
        root_ = Copy(other.root_, nil_);
        size_ = other.size_;
    }
}

template <typename Key, typename T, typename Compare>
Map<Key, T, Compare>::Map(Map&& other)
    : root_(other.root_), nil_(other.nil_), size_(other.size_), compare_(other.compare_) {
    other.root_ = nullptr;
    other.nil_ = nullptr;
    other.size_ = 0;
}

// Destructor

template <typename Key, typename T, typename Compare>
Map<Key, T, Compare>::~Map() {
    Clear();
    delete nil_;
}

// Assignment operators

template <typename Key, typename T, typename Compare>
Map<Key, T, Compare>& Map<Key, T, Compare>::operator=(const Map& other) {
    if (this != &other) {
        Clear();
        compare_ = other.compare_;
        if (other.root_ != other.nil_) {
            root_ = Copy(other.root_, nil_);
            size_ = other.size_;
        }
        else {
            root_ = nil_;
            size_ = 0;
        }
    }
    return *this;
}

template <typename Key, typename T, typename Compare>
Map<Key, T, Compare>& Map<Key, T, Compare>::operator=(Map&& other) {
    if (this != &other) {
        Clear();
        delete nil_;
        root_ = other.root_;
        nil_ = other.nil_;
        size_ = other.size_;
        compare_ = other.compare_;

        other.root_ = nullptr;
        other.nil_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

// Iterators

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::iterator Map<Key, T, Compare>::Begin() const {
    return iterator(Minimum(root_), this);
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::iterator Map<Key, T, Compare>::End() const {
    return iterator(nil_, this);
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::reverse_iterator Map<Key, T, Compare>::RBegin() const {
    return reverse_iterator(End());
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::reverse_iterator Map<Key, T, Compare>::REnd() const {
    return reverse_iterator(Begin());
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::const_iterator Map<Key, T, Compare>::CBegin() const {
    return Begin();
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::const_iterator Map<Key, T, Compare>::CEnd() const {
    return End();
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::const_reverse_iterator Map<Key, T, Compare>::CRBegin() const {
    return RBegin();
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::const_reverse_iterator Map<Key, T, Compare>::CREnd() const {
    return REnd();
}

// Capacity

template <typename Key, typename T, typename Compare>
bool Map<Key, T, Compare>::Empty() const {
    return size_ == 0;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::size_type Map<Key, T, Compare>::Size() const {
    return size_;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::size_type Map<Key, T, Compare>::MaxSize() const {
    return static_cast<size_type>(-1) / sizeof(Node);
}

// Element access

template <typename Key, typename T, typename Compare>
T& Map<Key, T, Compare>::operator[](const Key& key) {
    Node* x = root_;
    Node* y = nil_;
    while (x != nil_) {
        y = x;
        if (compare_(key, x->data.first)) {
            x = x->left;
        }
        else if (compare_(x->data.first, key)) {
            x = x->right;
        }
        else {
            return x->data.second;
        }
    }
    Node* z = new Node(key, T());
    z->parent = y;
    if (y == nil_) {
        root_ = z;
    }
    else if (compare_(z->data.first, y->data.first)) {
        y->left = z;
    }
    else {
        y->right = z;
    }
    z->left = nil_;
    z->right = nil_;
    z->is_red = true;

    InsertFixup(z);
    ++size_;
    return z->data.second;
}

// Modifiers

template <typename Key, typename T, typename Compare>
Pair<typename Map<Key, T, Compare>::iterator, bool> Map<Key, T, Compare>::Insert(
    const value_type& val) {
    return Insert(val.first, val.second);
}

template <typename Key, typename T, typename Compare>
Pair<typename Map<Key, T, Compare>::iterator, bool> Map<Key, T, Compare>::Insert(
    const Key& key, const T& val) {
    Node* z = new Node(key, val);
    Node* y = nil_;
    Node* x = root_;

    while (x != nil_) {
        y = x;
        if (compare_(z->data.first, x->data.first)) {
            x = x->left;
        }
        else if (compare_(x->data.first, z->data.first)) {
            x = x->right;
        }
        else {
            delete z;
            return Pair<iterator, bool>(iterator(x, this), false);
        }
    }

    z->parent = y;
    if (y == nil_) {
        root_ = z;
    }
    else if (compare_(z->data.first, y->data.first)) {
        y->left = z;
    }
    else {
        y->right = z;
    }

    z->left = nil_;
    z->right = nil_;
    z->is_red = true;

    InsertFixup(z);
    ++size_;
    return Pair<iterator, bool>(iterator(z, this), true);
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::iterator Map<Key, T, Compare>::Erase(const_iterator position) {
    if (position == End()) {
        return iterator();
    }
    Node* z = position.node_;
    Node* y = z;
    Node* x = nullptr;
    bool y_original_color = y->is_red;

    if (z->left == nil_) {
        x = z->right;
        Transplant(z, z->right);
    }
    else if (z->right == nil_) {
        x = z->left;
        Transplant(z, z->left);
    }
    else {
        y = Minimum(z->right);
        y_original_color = y->is_red;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        }
        else {
            Transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        Transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->is_red = z->is_red;
    }

    delete z;
    --size_;

    if (!y_original_color) {
        DeleteFixup(x);
    }

    return iterator(x, this);
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::size_type Map<Key, T, Compare>::Erase(const key_type& key) {
    iterator it = Find(key);
    if (it != End()) {
        Erase(it);
        return 1;
    }
    return 0;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::iterator Map<Key, T, Compare>::Erase(const_iterator first,
    const_iterator last) {
    while (first != last) {
        first = Erase(first);
    }
    return iterator(last.node_, this);
}

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::Swap(Map& other) {
    Node* temp_root = root_;
    Node* temp_nil = nil_;
    size_t temp_size = size_;
    Compare temp_compare = compare_;

    root_ = other.root_;
    nil_ = other.nil_;
    size_ = other.size_;
    compare_ = other.compare_;

    other.root_ = temp_root;
    other.nil_ = temp_nil;
    other.size_ = temp_size;
    other.compare_ = temp_compare;
}

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::Clear() {
    Clear(root_);
    root_ = nil_;
    size_ = 0;
}

// Observers

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::KeyCompare Map<Key, T, Compare>::KeyComp() const {
    return compare_;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::ValueCompare Map<Key, T, Compare>::ValueComp() const {
    return ValueCompare(compare_);
}

// Operations

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::iterator Map<Key, T, Compare>::Find(const key_type& key) const {
    Node* current = root_;
    while (current != nil_) {
        if (compare_(key, current->data.first)) {
            current = current->left;
        }
        else if (compare_(current->data.first, key)) {
            current = current->right;
        }
        else {
            return iterator(current, this);
        }
    }
    return End();
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::size_type Map<Key, T, Compare>::Count(const key_type& key) const {
    return Find(key) != End() ? 1 : 0;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::iterator Map<Key, T, Compare>::LowerBound(const key_type& key) const {
    Node* current = root_;
    Node* result = nil_;
    while (current != nil_) {
        if (!compare_(current->data.first, key)) {
            result = current;
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return iterator(result, this);
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::iterator Map<Key, T, Compare>::UpperBound(const key_type& key) const {
    Node* current = root_;
    Node* result = nil_;
    while (current != nil_) {
        if (compare_(key, current->data.first)) {
            result = current;
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return iterator(result, this);
}

template <typename Key, typename T, typename Compare>
Pair<typename Map<Key, T, Compare>::iterator, typename Map<Key, T, Compare>::iterator>
Map<Key, T, Compare>::EqualRange(const key_type& key) const {
    return Pair<iterator, iterator>(LowerBound(key), UpperBound(key));
}

// Helper Functions

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::LeftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != nil_) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nil_) {
        root_ = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::RightRotate(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    if (x->right != nil_) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == nil_) {
        root_ = x;
    }
    else if (y == y->parent->left) {
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::InsertFixup(Node* z) {
    while (z->parent->is_red) {
        if (z->parent == z->parent->parent->left) {
            Node* y = z->parent->parent->right;
            if (y->is_red) {
                z->parent->is_red = false;
                y->is_red = false;
                z->parent->parent->is_red = true;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    LeftRotate(z);
                }
                z->parent->is_red = false;
                z->parent->parent->is_red = true;
                RightRotate(z->parent->parent);
            }
        }
        else {
            Node* y = z->parent->parent->left;
            if (y->is_red) {
                z->parent->is_red = false;
                y->is_red = false;
                z->parent->parent->is_red = true;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    RightRotate(z);
                }
                z->parent->is_red = false;
                z->parent->parent->is_red = true;
                LeftRotate(z->parent->parent);
            }
        }
    }
    root_->is_red = false;
}

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::DeleteFixup(Node* x) {
    while (x != root_ && !x->is_red) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w->is_red) {
                w->is_red = false;
                x->parent->is_red = true;
                LeftRotate(x->parent);
                w = x->parent->right;
            }
            if (!w->left->is_red && !w->right->is_red) {
                w->is_red = true;
                x = x->parent;
            }
            else {
                if (!w->right->is_red) {
                    w->left->is_red = false;
                    w->is_red = true;
                    RightRotate(w);
                    w = x->parent->right;
                }
                w->is_red = x->parent->is_red;
                x->parent->is_red = false;
                w->right->is_red = false;
                LeftRotate(x->parent);
                x = root_;
            }
        }
        else {
            Node* w = x->parent->left;
            if (w->is_red) {
                w->is_red = false;
                x->parent->is_red = true;
                RightRotate(x->parent);
                w = x->parent->left;
            }
            if (!w->left->is_red && !w->right->is_red) {
                w->is_red = true;
                x = x->parent;
            }
            else {
                if (!w->left->is_red) {
                    w->right->is_red = false;
                    w->is_red = true;
                    LeftRotate(w);
                    w = x->parent->left;
                }
                w->is_red = x->parent->is_red;
                x->parent->is_red = false;
                w->left->is_red = false;
                RightRotate(x->parent);
                x = root_;
            }
        }
    }
    x->is_red = false;
}

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::Transplant(Node* u, Node* v) {
    if (u->parent == nil_) {
        root_ = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Node* Map<Key, T, Compare>::Minimum(Node* node) const {
    while (node->left != nil_) {
        node = node->left;
    }
    return node;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Node* Map<Key, T, Compare>::Maximum(Node* node) const {
    while (node->right != nil_) {
        node = node->right;
    }
    return node;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Node* Map<Key, T, Compare>::Successor(Node* node) const {
    if (node->right != nil_) {
        return Minimum(node->right);
    }
    Node* y = node->parent;
    while (y != nil_ && node == y->right) {
        node = y;
        y = y->parent;
    }
    return y;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Node* Map<Key, T, Compare>::Predecessor(Node* node) const {
    if (node->left != nil_) {
        return Maximum(node->left);
    }
    Node* y = node->parent;
    while (y != nil_ && node == y->left) {
        node = y;
        y = y->parent;
    }
    return y;
}

template <typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::Clear(Node* node) {
    if (node != nil_) {
        Clear(node->left);
        Clear(node->right);
        delete node;
    }
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Node* Map<Key, T, Compare>::Copy(Node* node, Node* parent) {
    if (node == nil_) {
        return nil_;
    }
    Node* new_node = new Node(node->data.first, node->data.second);
    new_node->is_red = node->is_red;
    new_node->parent = parent;
    new_node->left = Copy(node->left, new_node);
    new_node->right = Copy(node->right, new_node);
    return new_node;
}

// Iterator implementation

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Iterator& Map<Key, T, Compare>::Iterator::operator++() {
    if (node_ == map_->nil_) {
        return *this;
    }
    node_ = map_->Successor(node_);
    return *this;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Iterator Map<Key, T, Compare>::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Iterator& Map<Key, T, Compare>::Iterator::operator--() {
    if (node_ == map_->nil_) {
        node_ = map_->Maximum(map_->root_);
        return *this;
    }
    node_ = map_->Predecessor(node_);
    return *this;
}

template <typename Key, typename T, typename Compare>
typename Map<Key, T, Compare>::Iterator Map<Key, T, Compare>::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

#endif  // MAP_H_
