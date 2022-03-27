project(LLVMOufOfTreeTemplate-project)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

find_package(Clang REQUIRED CONFIG)
include_directories(${CLANG_INCLUDE_DIRS})
link_directories(${CLANG_LIBRARY_DIRS})
add_definitions(${CLANG_DEFINITIONS})

list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
include(AddLLVM)
list(APPEND CMAKE_MODULE_PATH ${CLANG_CMAKE_DIR})
include(AddClang)

set(LLVM_LINK_COMPONENTS
Support
Analysis
)

add_llvm_executable(LLVMOufOfTreeTemplate 
test.cpp
)








