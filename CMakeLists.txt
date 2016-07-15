cmake_minimum_required (VERSION 2.6)
project (dev_lib)

set(ROOT "/Users/didi/WorkSpace/gitlab/")
set(CMAKE_CXX_FLAGS "-g -pipe -std=c++0x -W -Wall -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(PROJECT_DIR /Users/didi/WorkSpace/CWorkSpace/dev_lib/)

set(OPENCV_MODULE_INCLUDE "/usr/local/include/opencv/"
    "/usr/local/include/opencv2/")
file(GLOB_RECURSE OPENCV_MODULE_LIB "/usr/local/lib/libopencv*.dylib")

set(LOG4CPLUS_MODULE ${ROOT}/third-64/log4cplus)
set(LOG4CPLUS_MODULE_INCLUDE ${LOG4CPLUS_MODULE}/include)
file(GLOB_RECURSE LOG4CPLUS_MODULE_LIB "${LOG4CPLUS_MODULE}/lib/*.a")

set(THRIFT_MODULE ${ROOT}/third-64/thrift)
set(THRIFT_MODULE_INCLUDE ${THRIFT_MODULE}/include)
file(GLOB_RECURSE THRIFT_MODULE_LIB "${THRIFT_MODULE}/lib/*.a")

set(BOOST_MODULE ${ROOT}/third-64/boost)
set(BOOST_MODULE_INCLUDE ${BOOST_MODULE}/include)
file(GLOB_RECURSE BOOST_MODULE_LIB "${BOOST_MODULE}/lib/*.a")

set(GTEST_MODULE ${ROOT}/third-64/gtest)
set(GTEST_MODULE_INCLUDE ${GTEST_MODULE}/include)
file(GLOB_RECURSE GTEST_MODULE_LIB "${GTEST_MODULE}/lib/*.a")

set(GMOCK_MODULE ${ROOT}/third-64/gmock)
set(GMOCK_MODULE_INCLUDE ${GMOCK_MODULE}/include)
file(GLOB_RECURSE GMOCK_MODULE_LIB "${GMOCK_MODULE}/lib/*.a")

#set(PROTO_COMPILER /usr/local/bin/protoc)
#set(PROTO_GEN_DIR ${PROJECT_DIR}/src)
#file(GLOB_RECURSE THIRD_PROTO_FILE "*.proto")
#FOREACH(PROTO_FILE ${THIRD_PROTO_FILE})
#    message ("proto file " ${PROTO_FILE})
#    EXECUTE_PROCESS(
#        COMMAND ${PROTO_COMPILER} ${PROTO_FILE} --cpp_out ${PROTO_GEN_DIR} -I${PROTO_GEN_DIR}
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        )
#ENDFOREACH(PROTO_FILE ${THIRD_PROTO_FILE})

#set(THRIFT_COMPILER ${THRIFT_MODULE}/bin/thrift)
#file(GLOB_RECURSE SERVICE_THRIFT_FILE "*.thrift")
#message ("thrift file" ${SERVICE_THRIFT_FILE})
#
#foreach (MY_THRIFT ${SERVICE_THRIFT_FILE})
#    file(RELATIVE_PATH file_name ${PROJECT_DIR} ${MY_THRIFT})
#    execute_process(COMMAND ${THRIFT_COMPILER} --gen cpp -o ${PROJECT_DIR}/output  ${MY_THRIFT})
#    file(GLOB_RECURSE SKELETON_FILE "${PROJECT_DIR}/output/gen-cpp/*.skeleton.cpp")
#    file(REMOVE ${SKELETON_FILE})
#endforeach(MY_THRIFT)

file(GLOB_RECURSE CORE_HEADERS
        "src/core/*.hpp"
        "src/core/*.h")
file(GLOB_RECURSE CORE_SOURCES 
        "src/core/*.cpp")

file(GLOB_RECURSE SERVICE_SOURCES 
        "src/*.cpp"
        "src/service/*.cpp"
        "src/core/*.cpp")

include_directories(
    ${LOG4CPLUS_MODULE_INCLUDE}
    ${BOOST_MODULE_INCLUDE}
    ${THRIFT_MODULE_INCLUDE}
    ${GTEST_MODULE_INCLUDE}
    ${GMOCK_MODULE_INCLUDE}
    )

add_executable(dev_lib-service ${SERVICE_SOURCES})

target_link_libraries(dev_lib-service
    ${LOG4CPLUS_MODULE_LIB}
    ${BOOST_MODULE_LIB}
    ${THRIFT_MODULE_LIB}
    ${GTEST_MODULE_LIB}
    ${GMOCK_MODULE_LIB}
    pthread)

add_library(rd_dev ${CORE_SOURCES})

install(FILES ${CORE_HEADERS} DESTINATION ${PROJECT_DIR}/../include/) 
install(TARGETS rd_dev DESTINATION ${PROJECT_DIR}/../lib/) 

install(FILES ${CORE_HEADERS} DESTINATION ${PROJECT_DIR}/output/include/) 
install(TARGETS rd_dev DESTINATION ${PROJECT_DIR}/output/lib/) 
install(TARGETS dev_lib-service DESTINATION ${PROJECT_DIR}/output/bin/) 
