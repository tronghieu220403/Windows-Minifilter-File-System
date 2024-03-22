#pragma once

#include "memory.h"

template<class T1, class T2> struct Pair
{
public:
	T1 first_;
	T2 second_;

	Pair() : first_(), second_() {}
	Pair(const T1& first_, const T2& second_) : first_(first_), second_(second_) {}

	Pair(const Pair& p) = default;

	constexpr Pair(Pair&& p) = default;

	Pair& operator=(const Pair& other)
	{
		first_ = other.first_;
		second_ = other.second_;
		return *this;
	}

	bool operator==(const Pair& other) const
	{
		return first_ == other.first_ && second_ == other.second_;
	}

	bool operator!=(const Pair& other) const
	{
		return first_ != other.first_ || second_ != other.second_;
	}

	bool operator<(const Pair& other) const
	{
		return first_ < other.first_ || (first_ == other.first_ && second_ < other.second_);
	}

	bool operator>(const Pair& other) const
	{
		return first_ > other.first_ || (first_ == other.first_ && second_ > other.second_);
	}

	bool operator<=(const Pair& other) const
	{
		return first_ < other.first_ || (first_ == other.first_ && second_ <= other.second_);
	}

	bool operator>=(const Pair& other) const
	{
		return first_ > other.first_ || (first_ == other.first_ && second_ >= other.second_);
	}
};
