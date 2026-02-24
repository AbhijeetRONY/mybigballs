add_library(mybigballslib src/mybigballslib.cpp)

target_include_directories(mybigballslib PUBLIC include)

install(TARGETS mybigballslib)
install(DIRECTORY include/ DESTINATION include/mybigballslib)
