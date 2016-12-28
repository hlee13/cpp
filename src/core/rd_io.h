/*
*  File    : rd_io.h
*  Author  : 
*  Version : 
*  Company : 
*  Contact : 
*  Date    : 2016-07-30 11:13:40
*/

#ifndef _RD_IO_H_
#define _RD_IO_H_

#include "rd_ret_code.h"

#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>

#include <google/protobuf/io/zero_copy_stream_impl.h> 
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/coded_stream.h>

const uint32_t MAX_PB_FILE_SIZE = (2u * 1024 * 1024 * 1024 - 1);     /**< 最大的pb size 单位,字节*/
const uint32_t MAX_PB_WARNNING_FILE_SIZE =  (3 * 512 * 1024 * 1024); /**< 输入警告pb size 单位,字节*/

template<class T>
ret_code pb_read(const std::string& filename, T& pb) {
    if (access(filename.c_str(), R_OK) == -1) {
        WARN_LOG("file %s not exists", filename.c_str());
        return ERROR;
    }

    int fd = open(filename.c_str(), O_RDONLY);
    if (0 == fd) {
        WARN_LOG("open file error str:%s, fd:%d.",
                filename.c_str(), fd);
        return ERROR;
    }

    ::google::protobuf::io::ZeroCopyInputStream* input =
        new ::google::protobuf::io::FileInputStream(fd);

    ::google::protobuf::io::CodedInputStream decoder(input);
    decoder.SetTotalBytesLimit(MAX_PB_FILE_SIZE, MAX_PB_WARNNING_FILE_SIZE);

    bool success = pb.ParseFromCodedStream(&decoder);
    delete input;
    if (true != success) {
        WARN_LOG("parse from code stream fail. [file:%s]", filename.c_str());
        return ERROR;
    }

    return OK;
}

template<class T>
ret_code pb_write(const std::string& filename, const T& pb) {
    std::fstream out(filename.c_str(),
            std::ios::out | std::ios::trunc | std::ios::binary);

    if (!pb.SerializeToOstream(&out)) {
        WARN_LOG("fail to serialize %s", filename.c_str());
        return ERROR;
    }

    return OK;
}

template<class T>
ret_code load_vec_bin_file(const std::string& file_path, std::vector<T>& t_vec) {
    FILE* bin_in = NULL;

    const char * fname = file_path.c_str();
    bin_in = fopen(fname, "rb");
    if(bin_in == NULL) {
        WARN_LOG("data invalid, can't open file %s", fname);
        return ERROR;
    }

    size_t num;
    if( 1 != fread( &num, sizeof(size_t), 1, bin_in ) ) {
        fclose(bin_in);
        WARN_LOG("error while reading: %s", fname);
        return ERROR;
    }

    INFO_LOG("t_vec size:%u",num);

    t_vec.resize(num);
    if( num != fread( &t_vec[0], sizeof(T), num, bin_in ) ) {
        fclose(bin_in);
        WARN_LOG("error while reading: %s", fname);
        return ERROR;
    }

    return OK;
}

template<class T>
ret_code dump_vec_bin_file(const std::string& file_path, std::vector<T>& t_vec) {
    FILE* output_file = NULL;
    output_file = fopen(file_path.c_str(), "wb");
    if (output_file == NULL) {
        WARN_LOG("data invalid, can't open file %s",
                file_path.c_str());
        return ERROR;
    }

    size_t vec_size = t_vec.size();

    fwrite(&vec_size, sizeof(size_t), 1, output_file);

    fwrite(&t_vec[0],
            sizeof(T),
            vec_size,
            output_file);
    fclose(output_file);

    return OK;
}

#endif//_RD_IO_H_

