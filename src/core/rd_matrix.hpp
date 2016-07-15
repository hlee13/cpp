/*
*  File    : rd_object.h
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-21 00:46:49
*/

#ifndef _RD_MATRIX_H_
#define _RD_MATRIX_H_

namespace rd {

#define IDX_T uint32_t

template <typename T_DType, IDX_T row, IDX_T col>
struct Matrix {
    Matrix(...) {
        va_list args;

        va_start(args, NULL);
        for (IDX_T i = 0; i < row * col; i++) {
            T_DType val = va_arg(args, T_DType);
            dim[i] = val;
        }
        va_end(args);
    }

    T_DType& operator [](IDX_T idx) {
        return dim[idx];
    }

    T_DType& operator ()(IDX_T r, IDX_T c) {
        return dim[r * row + c];
    }

    T_DType dim[row * col];
};

}
#endif

