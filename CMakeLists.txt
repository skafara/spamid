cmake_minimum_required(VERSION 3.5)

project(spamid LANGUAGES C)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -ansi")

add_executable(
    spamid.exe

    src/spamid.c
    src/classifier.c
    src/hashtable.c
    src/vector.c
    src/arrays.c
    src/primes.c
)
