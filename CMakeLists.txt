cmake_minimum_required(VERSION 3.10)
project(SudokuGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED) # or Qt6 if you prefer: change to find_package(Qt6 COMPONENTS Widgets REQUIRED) and target_link_libraries accordingly

set(SOURCES
    src/main.cpp
    src/sudoku.cpp
    src/mainwindow.cpp
)

set(HEADERS
    src/sudoku.h
    src/mainwindow.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
# if using Qt6:
# target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
