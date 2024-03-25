
#pragma once

#include "../memory/pair.h"
#include "../vector/vector.h"

#define RAND_MAX 0x7fff

#define _LEVELS 100

template <typename K, typename M>
class Map;

//contains the {key, value} Pair
template <typename K, typename M>
class Node {
public:
	friend class Map<K, M>;
	typedef Pair<const K, M> ValueType;

	Node(size_t level) {
		size_t _temp_level = level + 1;
		forward_links_ = new Node * [_temp_level];
		::ZeroMemory(forward_links_, _temp_level);
		value_ = nullptr;
		prev_ = nullptr;
	}

	Node(size_t level, const ValueType& val) {
		size_t _temp_level = level + 1;
		forward_links_ = new Node * [_temp_level];
		::ZeroMemory(forward_links_, _temp_level);
		value_ = new Pair<const K, M>(val);
		prev_ = nullptr;
	}

	//delete forward links and value
	~Node() {
		delete[] forward_links_;
		delete value_;
	}

	Node** forward_links_;
	Node* prev_;
	ValueType* value_;
};

//maintains a skiplists of nodes
template <typename K, typename M>
class Map {
public:
	//member type
	typedef Pair<const K, M> ValueType;

	/*
	defining Iterator class
	helps iterate through elements of the map
	*/
	class Iterator {
	public:
		Iterator(const Iterator& iter) : cur_(iter.GetCur()) {};
		Iterator(Node<K, M>* node) : cur_(node) {}
		Node<K, M>* GetCur() const { return cur_; }

		//defining operators for Iterator
		Iterator& operator=(const Iterator& iter) {
			cur_ = iter.GetCur();
			return *this;
		}
		Iterator& operator++() {
			if (cur_ == nullptr) return *this;
			cur_ = cur_->forward_links_[0];
			return *this;
		}
		Iterator operator++(int) {
			Map<K, M>::Iterator _this = *this;
			if (cur_ == nullptr) return _this;
			cur_ = cur_->forward_links_[0];
			return _this;
		}
		Iterator& operator--() {
			if (cur_ == nullptr) return *this;
			cur_ = cur_->prev_;
			return *this;
		}
		Iterator operator--(int) {
			Map<K, M>::Iterator _this = *this;
			if (cur_ == nullptr) return _this;
			cur_ = cur_->prev_;
			return _this;
		}
		ValueType& operator*() const {
			return *cur_->value_;
		}
		ValueType* operator->() const {
			return cur_->value_;
		}

		~Iterator() {
			cur_ = nullptr;
		}

	private:
		Node<K, M>* cur_;

	};

	/*
	defining ConstIterator class
	works exactly like the Iterator but values are const and cannot be changed
	*/
	class ConstIterator {
	public:
		ConstIterator(const ConstIterator& _citer) : cur_(_citer.GetCur()) {}
		ConstIterator(const Iterator& iter) : cur_(iter.GetCur()) {}
		ConstIterator(Node<K, M>* node) : cur_(node) {}
		Node<K, M>* GetCur() const { return cur_; }

		//defining operators for ConstIterator
		ConstIterator& operator=(const ConstIterator& _citer) {
			cur_ = _citer.GetCur();
			return *this;
		}
		ConstIterator& operator++() {
			if (cur_ == nullptr) return *this;
			cur_ = cur_->forward_links_[0];
			return *this;
		}
		ConstIterator operator++(int) {
			Map<K, M>::ConstIterator _this = *this;
			if (cur_ == nullptr) return _this;
			cur_ = cur_->forward_links_[0];
			return _this;
		}
		ConstIterator& operator--() {
			if (cur_ == nullptr) return *this;
			cur_ = cur_->prev_;
			return *this;
		}
		ConstIterator operator--(int) {
			Map<K, M>::ConstIterator _this = *this;
			if (cur_ == nullptr) return _this;
			cur_ = cur_->prev_;
			return _this;
		}
		const ValueType& operator*() const {
			return *cur_->value_;
		}
		const ValueType* operator->() const {
			return cur_->value_;
		}

		~ConstIterator() {
			cur_ = nullptr;
		}

	private:
		Node<K, M>* cur_;
	};

	/*
	defining ReverseIterator
	iterator through the elements of the map in reverse order
	*/
	class ReverseIterator {
	public:
		ReverseIterator(const ReverseIterator& _riter) : cur_(_riter.GetCur()) {}
		ReverseIterator(Node<K, M>* node) : cur_(node) {}
		Node<K, M>* GetCur() const { return cur_; }

