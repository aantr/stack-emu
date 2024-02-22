add_library(arithmetic arithmetic.cpp arithmetic_operators.cpp)

target_include_directories(arithmetic PRIVATE
                          "${PROJECT_SOURCE_DIR}/arithmetic"
                          "${PROJECT_SOURCE_DIR}/fft"
                          "${PROJECT_SOURCE_DIR}/include"
                          )

target_link_libraries(arithmetic PRIVATE fft)

add_library(arithmetic_32 arithmetic_32.cpp arithmetic_operators_32.cpp)

target_include_directories(arithmetic_32 PRIVATE
                          "${PROJECT_SOURCE_DIR}/arithmetic"
                          "${PROJECT_SOURCE_DIR}/fft"
                          "${PROJECT_SOURCE_DIR}/include"
                          )

target_link_libraries(arithmetic_32 PRIVATE fft)
target_link_libraries(arithmetic_32 PRIVATE defines)
