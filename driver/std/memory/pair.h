#pragma once

#include "memory.h"

// Pair structure
template <class T1, class T2>
struct Pair {
public:
    T1 first_;
    T2 second_;

    Pair() : first_(), second_() {}
    Pair(const T1& first, const T2& second) : first_(first), second_(second) {}

    Pair(const Pair& p) = default;
    Pair(Pair&& p) = default;

    Pair& operator=(const Pair& other) {
        first_ = other.first_;
        second_ = other.second_;
        return *this;
    }

    bool operator==(const Pair& other) const {
        return first_ == other.first_ && second_ == other.second_;
    }

    bool operator!=(const Pair& other) const {
        return !(*this == other);
    }

    bool operator<(const Pair& other) const {
        return first_ < other.first_ ||
            (!(other.first_ < first_) && second_ < other.second_);
    }

    bool operator>(const Pair& other) const {
        return other < *this;
    }

    bool operator<=(const Pair& other) const {
        return !(other < *this);
    }

    bool operator>=(const Pair& other) const {
        return !(*this < other);
    }
};

// MakePair function
template <class T1, class T2>
Pair<T1, T2> MakePair(const T1& a, const T2& b) {
    return Pair<T1, T2>(a, b);
}
