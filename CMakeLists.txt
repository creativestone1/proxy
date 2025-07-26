cmake_minimum_required(VERSION 3.8)
project(my_project)

# Генерируем compile_commands.json для LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Создаём исполняемый файл из исходников (на заголовочные файлы можно убрать, они не компилируются)
add_executable(${PROJECT_NAME} ${SOURCES})

# Указываем папки для поиска заголовков — подключаем include и дополнительные include внутри include
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Настройка путей вывода собранных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)   # для exe
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)   # для shared libs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)   # для static libs

