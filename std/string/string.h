#pragma once

#include "../memory/memory.h"

template<class T> class String
{
public:
	/* ----- Constructors ----- */

	// Default constructor
	String();

	explicit String(size_t size);

	// Copy constructor
	String(const String&);
	String(const T*);

	String(const UNICODE_STRING&);

	// Copy Assingment
	String<T>& operator=(const String<T>&);

	String<T>& operator=(const T*);

	String<T>& operator=(const PUNICODE_STRING&);

	// Destructor
	~String();

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

	// Changes the String's size.
	// If the newsize is smaller, the last elements will be lost.
	// Has a default value param for custom values when resizing.
	void Resize(size_t new_size, T val = T());

	// Returns the size of the String (number of elements). 
	size_t Size() const;

	// Returns the maximum number of elements the String can hold
	size_t MaxSize() const;

	// Reduces capcity to fit the size
	void ShrinkToFit();

	/*----------------------------*/





	/* -------- MODIFIERS --------*/

	// Removes all elements from the String
	// Capacity is not changed.
	void Clear();

	// Inserts element at the back
	void PushBack(const T& d);

	// Removes the last element from the String
	void PopBack();

	// Append a String to the back.
	void Append(const String<T>& v);

	// Add a String to the back.
	String<T>& operator+=(const String<T>&);

	// Add a C String to the back.
	String<T>& operator+=(const T*);

	// Combination of 2 Strings
	String<T> operator+(const String<T>&);

	/*----------------------------*/





	/* ----- ELEMENT ACCESS ----- */

	// Access elements with bounds checking
	T& At(size_t n);

	// Access elements with bounds checking for constant Strings.
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

	// Returns a posize_ter to the array used by String
	T* Data();

	// Returns a posize_ter to the array used by String
	const T* Data() const;

	/*----------------------------*/



	/* -------- COMPARISON -------*/

	// Check if the string is a prefix of another string
	bool IsPrefixOf(const String<T>&);

	// Check if the string is a suffix of another string
	bool IsSuffixOf(const String<T>&);

	// Overloading the equal operator
	bool operator==(const String<T>&);

	// Overloading the unequal operator
	bool operator!=(const String<T>&);

	// Overloading the greater operator
	bool operator>(const String<T>&);

	// Overloading the smaller operator
	bool operator<(const String<T>&);

	/*----------------------------*/

protected:
	T* Allocate(size_t);
	void Deallocate();

private:
	size_t	size_ = 0;		// Number of elements in String
	T* elements_ = nullptr;	// Posize_ter to first element of String
	size_t	space_ = 0;		// Total space used by String including
						// elements and free space.
};



template<class T> class String<T>::iterator
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
inline String<T>::String()
	:size_(0), elements_(nullptr), space_(0)
{}


template<class T>
inline String<T>::String(size_t size)
	: size_(size), elements_(Allocate(size)), space_(size)
{
	for (size_t index = 0; index < size_; ++index)
		elements_[index] = T();
}


template<class T>
inline String<T>::String(const String& str)
	:size_(str.size_), elements_(Allocate(str.size_)), space_(str.size_)
{
	for (size_t index = 0; index < str.size_; ++index)
		elements_[index] = str.elements_[index];
}

template<class T>
inline String<T>::String(const T* cstr)
{
	int cstr_size = 0;
	for (; cstr[cstr_size] != T(); ++cstr_size);

	elements_ = Allocate(cstr_size);
	size_ = cstr_size;
	space_ = cstr_size;

	for (size_t index = 0; index < cstr_size; ++index)
		elements_[index] = cstr[index];
}

template<>
inline String<WCHAR>::String(const UNICODE_STRING& uni_str)
	:size_(uni_str.Length / sizeof(WCHAR)), elements_(Allocate(uni_str.MaximumLength / sizeof(WCHAR))), space_(uni_str.MaximumLength / sizeof(WCHAR))
{
	MemCopy(elements_, uni_str.Buffer, uni_str.Length);
	elements_[uni_str.Length] = 0;
}

template<>
inline String<char>::String(const UNICODE_STRING& uni_str)
	:size_(uni_str.Length / sizeof(WCHAR)), elements_(Allocate(uni_str.MaximumLength / sizeof(WCHAR))), space_(uni_str.MaximumLength / sizeof(WCHAR))
{
	for (int i = 0; i < uni_str.Length; i++)
	{
		elements_[i] = (char)uni_str.Buffer[i];
	}
	elements_[uni_str.Length] = 0;
}


template<class T>
inline String<T>& String<T>::operator=(const String<T>& str)
{
	if (this == &str) return *this;	// Self-assingment not work needed

									// Current String has enough space, so there is no need for new allocation
	if (str.size_ <= space_)
	{
		for (size_t index = 0; index < str.size_; ++index)
		{
			elements_[index] = str.elements_[index];
		}
		size_ = str.size_;
		return *this;
	}

	T* p = Allocate(str.size_);

	for (size_t index = 0; index < str.size_; ++index)
	{
		p[index] = str.elements_[index];
	}

	Deallocate();
	size_ = str.size_;
	space_ = str.size_;
	elements_ = p;
	return *this;
}

template<class T>
inline String<T>& String<T>::operator=(const T* cstr)
{
	size_t cstr_size = 0;
	for (; cstr[cstr_size] != T(); ++cstr_size);

	// Current String has enough space, so there- is no need for new allocation
	if (cstr_size <= space_)
	{
		for (size_t index = 0; index < cstr_size; ++index)
		{
			elements_[index] = cstr[index];
		}
		size_ = cstr_size;
		return *this;
	}

	T* p = Allocate(cstr_size);

	for (size_t index = 0; index < cstr_size; ++index)
	{
		p[index] = cstr[index];
	}

	Deallocate();
	size_ = cstr_size;
	space_ = cstr_size;
	elements_ = p;
	return *this;

}