		//defining operators for ReverseIterator
		ReverseIterator& operator=(const ReverseIterator& _riter) {
			cur_ = _riter.GetCur();
			return *this;
		}
		ReverseIterator& operator++() {
			if (cur_ == nullptr) return *this;
			cur_ = cur_->prev_;
			return *this;
		}
		ReverseIterator operator++(int) {
			Map<K, M>::ReverseIterator _this = *this;
			if (cur_ == nullptr) return _this;
			cur_ = cur_->prev_;
			return _this;
		}
		ReverseIterator& operator--() {
			if (cur_ == nullptr) return *this;
			cur_ = cur_->forward_links_[0];
			return *this;
		}
		ReverseIterator operator--(int) {
			Map<K, M>::ReverseIterator _this = *this;
			if (cur_ == nullptr) return _this;
			cur_ = cur_->prev_;
			return _this;
		}
		ValueType& operator*() const {
			return *cur_->value_;
		}
		ValueType* operator->() const {
			return cur_->value_;
		}

		~ReverseIterator() {
			cur_ = nullptr;
		}

	private:
		Node<K, M>* cur_;
	};

	Map() {
		MapCtor();
	}

	Map(const Map& _map) {
		MapCtor();
		Node<K, M>* temp = _map.GetHead()->forward_links_[0];
		while (temp != _map.GetTail()) {
			ValueType& val = *(temp->value_);
			Insert(val);
			temp = temp->forward_links_[0];
		}
	}

	Map(Vector<Pair<const K, M>> _l) {
		MapCtor();
		auto iter = _l.Begin();
		while (iter != _l.End()) {
			Insert(*iter);
			iter++;
		}
	}

	//delete every node in the map
	~Map() {
		Node<K, M>* temp_head = head_;
		Node<K, M>* temp;
		while (temp_head != nullptr) {
			temp = temp_head->forward_links_[0];
			delete temp_head;
			temp_head = temp;
		}
	}

	Node<K, M>* GetHead() const {
		return head_;
	}
	Node<K, M>* GetTail() const {
		return tail_;
	}

	//constructs the map
	void MapCtor() {
		InitHeadTail();
		InitAssignHeadTail();
		InitSize();
	}
	//init parameters
	void InitHeadTail() {
		head_ = new Node<K, M>(_LEVELS);
		tail_ = new Node<K, M>(_LEVELS);
	}
	void InitAssignHeadTail() {
		head_->forward_links_[0] = tail_;
		tail_->prev_ = head_;
		head_->prev_ = nullptr;
		tail_->forward_links_[0] = nullptr;
	}
	void InitSize() {
		max_ = 0;
		size_ = 0;
	}

	//reset parameters
	void ResetHeadTail() {
		head_ = nullptr;
		tail_ = nullptr;
	}
	void ResetSize() {
		max_ = 0;
		size_ = 0;
	}

	//return Iterator pointing to the node which has key given by key
	Iterator Find(const K& key) {
		Node<K, M>* temp_head = FindAtBottom(key);
		if (temp_head == nullptr) {
			return Map<K, M>::Iterator(tail_);
		}
		return Map<K, M>::Iterator(temp_head);
	}

	//return ConstIterator pointing to the node which has key given by key
	ConstIterator Find(const K& key) const {
		Node<K, M>* temp_head = FindAtBottom(key);
		if (temp_head == nullptr) {
			return Map<K, M>::ConstIterator(tail_);
		}
		return Map<K, M>::ConstIterator(temp_head);
	}

	/*
	Begin() -- return iterator pointing to the first_ element in the map
	End() -- return iterator pointing to one past the last element in the map (logically)
	RBegin() -- works like Begin() for ReverseIterator
	REnd() -- works like End() for ReverseIterator
	*/
	Iterator Begin()
	{
		return Map<K, M>::Iterator(head_->forward_links_[0]);
	};

	Iterator End()
	{
		return Map<K, M>::Iterator(tail_);
	};

	ConstIterator Begin() const
	{
		return ConstIterator(head_->forward_links_[0]);
	}

	ConstIterator End() const
	{
		return ConstIterator(tail_);
	}

