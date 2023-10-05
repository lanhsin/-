cmake_minimum_required(VERSION 3.17)

project(SecurityCmodel)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_definitions (-DLOG_LEVEL=\"${LOG_LEVEL}\")

set(cmodle_srcs 
    log.cpp
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
    main.cpp ) 


add_executable(SecCmodel ${cmodle_srcs})
target_compile_options(SecCmodel PUBLIC -Wall)

install(TARGETS SecCmodel DESTINATION ${CMAKE_SOURCE_DIR}) 