template<>
inline String<WCHAR>& String<WCHAR>::operator=(const PUNICODE_STRING& uni_str)
{
	Deallocate();
	size_ = uni_str->Length / sizeof(WCHAR);
	elements_ = Allocate(uni_str->MaximumLength / sizeof(WCHAR));
	space_ = uni_str->MaximumLength / sizeof(WCHAR);

	MemCopy(elements_, uni_str->Buffer, uni_str->Length);
	elements_[uni_str->Length] = 0;

	return *this;
}

template<class T>
String<T>::~String()
{
	Deallocate();
}



// Iterators
template<class T>
inline typename String<T>::iterator String<T>::Begin()
{
	return String<T>::iterator(&elements_[0]);
}

template<class T>
inline const String<T>::iterator String<T>::Begin() const
{
	return String<T>::iterator(&elements_[0]);
}

template<class T>
inline String<T>::iterator String<T>::End()
{
	return String<T>::iterator(&elements_[size_]);
}

template<class T>
inline const String<T>::iterator String<T>::End() const
{
	return String<T>::iterator(&elements_[size_]);
}

template<class T>
inline const String<T>::iterator String<T>::ConstBegin() const
{
	return String<T>::iterator(&elements_[0]);
}

template<class T>
inline const String<T>::iterator String<T>::ConstEnd() const
{
	return String<T>::iterator(&elements_[size_]);
}



// Capacity
template<class T>
inline bool String<T>::Empty() const
{
	return (size_ == 0);
}

template<class T>
inline size_t String<T>::Capacity() const
{
	return space_;
}

template<class T>
inline void String<T>::Reserve(size_t new_size)
{
	if (new_size <= space_) return;

	T* p = Allocate(new_size);

	for (size_t i = 0; i < size_; ++i)
		p[i] = elements_[i];

	Deallocate();

	elements_ = p;

	space_ = new_size;
}

template<class T>
inline void String<T>::Resize(size_t new_size, T val)
{
	Reserve(new_size);

	for (size_t index = size_; index < new_size; ++index)
		elements_[index] = T();

	size_ = new_size;
}

template<class T>
inline size_t String<T>::Size() const
{
	return size_;
}



template<class T>
inline void String<T>::Clear()
{
	for (size_t index = 0; index < size_; ++index)
		elements_[index] = T();
	space_ += size_;
	size_ = 0;
}

// Modifiers
template<class T>
inline void String<T>::PushBack(const T& ele)
{
	if (space_ == 0)
		Reserve(8);
	else if (size_ == space_)
		Reserve(2 * space_);

	elements_[size_] = ele;

	++size_;
}

template<class T>
inline void String<T>::Append(const String<T>& str)
{
	Reserve(max(str.Size(), size_) * 2);

	for (size_t index = 0; index < str.Size(); ++index)
		elements_[index + size_] = str[index];

	size_ += str.Size();

}

template<class T>
inline String<T>& String<T>::operator+=(const String<T>& str)
{
	Append(str);
	return *this;
}

template<class T>
inline String<T> String<T>::operator+(const String<T>& str)
{
	// TODO: insert return statement here
	String<T> res;
	res.Reserve(max(str.Size(), size_) * 2);
	res.Resize(str.Size() + size_);

	for (size_t index = 0; index < size_; ++index)
	{
		res[index] = elements_[index];
	}

	for (size_t index = 0; index < str.Size(); ++index)
	{
		res[index + size_] = str[index];
	}
	return res;
}


// Accessors
template<class T>
inline T& String<T>::At(size_t n)
{
	return elements_[n];
}

template<class T>
inline const T& String<T>::At(size_t n) const
{
	return elements_[n];
}

template<class T>
inline T& String<T>::operator[](size_t i)
{
	return elements_[i];
}

template<class T>
inline const T& String<T>::operator[](size_t i) const
{
	return elements_[i];
}

template<class T>
inline T& String<T>::Front()
{
	return elements_[0];
}

template<class T>
inline const T& String<T>::Front() const
{
	return elements_[0];
}

template<class T>
inline T& String<T>::Back()
{
	return elements_[size_ - 1];
}

template<class T>
inline const T& String<T>::Back() const
{
	return elements_[size_ - 1];
}

template<class T>
inline T* String<T>::Data()
{
	return elements_;
}

template<class T>
inline const T* String<T>::Data() const
{
	return elements_;
}

template<class T>
inline bool String<T>::IsPrefixOf(const String<T>& str)
{
	if (size_ > str.size_)
	{
		return false;
	}

	for (size_t i = 0; i < size_; ++i)
	{
		if (elements_[i] != str.elements_[i])
		{
			return false;
		}
	}

	return true;
}

template<class T>
inline bool String<T>::IsSuffixOf(const String<T>& str)
{
	if (size_ > str.size_)
	{
		return false;
	}

	for (size_t i = 0; i < size_; ++i)
	{
		if (elements_[i] != str.elements_[str.size_ - size_ + i])
		{
			return false;
		}
	}

	return true;
}

template<class T>
inline bool String<T>::operator==(const String<T>& str)
{
	if (size_ != str.Size())
	{
		return false;
	}

	for (size_t index = 0; index < size_; ++index)
	{
		if (elements_[index] != str[index])
		{
			return false;
		}
	}

	return true;

}

template<class T>
inline T* String<T>::Allocate(size_t n)
{
	T* p = (T *)krnl_std::Alloc( (n + 1) * sizeof(T));
	p[n] = T();
	return p;
}

template<class T>
inline void String<T>::Deallocate()
{
	if (elements_ == nullptr)
	{
		return;
	}
	krnl_std::Free(elements_);
	elements_ = 0;
}