	ReverseIterator RBegin()
	{
		return ReverseIterator(tail_->prev_);
	}

	ReverseIterator REnd()
	{
		return ReverseIterator(head_);
	}

	size_t Size() const
	{
		return size_;
	}

	bool Empty() const
	{
		return (size_ == 0) ? true : false;
	}

	//return mapped type for the given key
	M At(const K& key) {
		Node<K, M>* temp_head = FindAtBottom(key);
		if (temp_head == nullptr) {
			//throw std::out_of_range("out of range");
			return M();
		}
		else return temp_head->value_->second_;
	}
	//return const reference to mapped type for given key
	const M At(const K& key) const {
		Node<K, M>* temp_head = FindAtBottom(key);
		if (temp_head == nullptr) {
			//throw std::out_of_range("out of range");
			return M();
		}
		else return temp_head->value_->second_;
	}

	//comparison operators on iterator
	friend bool operator==(const Iterator& iter1, const Iterator& iter2) {
		return (iter1.GetCur() == iter2.GetCur()) ? true : false;
	}
	friend bool operator!=(const Iterator& iter1, const Iterator& iter2) {
		return (iter1.GetCur() != iter2.GetCur()) ? true : false;
	}
	friend bool operator==(const ConstIterator& _citer1, const ConstIterator& _citer2) {
		return (_citer1.GetCur() == _citer2.GetCur()) ? true : false;
	}
	friend bool operator!=(const ConstIterator& _citer1, const ConstIterator& _citer2) {
		return (_citer1.GetCur() != _citer2.GetCur()) ? true : false;
	}
	friend bool operator==(const ReverseIterator& _riter1, const ReverseIterator& _riter2) {
		return (_riter1.GetCur() == _riter2.GetCur()) ? true : false;
	}
	friend bool operator!=(const ReverseIterator& _riter1, const ReverseIterator& _riter2) {
		return (_riter1.GetCur() != _riter2.GetCur()) ? true : false;
	}
	friend bool operator==(const Iterator& iter, const ConstIterator& _citer) {
		return (iter.GetCur() == _citer.GetCur()) ? true : false;
	}
	friend bool operator==(const ConstIterator& _citer, const Iterator& iter) {
		return (_citer.GetCur() == iter.GetCur()) ? true : false;
	}
	friend bool operator!=(const Iterator& iter, const ConstIterator& _citer) {
		return (iter.GetCur() != _citer.GetCur()) ? true : false;
	}
	friend bool operator!=(const ConstIterator& _citer, const Iterator& iter) {
		return (_citer.GetCur() != iter.GetCur()) ? true : false;
	};

	//Clear all nodes in the map
	void Clear();
	//operator[] for accessing mapped type for key
	M& operator[](const K&);
	//level Randomizer for Insert
	size_t RandomLevel(Node<K, M>**);
	//getting node for key using bottom link
	Node<K, M>* FindAtBottom(const K& key) const;
	//Insert ValueType into map and return iterator positioned At that node
	Pair<Iterator, bool> Insert(const ValueType&);
	//range based Insert
	void Insert(const K& first_, const M& second_);
	//remove node pointed to by iterator
	void Erase(Iterator);
	//remove node which has given key
	void Erase(const K&);
	//assignment of maps
	Map& operator=(const Map&);
	//map comparisons
	bool operator!=(const Map&) const;
	template <typename Key, typename Mapped>
	friend bool operator==(const Map<Key, Mapped>&, const Map<Key, Mapped>&);
	template <typename Key, typename Mapped>
	friend bool operator<(const Map<Key, Mapped>&, const Map<Key, Mapped>&);
	template <typename Key, typename Mapped>
	friend bool operator!=(const Map<Key, Mapped>&, const Map<Key, Mapped>&);

private:
	Node<K, M>* head_;
	Node<K, M>* tail_;
	size_t max_;
	size_t size_;
};

template <typename K, typename M>
inline M& Map<K, M>::operator[](const K& key) {
	Node<K, M>* temp_head = FindAtBottom(key);
	if (temp_head == nullptr)
	{
		ValueType vt((const K&)key, M());
		Pair<Iterator, bool> iter1 = Insert((const ValueType&)vt);

		Map<K, M>::Iterator iter(iter1.first_);
		return iter.GetCur()->value_->second_;
	}
	return temp_head->value_->second_;
}

