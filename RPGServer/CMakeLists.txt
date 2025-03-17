# CMake 프로그램의 최소 버전
cmake_minimum_required(VERSION 3.3)

# 프로젝트 정보
project(
    RPGServer
    VERSION 0.1
    LANGUAGES CXX)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 디렉토리의 모든 .cpp 파일을 검색
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)

# 실행 파일 생성
add_executable(RPGServer ${SRC_FILES} ${HEADER_FILES})

target_include_directories(RPGServer PRIVATE include)

# 소스 파일 필터링
foreach(FILE_PATH IN LISTS SRC_FILES)
    get_filename_component(PARENT_DIR "${FILE_PATH}" DIRECTORY)
    string(REPLACE "${CMAKE_SOURCE_DIR}/src/" "" GROUP_NAME "${PARENT_DIR}")
    source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
endforeach()

# 헤더 파일 필터링
foreach(FILE_PATH IN LISTS HEADER_FILES)
    get_filename_component(PARENT_DIR "${FILE_PATH}" DIRECTORY)
    string(REPLACE "${CMAKE_SOURCE_DIR}/include/" "" GROUP_NAME "${PARENT_DIR}")
    source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
endforeach()

# yaml-cpp를 서브 디렉토리로 추가하기 전에 CMake 옵션 설정
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Disable yaml-cpp tools" FORCE)

# yaml-cpp 서브 디렉토리 추가
add_subdirectory(external/yaml-cpp)

# RPGServer 타겟에 yaml-cpp 연결
target_link_libraries(RPGServer PRIVATE yaml-cpp)

# MySQL Connector 경로 설정
set(MYSQL_CONNECTOR_ROOT "${CMAKE_SOURCE_DIR}/external/mysql-connector-cpp/build")

# Debug / Release에 맞는 라이브러리 파일 선택
set(MYSQL_LIB_FILE_DEBUG "${MYSQL_CONNECTOR_ROOT}/Debug/mysqlcppconnx.lib")
set(MYSQL_LIB_FILE_RELEASE "${MYSQL_CONNECTOR_ROOT}/Release/mysqlcppconnx.lib")

# `#pragma comment(lib, ...)`를 위한 매크로 정의
add_compile_definitions(
    MYSQL_LIB_DEBUG="${MYSQL_LIB_FILE_DEBUG}"
    MYSQL_LIB_RELEASE="${MYSQL_LIB_FILE_RELEASE}"
)

# 빌드 타입에 따라 라이브러리 및 DLL 경로 설정 (Visual Studio 지원)
set(MYSQL_LIB_DIR "$<IF:$<CONFIG:Debug>,${MYSQL_CONNECTOR_ROOT}/Debug,${MYSQL_CONNECTOR_ROOT}/Release>")
set(MYSQL_LIB_FILE "${MYSQL_LIB_DIR}/mysqlcppconnx.lib")
set(MYSQL_DLL_FILE "${MYSQL_LIB_DIR}/mysqlcppconnx-2-vs14.dll")

# MySQL Connector 라이브러리 링크
target_link_directories(RPGServer PRIVATE "${MYSQL_LIB_DIR}")
target_link_libraries(RPGServer PRIVATE "${MYSQL_LIB_FILE}")

######################################################################################################################################################################

set(HIREDIS_ROOT "${CMAKE_SOURCE_DIR}/external/hiredis/install")
set(REDIS_PLUS_PLUS_ROOT "${CMAKE_SOURCE_DIR}/external/redis-plus-plus/install")

# hiredis 포함
include_directories(${HIREDIS_ROOT}/include)
link_directories(${HIREDIS_ROOT}/lib)

# redis-plus-plus 포함
include_directories(${REDIS_PLUS_PLUS_ROOT}/include)
link_directories(${REDIS_PLUS_PLUS_ROOT}/lib)

# 라이브러리 연결
target_link_libraries(RPGServer PRIVATE ${REDIS_PLUS_PLUS_ROOT}/lib/redis++.lib ${HIREDIS_ROOT}/lib/hiredis.lib)

# DLL 자동 복사 (Windows 전용)
if(WIN32)
    add_custom_command(TARGET RPGServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MYSQL_DLL_FILE}"
            "$<TARGET_FILE_DIR:RPGServer>"
    )


endif()