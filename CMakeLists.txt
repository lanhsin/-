cmake_minimum_required(VERSION 3.17)

project(SecurityCmodel)

set(cmodle_srcs 
    SNOW_3G.cpp
    ZUC.cpp
    AES_Encrypt.cpp
    eea1.cpp
    eea2.cpp
    eea3.cpp
    eia1.cpp
    eia2.cpp
    eia3.cpp
    sysio.cpp
    security.cpp
    wrapper.cpp ) 


add_executable(SecCmodel ${cmodle_srcs})
target_compile_options(SecCmodel PUBLIC -Wall)

install(TARGETS SecCmodel DESTINATION ${CMAKE_SOURCE_DIR}) 

