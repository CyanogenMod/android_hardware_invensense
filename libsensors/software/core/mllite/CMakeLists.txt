## CMakeLists for mlsdk/mldmp

project(mldmp C)

if (NOT CMAKE_BUILD_ENGINEERING)

#    # just copy the library from mldmp/mpl/$PLATFORM to mldmp/
#    if (CMAKE_SYSTEM_NAME MATCHES Windows)
#        execute_process(
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/mpl/win32/mpl.lib ${CMAKE_CURRENT_BINARY_DIR}/mpl.lib
#        )
#    elseif (CMAKE_SYSTEM_NAME MATCHES Android)
#        execute_process(
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/mpl/android/libmpl.a ${CMAKE_CURRENT_BINARY_DIR}/libmpl.a
#        )
#    elseif (CMAKE_SYSTEM_NAME MATCHES Linux)
#        message("copying ${CMAKE_CURRENT_SOURCE_DIR}/mpl/linux/libmpl.a to ${CMAKE_CURRENT_BINARY_DIR}/libmpl.a")
#        execute_process(
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/mpl/linux/libmpl.a ${CMAKE_CURRENT_BINARY_DIR}/libmpl.a
#        )
#    elseif (CMAKE_SYSTEM_NAME MATCHES Catalina)
#        execute_process(
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/mpl/linux/mpl.lib ${CMAKE_CURRENT_BINARY_DIR}/libmpl.a
#        )
#    endif()
#    # better way that doesn't work for now
#    # add_custom_command(
#        # TARGET mpl
#        # PRE_BUILD
#        # COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${DDF} ${CMAKE_CURRENT_BINARY_DIR}
#        # COMMAND ${CMAKE_COMMAND} ARGS -E echo "copying ${DDF}"
#    # )
    
else (NOT CMAKE_BUILD_ENGINEERING)

  set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
  include(Engineering)

endif (NOT CMAKE_BUILD_ENGINEERING)