template <typename K, typename M>
inline size_t Map<K, M>::RandomLevel(Node<K, M>** _nodes) {
	size_t level = 0;
	while (level < _LEVELS && (short)Rand() < RAND_MAX) {
		++level;
	}
	if (level > max_) {
		size_t temp = max_ + 1;
		while (temp <= level) {
			_nodes[temp] = head_;
			++temp;
		}
		max_ = level;
	}
	return level;
}

template <typename K, typename M>
inline Map<K, M>& Map<K, M>::operator=(const Map<K, M>& _map) {
	if (this == &_map) {
		return *this;
	}
	Node<K, M>* temp_head = head_;
	Node<K, M>* temp;
	while (temp_head != nullptr) {
		temp = temp_head->forward_links_[0];
		delete temp_head;
		temp_head = nullptr;
		temp_head = temp;
	}

	MapCtor();
	Node<K, M>* first = _map.GetHead()->forward_links_[0];
	if (first == _map.GetTail()) return *this;
	while (first != _map.GetTail()) {
		Insert(*(first->value_));
		first = first->forward_links_[0];
	}
	return *this;
}

template <typename K, typename M>
inline Pair<typename Map<K, M>::Iterator, bool> Map<K, M>::Insert(const ValueType& val) {
	const K& key = val.first_;
	Node<K, M>* temp = head_;
	Node<K, M>** updated = new Node<K, M>* [_LEVELS + 1];
	::ZeroMemory(updated, (_LEVELS + 1));
	size_t i = max_;
	while (i >= 1) {
		while (temp->forward_links_[i] != nullptr && temp->forward_links_[i]->value_->first_ < key) {
			temp = temp->forward_links_[i];
		}
		updated[i] = temp;
		i--;
	}

	while (temp->forward_links_[0] != tail_ && temp->forward_links_[0]->value_->first_ < key) {
		temp = temp->forward_links_[0];
	}
	updated[0] = temp;
	Node<K, M>* first_updated = updated[0];
	first_updated = first_updated->forward_links_[0];
	if (first_updated->value_ != nullptr && first_updated->value_->first_ == val.first_) {
		delete[] updated;
		return Pair(Map<K, M>::Iterator(first_updated), false);
	}

	size_t level = RandomLevel(updated);
	first_updated = nullptr;
	first_updated = new Node<K, M>(level, val);
	i = 0;
	while (i <= level) {
		first_updated->forward_links_[i] = updated[i]->forward_links_[i];
		updated[i]->forward_links_[i] = first_updated;
		++i;
	}
	first_updated->prev_ = updated[0];
	if (first_updated->forward_links_[0] != tail_) {
		first_updated->forward_links_[0]->prev_ = first_updated;
	}
	else {
		tail_->prev_ = first_updated;
	}
	++size_;
	delete[] updated;
	return Pair(Map<K, M>::Iterator(first_updated), true);
}

template <typename K, typename M>
inline void Map<K, M>::Insert(const K& first_, const M& second_) {
	Insert(Pair<const K, M>(first_, second_));
}

template <typename K, typename M>
inline void Map<K, M>::Erase(const K& key) {
	Node<K, M>* temp = head_;
	Node<K, M>** updated = new Node<K, M>* [_LEVELS + 1];
	::ZeroMemory(updated, (_LEVELS + 1));
	size_t i = max_;
	while (i >= 1) {
		while (temp->forward_links_[i] != nullptr && temp->forward_links_[i]->value_->first_ < key) {
			temp = temp->forward_links_[i];
		}
		updated[i] = temp;
		i--;
	}

	while (temp->forward_links_[0] != tail_ && temp->forward_links_[0]->value_->first_ < key) {
		temp = temp->forward_links_[0];
	}
	updated[0] = temp;
	Node<K, M>* first_updated = updated[0];
	first_updated = first_updated->forward_links_[0];
	if (first_updated->value_->first_ == key) {
		i = 0;
		while (i <= max_ && updated[i]->forward_links_[i] == first_updated) {
			updated[i]->forward_links_[i] = first_updated->forward_links_[i];
			++i;
		}

		if (first_updated->forward_links_[0] != tail_) {
			first_updated->forward_links_[0]->prev_ = first_updated->prev_;
		}
		else {
			tail_->prev_ = first_updated->prev_;
			first_updated->prev_->forward_links_[0] = tail_;
		}

		delete first_updated;
		while (max_ > 0 && head_->forward_links_[max_] == nullptr) {
			--max_;
		}
		--size_;
		delete[] updated;
	}
	else {
		//throw std::out_of_range("out of range");
	}
}

