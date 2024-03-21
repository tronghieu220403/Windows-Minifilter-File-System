#pragma once

#include "../memory/memory.h"

template<typename T> class Vector
{
public:
	/* ----- Constructors ----- */

	// Default constructor
	Vector();

	explicit Vector(size_t s);

	// Copy constructor
	Vector(const Vector&);

	// Copy Assingment
	Vector<T>& operator=(const Vector<T>&);

	// Destructor
	~Vector();

	/*----------------------------*/





	/* -------- ITERATORS --------*/

	class iterator;

	iterator Begin();

	const iterator Begin() const;

	iterator End();
	
	const iterator End() const;

	const iterator ConstBegin() const;

	const iterator ConstEnd() const;

	/*----------------------------*/





	/* -------- CAPACITY -------- */

	bool Empty() const;

	// Returns size of allocated storate capacity
	size_t Capacity() const;

	// Requests a change in capacity
	// reserve() will never decrase the capacity.
	void Reserve(size_t new_size);

	// Changes the Vector's size.
	// If the newsize is smaller, the last elements will be lost.
	// Has a default value param for custom values when resizing.
	void Resize(size_t new_size, T val = T());

	// Returns the size of the Vector (number of elements). 
	size_t Size() const;

	// Returns the maximum number of elements the Vector can hold
	size_t MaxSize() const;

	// Reduces capcity to fit the size
	void ShrinkToFit();

	/*----------------------------*/





	/* -------- MODIFIERS --------*/

	// Removes all elements from the Vector
	// Capacity is not changed.
	void Clear();

	// Inserts element at the back
	void PushBack(const T& d);

	// Removes the last element from the Vector
	void PopBack();

	// Append a vector to the back.
	void Append(const Vector<T>& v);

	bool Swap(size_t, size_t);

	bool Erase(size_t);
	bool EraseUnordered(size_t);

	// Add a vector to the back.
	Vector<T>& operator+=(const Vector<T>&);

	// Combination of 2 vectors
	Vector<T> operator+(const Vector<T>&);

	/*----------------------------*/





	/* ----- ELEMENT ACCESS ----- */

	// Access elements with bounds checking
	T& At(size_t n);

	// Access elements with bounds checking for constant Vectors.
	const T& At(size_t n) const;

	// Access elements, no bounds checking
	T& operator[](size_t i);

	// Access elements, no bounds checking
	const T& operator[](size_t i) const;

	// Returns a reference to the first element
	T& Front();

	// Returns a reference to the first element
	const T& Front() const;

	// Returns a reference to the last element
	T& Back();

	// Returns a reference to the last element
	const T& Back() const;

	// Returns a posize_ter to the array used by Vector
	T* Data();

	// Returns a posize_ter to the array used by Vector
	const T* Data() const;

	/*----------------------------*/

	/* -------- COMPARISON -------*/

	// Overloading the equal operator
	bool operator==(const Vector<T>&);

	// Overloading the unequal operator
	bool operator!=(const Vector<T>&);

	/*----------------------------*/

protected:
	T* Allocate(size_t);
	void Deallocate();
	

private:
	size_t	size_;		// Number of elements in Vector
	T*		elements_;	// Posize_ter to first element of Vector
	size_t	space_;		// Total space used by Vector including
						// elements and free space.
};



template<class T> class Vector<T>::iterator
{
public:
	iterator(T* p)
		:curr_(p)
	{}

	iterator& operator++()
	{
		curr_++;
		return *this;
	}

	iterator& operator--()
	{
		curr_--;
		return *this;
	}

	T& operator*()
	{
		return *curr_;
	}

	bool operator==(const iterator& b) const
	{
		return *curr_ == *b.curr_;
	}

	bool operator!=(const iterator& b) const
	{
		return *curr_ != *b.curr_;
	}

private:
	T* curr_;
};



// Constructors/Destructor
template<class T>
inline Vector<T>::Vector()
	:size_(0), elements_(0), space_(0)
{}


template<class T>
inline Vector<T>::Vector(size_t s)
	:size_(s), elements_(Allocate(s)), space_(s)
{
	for (size_t index = 0; index < size_; ++index)
		elements_[index] = T();
}


template<class T>
inline Vector<T>::Vector(const Vector & v)
	:size_(v.size_), elements_(Allocate(v.size_)), space_(v.size_)
{
	for (size_t index = 0; index < v.size_; ++index)
		elements_[index] = v.elements_[index];
}

template<class T>
inline Vector<T>& Vector<T>::operator=(const Vector<T>& v)
{
	if (this == &v) return *this;	// Self-assingment not work needed

									// Current Vector has enough space, so there is no need for new allocation
	if (v.size_ <= space_)
	{
		for (size_t index = 0; index < v.size_; ++index)
		{
			elements_[index] = v.elements_[index];
			size_ = v.size_;
			return *this;
		}
	}

	T* p = Allocate(v.size_);

	for (size_t index = 0; index < v.size_; ++index)
		p[index] = v.elements_[index];

	delete[] elements_;
	size_ = v.size_;
	space_ = v.size_;
	elements_ = p;
	return *this;
}

template<class T>
Vector<T>::~Vector()
{
	delete[] elements_;
}



// Iterators
template<class T>
inline typename Vector<T>::iterator Vector<T>::Begin()
{	
	return Vector<T>::iterator(&elements_[0]);
}

template<class T>
inline const Vector<T>::iterator Vector<T>::Begin() const
{
	return Vector<T>::iterator(&elements_[0]);
}

template<class T>
inline Vector<T>::iterator Vector<T>::End()
{
	return Vector<T>::iterator(&elements_[size_]);
}

template<class T>
inline const Vector<T>::iterator Vector<T>::End() const
{
	return Vector<T>::iterator(&elements_[size_]);
}

