/*
*  File    : id_allocator.h
*  Author  : 
*  Version : 
*  Company : 
*  Contact : 
*  Date    : 2016-07-29 20:27:17
*/

#ifndef _ID_ALLOCATOR_H_
#define _ID_ALLOCATOR_H_

#include <boost/unordered_map.hpp>

class IDAllocator {
public:
    IDAllocator();
    ~IDAllocator();

    typedef long long OldIdType;
    typedef int32_t NewIdType;
    typedef boost::unordered_map<OldIdType, NewIdType> Old2NewIDMap;
    typedef boost::unordered_map<NewIdType, OldIdType> New2OldIDMap;
public:
    NewIdType allocate_id(OldIdType old_id);

    const Old2NewIDMap& get_o2n_id_map();
    const New2OldIDMap& get_n2o_id_map();
private:
    Old2NewIDMap _old_id2new_id_map;
    New2OldIDMap _new_id2old_id_map;
    uint64_t _current_max_new_id;
};

#endif//_ID_ALLOCATOR_H_

