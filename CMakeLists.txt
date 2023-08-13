cmake_minimum_required (VERSION 2.6)
project (sdpread)

add_executable(sdpread sdpread.c)
target_compile_options(sdpread PRIVATE -Werror -Wall -Wextra)
install(TARGETS sdpread DESTINATION sbin)

