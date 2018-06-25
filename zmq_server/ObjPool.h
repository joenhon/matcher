#pragma once
#include <queue>  
#include <memory>  
#include <stdexcept>  

using std::queue;
using std::shared_ptr;


template <typename T>
class ObjPool {

public:
	ObjPool(int size = defaultSize) throw(std::invalid_argument, std::bad_alloc) {
		if (0 == size) {
			throw std::invalid_argument("size can't not small than zero");
		}

		mSize = size;
		allocateChunk();

	}


	shared_ptr<T> getObj() {
		if (freeList.empty()) {
			allocateChunk();
		}

		auto obj = freeList.front();
		freeList.pop();
		return obj;
	}

	void releaseObj(shared_ptr<T> obj) {
		freeList.push(obj);
	}


protected:

	queue<shared_ptr<T>> freeList;

	int mSize;

	static const int defaultSize = 30;

	void allocateChunk() {
		for (int i = 0; i<mSize; i++) {
			freeList.push(std::make_shared<T>());
		}

	}

private:
	ObjPool(const ObjPool<T> &src) = delete;
	ObjPool<T> &operator=(const ObjPool<T> &rhs) = delete;
};