template<class T>
inline const Vector<T>::iterator Vector<T>::ConstBegin() const
{
	return Vector<T>::iterator(&elements_[0]);
}

template<class T>
inline const Vector<T>::iterator Vector<T>::ConstEnd() const
{
	return Vector<T>::iterator(&elements_[size_]);
}



// Capacity
template<class T>
inline bool Vector<T>::Empty() const
{
	return (size_ == 0);
}

template<class T>
inline size_t Vector<T>::Capacity() const
{
	return space_;
}

template<class T>
inline void Vector<T>::Reserve(size_t new_size)
{
	if (new_size <= space_) return;

	T* p = Allocate(new_size);

	for (size_t i = 0; i < size_; ++i)
		p[i] = elements_[i];

	delete[] elements_;

	elements_ = p;

	space_ = new_size;
}

template<class T>
inline void Vector<T>::Resize(size_t new_size, T val)
{
	Reserve(new_size);

	for (size_t index = size_; index < new_size; ++index)
		elements_[index] = T();

	size_ = new_size;
}

template<class T>
inline size_t Vector<T>::Size() const
{
	return size_;
}



template<class T>
inline void Vector<T>::Clear()
{
	for (size_t index = 0; index < size_; ++index)
		elements_[index] = T();
	space_ += size_;
	size_ = 0;
}

// Modifiers
template<class T>
inline void Vector<T>::PushBack(const T& d)
{
	if (space_ == 0)
		Reserve(8);
	else if (size_ == space_)
		Reserve(2 * space_);

	elements_[size_] = d;

	++size_;
}

template<typename T>
inline void Vector<T>::PopBack()
{
	if (size_ == 0)
		return;

	elements_[size_ - 1] = T();
	--size_;
}

template<class T>
inline void Vector<T>::Append(const Vector<T>& v)
{
	Reserve(max(v.Size(), size_) * 2);

	for (size_t index = 0; index < v.Size(); ++index)
		elements_[index + size_] = v[index];

	size_ += v.Size();

}

template<typename T>
inline bool Vector<T>::Swap(size_t i, size_t j)
{
	if (i >= size_ || j >= size_)
		return false;
	T temp = elements_[i];
	elements_[i] = elements_[j];
	elements_[j] = temp;
	return true;
}

template<typename T>
inline bool Vector<T>::Erase(size_t i)
{
	if (i >= size_)
		return false;
	if (size_ == 1)
	{
		elements_[0] = T();
		--size_;
		return true;
	}
	// Move all elements after i one position to the left
	for (size_t index = i; index < size_ - 2; ++index)
		elements_[index] = elements_[index + 1];
	elements_[size_ - 1] = T();
	--size_;
	return true;
}

template<typename T>
inline bool Vector<T>::EraseUnordered(size_t i)
{
	// If the index is out of bounds, return false
	if (i >= size_)
		return false;
	// If the size is 1, just clear the vector
	if (size_ == 1)
	{
		elements_[0] = T();
		--size_;
		return true;
	}
	// Swap the element to be removed with the last element
	elements_[i] = elements_[size_ - 1];
	elements_[size_ - 1] = T();
	--size_;
	return true;
}

template<class T>
inline Vector<T>& Vector<T>::operator+=(const Vector<T>& v)
{
	Append(v);
	return *this;
}

template<class T>
inline Vector<T> Vector<T>::operator+(const Vector<T>& v)
{
	// TODO: insert return statement here
	Vector<T> res;
	res.Reserve(max(v.Size(), size_) * 2);
	res.Resize(v.Size() + size_);

	for (size_t index = 0; index < size_; ++index)
	{
		res[index] = elements_[index];
	}

	for (size_t index = 0; index < v.Size(); ++index)
	{
		res[index + size_] = v[index];
	}

	return res;

}


// Accessors
template<class T>
inline T & Vector<T>::At(size_t n)
{
	return elements_[n];
}

template<class T>
inline const T & Vector<T>::At(size_t n) const
{
	return elements_[n];
}

template<class T>
inline T & Vector<T>::operator[](size_t i)
{
	return elements_[i];
}

template<class T>
inline const T & Vector<T>::operator[](size_t i) const
{
	return elements_[i];
}

template<class T>
inline T& Vector<T>::Front()
{
	return elements_[0];
}

template<class T>
inline const T& Vector<T>::Front() const
{
	return elements_[0];
}

template<class T>
inline T& Vector<T>::Back()
{
	return elements_[size_ - 1];
}

template<class T>
inline const T& Vector<T>::Back() const
{
	return elements_[size_ - 1];
}

template<class T>
inline T* Vector<T>::Data()
{
	return elements_;
}

template<class T>
inline const T* Vector<T>::Data() const
{
	return elements_;
}

template<class T>
inline bool Vector<T>::operator==(const Vector<T>& v)
{
	if (size_ != v.Size())
	{
		return false;
	}

	for (size_t index = 0; index < size_; ++index)
	{
		if (elements_[index] != v[index])
		{
			return false;
		}
	}

	return true;
}

template<class T>
inline bool Vector<T>::operator!=(const Vector<T>& v)
{
	if (size_ != v.Size())
	{
		return true;
	}

	for (size_t index = 0; index < size_; ++index)
	{
		if (elements_[index] != v[index])
		{
			return true;
		}
	}

	return false;
}

template<class T>
inline T* Vector<T>::Allocate(size_t n)
{
	T* p = (T *)krnl_std::Alloc(n * sizeof(T));
	return p;
}

template<class T>
inline void Vector<T>::Deallocate()
{
	if (elements_ == nullptr)
	{
		return;
	}

	krnl_std::Free(elements_);
	elements_;
}
