cmake_minimum_required(VERSION 3.10)
project(ppVectorDB)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 项目结构配置
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 第三方库路径配置
if(NOT DEFINED THIRD_PARTY_DIR)
    set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)
endif()

# spdlog
set(SPDLOG_LIBRARY ${THIRD_PARTY_DIR}/spdlog/install/lib)
set(SPDLOG_INCLUDE_DIR ${THIRD_PARTY_DIR}/spdlog/install/include)

# faiss
SET(FAISS_LIBRARY ${THIRD_PARTY_DIR}/faiss/install/lib64/libfaiss.so)
SET(FAISS_INCLUDE_DIR ${THIRD_PARTY_DIR}/faiss/install/include)

# 处理 header-only 库
set(HTTPLIB_INCLUDE_DIR ${THIRD_PARTY_DIR}/cpp-httplib)
set(RAPIDJSON_INCLUDE_DIR ${THIRD_PARTY_DIR}/rapidjson/include)

# 添加可执行文件
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SPDLOG_INCLUDE_DIR}
    ${FAISS_INCLUDE_DIR}
    ${HTTPLIB_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIR}
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    ${FAISS_LIBRARY}
    pthread
)

# 设置编译选项
# target_compile_options(${PROJECT_NAME} PRIVATE
#     -Wall -Wextra -Wpedantic
#     $<$<CONFIG:Debug>:-O0 -g3>
#     $<$<CONFIG:Release>:-O3>
# )

# 安装目标（可选）
install(TARGETS ${PROJECT_NAME} DESTINATION bin)