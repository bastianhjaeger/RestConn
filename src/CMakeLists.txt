set(header_path "${${PROJECT_NAME}_SOURCE_DIR}/include/${PROJECT_NAME}")
set(header 
    ${header_path}/${PROJECT_NAME}.h
)

set(src 
    ${PROJECT_NAME}.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${header}
    ${src}
)
target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
         ${RestConn_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
  pthread
  curl)
  
add_library(${PROJECT_NAME}_static STATIC
  ${header}
  ${src})
target_include_directories(${PROJECT_NAME}_static
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
         ${RestConn_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_static
  pthread)


install(TARGETS ${PROJECT_NAME}
		LIBRARY DESTINATION "${main_lib_dest}"
        ARCHIVE DESTINATION "${main_lib_dest}"
        COMPONENT library)
install(FILES ${header} DESTINATION "${include_dest}")
