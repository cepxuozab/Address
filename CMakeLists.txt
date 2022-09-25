cmake_minimum_required(VERSION 3.10)

project(PhoneBook CXX)
set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS KEEP_RPATHS)

set(TARGET_NAME PhoneBook )

set(PHONEBOOK_FILES iterator_range.h main.cpp phone_book.h phone_book.cpp contact.proto)

add_executable(${TARGET_NAME}
  ${PHONEBOOK_FILES}
)
set_property(TARGET ${TARGET_NAME} PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
protobuf_generate(TARGET ${TARGET_NAME})
 
# Need to use protobuf, of course
target_link_libraries(${TARGET_NAME}
  PUBLIC CONAN_PKG::protobuf
)
 
# Make sure we can find the generated headers
target_include_directories(${TARGET_NAME}
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)