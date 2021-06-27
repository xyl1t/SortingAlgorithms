#include "heap.hpp"
#include <algorithm>

template<typename T>
Heap<T>::Heap() : data() {}

template<typename T>
Heap<T>::Heap(std::initializer_list<T> list) : data(list) {}

template<typename T>
void Heap<T>::add(const T& element) {
	data.push_back(element);
}

template<typename T>
void Heap<T>::remove(const T& element) {
	data.erase(std::find(data.begin(), data.end(), element));
}

template<typename T>
void Heap<T>::remove(int index) {
	data.erase(data.begin() + index);
}

template<typename T>
void Heap<T>::removeLast() {
	data.erase(data.end());
}

template<typename T>
void Heap<T>::heapify() {
	
	
	
}
