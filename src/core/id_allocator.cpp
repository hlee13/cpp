/*
*  File    : id_allocator.cpp
*  Author  : 
*  Version : 
*  Company : 
*  Contact : 
*  Date    : 2016-07-29 20:27:17
*/

#include "id_allocator.h"

IDAllocator::IDAllocator() {
    _current_max_new_id = 0;
}

IDAllocator::~IDAllocator() {
}

IDAllocator::NewIdType IDAllocator::allocate_id(OldIdType old_id) {
    auto it = _old_id2new_id_map.find(old_id);
    if (_old_id2new_id_map.find(old_id) != _old_id2new_id_map.end()) {
        return it->second;
    } else {
        //allocate a new id
        _old_id2new_id_map[old_id] = _current_max_new_id;
        _new_id2old_id_map[_current_max_new_id] = old_id;

        _current_max_new_id++;
        return _current_max_new_id - 1;
    }
}

const IDAllocator::Old2NewIDMap& IDAllocator::get_o2n_id_map() {
    return _old_id2new_id_map;
}

const IDAllocator::New2OldIDMap& IDAllocator::get_n2o_id_map() {
    return _new_id2old_id_map;
}
