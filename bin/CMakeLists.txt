set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(circularbuffer)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})