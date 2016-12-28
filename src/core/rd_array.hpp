/*
*  File    : logger.h
*  Author  : 
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:28:28
*/

#ifndef _RDARRAY_H_
#define _RDARRAY_H_

#include <vector>

namespace rd {

template<typename _Tp, size_t capacity = 64>
class Array {
protected:
    typedef std::vector<_Tp> _TList;
public:
    Array() {
    }

    ~Array() {
        for (size_t idx = 0, end_idx = array_.size(); idx < end_idx; ++idx) {
            if (array_[idx] != NULL) {
                array_[idx] = NULL;
                delete array_[idx];
            }
        }
    }

    void push_back(const _Tp& element) {
        if (array_.empty()) {
            _TList* list_ptr = new _TList;
            list_ptr->reserve(capacity);
            array_.push_back(list_ptr);
            list_ptr->push_back(element);
            return;
        }

        int last_idx = array_.size() - 1;
        _TList* last_list_ptr = array_[last_idx];

        if (last_list_ptr->size() >= capacity) {
            _TList* new_list_ptr = new _TList;
            new_list_ptr->reserve(capacity);
            array_.push_back(new_list_ptr);
            new_list_ptr->push_back(element);
            return;
        }

        last_list_ptr->push_back(element);
        return;
    }

    void swap(rd::Array<_Tp, capacity>& other) {
        array_.swap(other.array_);
    }

    size_t size() {
        if (array_.empty()) {
            return 0;
        }
        int last_idx = array_.size() - 1;

        return last_idx * capacity + array_[last_idx]->size();
    }

    bool empty() {
        return array_.empty();
    }

    _Tp& operator [](size_t idx) {
        size_t outer_idx = idx / capacity;
        size_t inner_idx = idx % capacity;

        _TList& list = *array_[outer_idx];
        return list[inner_idx];
    }
private:
    std::vector<_TList*> array_;
};

}

#endif

