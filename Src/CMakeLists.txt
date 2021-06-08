include_directories(${ROOT_DIR}/Inc)

add_library(command_parser STATIC command_parser.cpp)
add_executable(command_parser_ex.out main.cpp)

target_link_libraries(command_parser_ex.out command_parser)

install(TARGETS command_parser DESTINATION ${ROOT_DIR}/lib)
install(TARGETS command_parser_ex.out DESTINATION ${ROOT_DIR}/bin)