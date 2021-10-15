cmake_minimum_required (VERSION 3.12)
project ("2020" CXX)

# Required C++ standard
set(CMAKE_CXX_STANDARD 20)
# Disable fallback in case the compiler doesn't support the standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Single configuration
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

# CPP files
add_executable (2020
"src/main.cpp"
"src/01.cpp"
"src/02.cpp"
"src/03.cpp"
"src/04.cpp"
"src/05.cpp"
"src/06.cpp"
"src/07.cpp"
"src/08.cpp"
"src/09.cpp"
"src/10.cpp"
"src/11.cpp"
"src/12.cpp"
"src/13.cpp"
"src/14.cpp"
"src/15.cpp"
"src/16.cpp"
"src/17.cpp"
"src/18.cpp"
"src/19.cpp"
"src/20.cpp"
"src/21.cpp"
"src/22.cpp"
"src/23.cpp"
"src/24.cpp"
"src/25.cpp")
