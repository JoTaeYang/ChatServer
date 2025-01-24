# CMake 프로그램의 최소 버전
cmake_minimum_required(VERSION 3.3)

# 프로젝트 정보
project(
	ChatServer
	VERSION 0.1
	LANGUAGES CXX)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 디렉토리의 모든 .cpp 파일을 검색
file(GLOB_RECURSE  SRC_FILES src/*.cpp)

file(GLOB_RECURSE  HEADER_FILES include/*.h)

# 실행 파일 생성
add_executable(ChatServer ${SRC_FILES} ${HEADER_FILES})

target_include_directories(ChatServer PRIVATE include)

# 소스 파일 필터링
foreach(FILE_PATH IN LISTS SRC_FILES)
    # 경로에서 디렉토리 이름 추출 
    get_filename_component(PARENT_DIR "${FILE_PATH}" DIRECTORY)

    # src/ 이후의 경로를 추출 (Visual Studio 필터로 사용)
    string(REPLACE "${CMAKE_SOURCE_DIR}/src/" "" GROUP_NAME "${PARENT_DIR}")

    # 파일을 그룹에 추가
    source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
endforeach()

# 헤더 파일 필터링
foreach(FILE_PATH IN LISTS HEADER_FILES)
    # 경로에서 디렉토리 이름 추출 
    get_filename_component(PARENT_DIR "${FILE_PATH}" DIRECTORY)

    # src/ 이후의 경로를 추출 (Visual Studio 필터로 사용)
    string(REPLACE "${CMAKE_SOURCE_DIR}/include/" "" GROUP_NAME "${PARENT_DIR}")

    # 파일을 그룹에 추가
    source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
endforeach()


# yaml-cpp를 서브 디렉토리로 추가하기 전에 CMake 옵션 설정
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Disable yaml-cpp tools" FORCE)

# yaml-cpp 서브 디렉토리 추가
add_subdirectory(external/yaml-cpp)

# ChatServer 타겟에 yaml-cpp 연결
target_link_libraries(ChatServer PRIVATE yaml-cpp)
