/*
*  File    : logger.h
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:28:28
*/

#ifndef _QUICKSORT_H_
#define _QUICKSORT_H_

namespace rd {

// template<typename T, typename PivotDataType=double, typename T_IDX=size_t>
// void quick_sort(std::vector<T>& arr_item, std::vector<T_IDX>& arr_idx, T_IDX dim_id, T_IDX start_idx, T_IDX end_idx) { 
//     if (start_idx >= end_idx) {
//         return;
//     }
// 
//     PivotDataType pivot = arr_item[arr_idx[start_idx]][dim_id];
//     T_IDX left = start_idx, right = end_idx;
// 
//     while (left < right) {
//         while (arr_item[arr_idx[right]][dim_id] > pivot and left < right) right--;
//         std::swap(arr_idx[left], arr_idx[right]);
// 
//         while (arr_item[arr_idx[left]][dim_id] <= pivot and left < right) left++;
//         std::swap(arr_idx[left], arr_idx[right]);
//     }
// 
//     if (left > start_idx) {
//         quick_sort(arr_item, arr_idx, dim_id, start_idx, left - 1);
//     }
//     if (end_idx > left) {
//         quick_sort(arr_item, arr_idx, dim_id, left + 1, end_idx);
//     }
// }

template<typename _T1, typename _T2, typename T_DIM, typename PivotDataType=double, typename T_IDX=size_t>
void quick_sort(_T1& arr_item, _T2& arr_idx, T_DIM dim_id, T_IDX start_idx, T_IDX end_idx) { 
    if (start_idx >= end_idx) {
        return;
    }

    PivotDataType pivot = arr_item[arr_idx[start_idx]][dim_id];
    T_IDX left = start_idx, right = end_idx;

    while (left < right) {
        while (arr_item[arr_idx[right]][dim_id] > pivot and left < right) right--;
        std::swap(arr_idx[left], arr_idx[right]);

        while (arr_item[arr_idx[left]][dim_id] <= pivot and left < right) left++;
        std::swap(arr_idx[left], arr_idx[right]);
    }

    if (left > start_idx) {
        quick_sort(arr_item, arr_idx, dim_id, start_idx, left - 1);
    }
    if (end_idx > left) {
        quick_sort(arr_item, arr_idx, dim_id, left + 1, end_idx);
    }
}

// template<typename T, typename PivotDataType=double, typename T_IDX=size_t>
// void nth_quick_sort(std::vector<T>& arr_item, std::vector<T_IDX>& arr_idx, T_IDX dim_id, T_IDX start_idx, T_IDX end_idx, T_IDX target_idx) {
//     if (start_idx >= end_idx) {
//         return;
//     }
// 
//     PivotDataType pivot = arr_item[arr_idx[start_idx]][dim_id];
//     T_IDX left = start_idx, right = end_idx;
// 
//     while (left < right) {
//         while (arr_item[arr_idx[right]][dim_id] > pivot and left < right) right--;
//         std::swap(arr_idx[left], arr_idx[right]);
// 
//         while (arr_item[arr_idx[left]][dim_id] <=  pivot and left < right) left++;
//         std::swap(arr_idx[left], arr_idx[right]);
//     }
// 
//     if (left == target_idx) {
//         return;
//     }
// 
//     // search left-half
//     if (left > start_idx and left > target_idx) {
//         nth_quick_sort(arr_item, arr_idx, dim_id, start_idx, left - 1, target_idx);
//     }
//     // search right-half
//     if (end_idx > left and target_idx > left) {
//         nth_quick_sort(arr_item, arr_idx, dim_id, left + 1, end_idx, target_idx);
//     }
// }

template<typename _T1, typename _T2, typename T_DIM, typename PivotDataType=double, typename T_IDX=size_t>
void nth_quick_sort(_T1& arr_item, _T2& arr_idx, T_DIM dim_id, T_IDX start_idx, T_IDX end_idx, T_IDX target_idx) {
    if (start_idx >= end_idx) {
        return;
    }

    PivotDataType pivot = arr_item[arr_idx[start_idx]][dim_id];
    T_IDX left = start_idx, right = end_idx;

    while (left < right) {
        while (arr_item[arr_idx[right]][dim_id] > pivot and left < right) right--;
        std::swap(arr_idx[left], arr_idx[right]);

        while (arr_item[arr_idx[left]][dim_id] <=  pivot and left < right) left++;
        std::swap(arr_idx[left], arr_idx[right]);
    }

    if (left == target_idx) {
        return;
    }

    // search left-half
    if (left > start_idx and left > target_idx) {
        nth_quick_sort(arr_item, arr_idx, dim_id, start_idx, left - 1, target_idx);
    }
    // search right-half
    if (end_idx > left and target_idx > left) {
        nth_quick_sort(arr_item, arr_idx, dim_id, left + 1, end_idx, target_idx);
    }
}

}

#endif

