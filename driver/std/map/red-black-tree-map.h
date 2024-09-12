#pragma once
/*
Generate a map library like std::map but using red black tree in it implementation and do not use std::map. The new library must have these following function: delete, instert, at. It must be worked on the Windows kernel mode driver. Write full code for me.
*/

// TODO: write contain() function to check if the key exists
// TODO: write clear() function to delete all nodes
// TODO: write a destructor to delete all nodes
// TODO: write a copy constructor
// TODO: write an assignment operator
// TODO: write a move constructor

template <typename Key, typename Value>
class RedBlackTreeMap {
private:
    struct Node {
        Key key;
        Value value;
        bool isRed;
        Node* left;
        Node* right;
        Node* parent;

        Node(const Key& k, const Value& v) : key(k), value(v), isRed(true), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node* root;

public:
    RedBlackTreeMap() : root(nullptr) {}

    void Insert(const Key& key, const Value& value) {
        Node* new_node = new Node(key, value);
        if (root == nullptr) {
            root = new_node;
            root->isRed = false;
        }
        else {
            Node* current = root;
            Node* parent = nullptr;
            while (current != nullptr) {
                parent = current;
                if (key < current->key) {
                    current = current->left;
                }
                else if (key > current->key) {
                    current = current->right;
                }
                else {
                    current->value = value;
                    return;
                }
            }
            new_node->parent = parent;
            if (key < parent->key) {
                parent->left = new_node;
            }
            else {
                parent->right = new_node;
            }
            FixInsertion(new_node);
        }
    }

    void Erase(const Key& key) {
        Node* node = FindNode(key);
        if (node == nullptr) {
            return;
        }
        if (node->left != nullptr && node->right != nullptr) {
            Node* successor = FindSuccessor(node);
            node->key = successor->key;
            node->value = successor->value;
            node = successor;
        }
        Node* child = (node->left != nullptr) ? node->left : node->right;
        if (node->isRed) {
            ReplaceNode(node, child);
            delete node;
        }
        else if (child != nullptr && child->isRed) {
            child->isRed = false;
            ReplaceNode(node, child);
            delete node;
        }
        else {
            FixDeletion(node);
            ReplaceNode(node, nullptr);
            delete node;
        }
    }

    Value& At(const Key& key) {
        Node* node = FindNode(key);
        if (node == nullptr) {
            return Value();
        }
        return node->value;
    }

private:
    void FixInsertion(Node* node) {
        while (node->parent != nullptr && node->parent->isRed) {
            if (node->parent == node->parent->parent->left) {
                Node* uncle = node->parent->parent->right;
                if (uncle != nullptr && uncle->isRed) {
                    node->parent->isRed = false;
                    uncle->isRed = false;
                    node->parent->parent->isRed = true;
                    node = node->parent->parent;
                }
                else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        RotateLeft(node);
                    }
                    node->parent->isRed = false;
                    node->parent->parent->isRed = true;
                    RotateRight(node->parent->parent);
                }
            }
            else {
                Node* uncle = node->parent->parent->left;
                if (uncle != nullptr && uncle->isRed) {
                    node->parent->isRed = false;
                    uncle->isRed = false;
                    node->parent->parent->isRed = true;
                    node = node->parent->parent;
                }
                else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        RotateRight(node);
                    }
                    node->parent->isRed = false;
                    node->parent->parent->isRed = true;
                    RotateLeft(node->parent->parent);
                }
            }
        }
        root->isRed = false;
    }

    void FixDeletion(Node* node) {
        while (node != root && !node->isRed) {
            if (node == node->parent->left) {
                Node* sibling = node->parent->right;
                if (sibling->isRed) {
                    sibling->isRed = false;
                    node->parent->isRed = true;
                    RotateLeft(node->parent);
                    sibling = node->parent->right;
                }
                if (!sibling->left->isRed && !sibling->right->isRed) {
                    sibling->isRed = true;
                    node = node->parent;
                }
                else {
                    if (!sibling->right->isRed) {
                        sibling->left->isRed = false;
                        sibling->isRed = true;
                        RotateRight(sibling);
                        sibling = node->parent->right;
                    }
                    sibling->isRed = node->parent->isRed;
                    node->parent->isRed = false;
                    sibling->right->isRed = false;
                    RotateLeft(node->parent);
                    node = root;
                }
            }
            else {
                Node* sibling = node->parent->left;
                if (sibling->isRed) {
                    sibling->isRed = false;
                    node->parent->isRed = true;
                    RotateRight(node->parent);
                    sibling = node->parent->left;
                }
                if (!sibling->left->isRed && !sibling->right->isRed) {
                    sibling->isRed = true;
                    node = node->parent;
                }
                else {
                    if (!sibling->left->isRed) {
                        sibling->right->isRed = false;
                        sibling->isRed = true;
                        RotateLeft(sibling);
                        sibling = node->parent->left;
                    }
                    sibling->isRed = node->parent->isRed;
                    node->parent->isRed = false;
                    sibling->left->isRed = false;
                    RotateRight(node->parent);
                    node = root;
                }
            }
        }
        node->isRed = false;
    }

    void RotateLeft(Node* node) {
        Node* right_child = node->right;
        node->right = right_child->left;
        if (right_child->left != nullptr) {
            right_child->left->parent = node;
        }
        right_child->parent = node->parent;
        if (node->parent == nullptr) {
            root = right_child;
        }
        else if (node == node->parent->left) {
            node->parent->left = right_child;
        }
        else {
            node->parent->right = right_child;
        }
        right_child->left = node;
        node->parent = right_child;
    }

    void RotateRight(Node* node) {
        Node* left_child = node->left;
        node->left = left_child->right;
        if (left_child->right != nullptr) {
            left_child->right->parent = node;
        }
        left_child->parent = node->parent;
        if (node->parent == nullptr) {
            root = left_child;
        }
        else if (node == node->parent->left) {
            node->parent->left = left_child;
        }
        else {
            node->parent->right = left_child;
        }
        left_child->right = node;
        node->parent = left_child;
    }

    Node* FindNode(const Key& key) {
        Node* current = root;
        while (current != nullptr) {
            if (key < current->key) {
                current = current->left;
            }
            else if (key > current->key) {
                current = current->right;
            }
            else {
                return current;
            }
        }
        return nullptr;
    }

    Node* FindSuccessor(Node* node) {
        Node* current = node->right;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    void ReplaceNode(Node* node, Node* child) {
        if (node->parent == nullptr) {
            root = child;
        }
        else if (node == node->parent->left) {
            node->parent->left = child;
        }
        else {
            node->parent->right = child;
        }
        if (child != nullptr) {
            child->parent = node->parent;
        }
    }
};