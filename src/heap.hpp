#ifndef HEAP_HPP
#define HEAP_HPP

#include <vector>
#include <initializer_list>

template<typename T>
class Heap {
private:
	std::vector<T> data;
	
public:
	Heap();
	Heap(std::initializer_list<T> list);
	
	void add(const T& element);
	void remove(int index);
	void remove(const T& element);
	void removeLast();
	
	void heapify();
};

#endif