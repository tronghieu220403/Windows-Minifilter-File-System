#pragma once

#include "../ulti/def.h"

struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag : public InputIteratorTag {};
struct BidirectionalIteratorTag : public ForwardIteratorTag {};
struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

// Custom iterator base class
template <typename T, typename Category>
struct IteratorBase {
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
    typedef Category iterator_category;
};

// Custom reverse iterator
template <typename IteratorType>
class ReverseIterator {
private:
    IteratorType base_iterator_;

public:
    typedef typename IteratorType::value_type value_type;
    typedef typename IteratorType::difference_type difference_type;
    typedef typename IteratorType::pointer pointer;
    typedef typename IteratorType::reference reference;
    typedef BidirectionalIteratorTag iterator_category;

    explicit ReverseIterator(IteratorType it) : base_iterator_(it) {}

    reference operator*() const {
        IteratorType temp = base_iterator_;
        --temp;
        return *temp;
    }

    pointer operator->() const {
        IteratorType temp = base_iterator_;
        --temp;
        return temp.operator->();
    }

    ReverseIterator& operator++() {
        --base_iterator_;
        return *this;
    }

    ReverseIterator operator++(int) {
        ReverseIterator temp = *this;
        --base_iterator_;
        return temp;
    }

    ReverseIterator& operator--() {
        ++base_iterator_;
        return *this;
    }

    ReverseIterator operator--(int) {
        ReverseIterator temp = *this;
        ++base_iterator_;
        return temp;
    }

    bool operator==(const ReverseIterator& other) const {
        return base_iterator_ == other.base_iterator_;
    }

    bool operator!=(const ReverseIterator& other) const {
        return base_iterator_ != other.base_iterator_;
    }
};