/*
*  File    : rd_object.h
*  Author  : 
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-21 00:46:49
*/

#ifndef _RD_VECTOR_H_
#define _RD_VECTOR_H_

#include "rd_matrix.hpp"

namespace rd {

#define IDX_T uint32_t

// All the expected vector operations are also implemented:
// 
//     v1 = v2 + v3
//     v1 = v2 - v3
//     v1 = v2 * scale
//     v1 = scale * v2
//     v1 = -v2
//     v1 += v2 and other augmenting operations
//     v1 == v2, v1 != v2

template <typename T_DType, IDX_T dimension>
struct Vector : public Matrix<T_DType, dimension, 1> {
    Vector(...) { 
        va_list args;

        va_start(args, NULL);
        for (IDX_T i = 0; i < dimension; i++) {
            T_DType val = va_arg(args, T_DType);

            Matrix<T_DType, dimension, 1>::dim[i] = val;
        }
        va_end(args);
    }
};

}
#endif