template <typename K, typename M>
inline void Map<K, M>::Erase(Map<K, M>::Iterator iter) {
	const K& key = iter.GetCur()->value_->first_;
	Node<K, M>* temp = head_;
	Node<K, M>** updated = new Node<K, M>* [_LEVELS + 1];
	::ZeroMemory(updated, (_LEVELS + 1));
	size_t i = max_;
	while (i >= 1) {
		while (temp->forward_links_[i] != nullptr && temp->forward_links_[i]->value_->first_ < key) {
			temp = temp->forward_links_[i];
		}
		updated[i] = temp;
		i--;
	}

	while (temp->forward_links_[0] != tail_ && temp->forward_links_[0]->value_->first_ < key) {
		temp = temp->forward_links_[0];
	}
	updated[0] = temp;
	Node<K, M>* first_updated = updated[0];
	first_updated = first_updated->forward_links_[0];
	if (first_updated->value_->first_ == key) {
		i = 0;
		while (i <= max_ && updated[i]->forward_links_[i] == first_updated) {
			updated[i]->forward_links_[i] = first_updated->forward_links_[i];
			++i;
		}

		if (first_updated->forward_links_[0] != tail_) {
			first_updated->forward_links_[0]->prev_ = first_updated->prev_;
		}
		else {
			tail_->prev_ = first_updated->prev_;
			first_updated->prev_->forward_links_[0] = tail_;
		}

		delete first_updated;
		while (max_ > 0 && head_->forward_links_[max_] == nullptr) {
			--max_;
		}
		--size_;
		delete[] updated;
	}
	else {
		//throw std::out_of_range("out of range");
	}
}

template <typename K, typename M>
inline void Map<K, M>::Clear() {
	Node<K, M>* temp_head = head_;
	Node<K, M>* temp;
	while (temp != nullptr) {
		temp = temp_head->forward_links_[0];
		delete temp_head;
		temp_head = temp;
	}
	ResetSize();
	ResetHeadTail();
	MapCtor();
}

template <typename K, typename M>
inline Node<K, M>* Map<K, M>::FindAtBottom(const K& key) const {
	Node<K, M>* temp = head_;
	int i = max_;
	while (i >= 1) {
		while (temp->forward_links_[i] != nullptr && temp->forward_links_[i]->value_->first_ < key) {
			temp = temp->forward_links_[i];
		}
		i--;
	}

	while (temp->forward_links_[0] != tail_ && temp->forward_links_[0]->value_->first_ < key) {
		temp = temp->forward_links_[0];
	}
	temp = temp->forward_links_[0];
	if (temp == tail_) return nullptr;
	if (temp != nullptr) {
		if (temp->value_->first_ == key)
			return temp;
	}
	return nullptr;
}

template <typename K, typename M>
inline bool Map<K, M>::operator!=(const Map<K, M>& _map) const {
	if (*this == _map) return false;
	return true;
}

template <typename K, typename M>
inline bool operator==(const Map<K, M>& map1, const Map<K, M>& map2) {
	if (map1.Size() != map2.Size()) return false;
	auto iter1 = map1.Begin();
	auto iter2 = map2.Begin();
	while (iter1 != map1.End() && iter2 != map2.End()) {
		if (*iter1 != *iter2) {
			return false;
		}
		++iter1;
		++iter2;
	}
	return true;
}

template <typename K, typename M>
inline bool operator!=(const Map<K, M>& map1, const Map<K, M>& map2) {
	return !(map1 == map2);
}

template <typename K, typename M>
inline bool operator<(const Map<K, M>& map1, const Map<K, M>& map2) {
	size_t size1 = map1.Size();
	size_t size2 = map2.Size();
	if (size1 < size2) return true;
	if (size2 < size1) return false;
	//same Size
	auto iter1 = map1.Begin();
	auto iter2 = map2.Begin();
	while (iter1 != map1.End() && iter2 != map2.End()) {
		bool less = (*iter1).first_ < (*iter2).first_;
		bool less2 = (*iter2).first_ < (*iter1).first_;
		if (less) return true;
		if (less2) return false;
		++iter1;
		++iter2;
	}
	//maps are same
	return false;
}
