/*
*  File    : logger.h
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:28:28
*/

#ifndef _MERGESORT_H_
#define _MERGESORT_H_

#include <vector>

namespace rd {

template<typename T, typename T_IDX=size_t>
void merge_sort(std::vector<T>& arr_item, std::vector<T_IDX>& arr_idx, T_IDX dim_id, T_IDX start_idx, T_IDX end_idx) {
    if (start_idx >= end_idx - 1) {
        return;
    }

    T_IDX cnt = end_idx - start_idx;
    T_IDX mid_idx = start_idx + (cnt >> 1);

    merge_sort(arr_item, arr_idx, dim_id, start_idx, mid_idx);
    merge_sort(arr_item, arr_idx, dim_id, mid_idx, end_idx);

    T_IDX s_idx = start_idx; 
    T_IDX e_idx = end_idx;

    while (s_idx < mid_idx) {
        while (s_idx < mid_idx and arr_item[arr_idx[s_idx]][dim_id] < arr_item[arr_idx[mid_idx]][dim_id]) s_idx++;
        std::swap(arr_idx[s_idx++], arr_idx[mid_idx]);

        // m_idx, e_idx 冒泡
        T_IDX m_idx = mid_idx;
        while (m_idx + 1 < e_idx and arr_item[arr_idx[m_idx]][dim_id] > arr_item[arr_idx[m_idx+1]][dim_id]) {
            std::swap(arr_idx[m_idx], arr_idx[m_idx+1]);
            m_idx++;
        }
    }
}

}

#endif

