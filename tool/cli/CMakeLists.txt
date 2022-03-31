include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../)

set(CMAKE_CXX_FLAGS_RELEASE "/MT")

add_clang_tool(UnknownField-cli
  UnknownField-cli.cpp
  )
target_link_libraries(UnknownField-cli
  PRIVATE
  UnknownField-lib
  )
