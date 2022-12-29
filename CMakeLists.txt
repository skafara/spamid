cmake_minimum_required(VERSION 3.5)

project(spamid LANGUAGES C)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -ansi")

add_executable(
    spamid.exe

    src/spamid.c
    src/classifier.c
    src/structures/hashtable.c
    src/structures/vector.c
    src/utilities/arrays.c
    src/utilities/primes.c
)
