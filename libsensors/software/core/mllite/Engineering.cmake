## Engineering CMakeLists for software/core/mllite

include_directories(
    .
    ${SOLUTION_SOURCE_DIR}/core/mpl
    ${SOLUTION_SOURCE_DIR}/core/HAL
    ${SOLUTION_SOURCE_DIR}/driver/include
)

add_definitions (
    -DINV_INCLUDE_LEGACY_HEADERS
)

if (CMAKE_SYSTEM_NAME MATCHES Android)

    include_directories(
        ${SOLUTION_SOURCE_DIR}/core/mllite/linux
        ${SOLUTION_SOURCE_DIR}/driver/include/linux
    )
    add_definitions(
        -DLINUX
        -D_REENTRANT
    )
    set (HEADERS
        ${HEADERS}
        linux/mlos.h
        linux/ml_stored_data.h
        linux/ml_load_dmp.h
        linux/ml_sysfs_helper.h
    )
    set (SOURCES
        ${SOURCES}
        linux/mlos_linux.c
        linux/ml_stored_data.c
        linux/ml_load_dmp.c
        linux/ml_sysfs_helper.c
    )

elseif (CMAKE_SYSTEM_NAME MATCHES Linux)

    add_definitions(
        -DLINUX
        -D_REENTRANT
    )
    set (HEADERS
        ${HEADERS}
    )
    set (SOURCES
        ${SOURCES}
    )

elseif (CMAKE_SYSTEM_NAME MATCHES Windows)

    add_definitions(
        -DAIO
        -DUART_COM
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
    )
    set (HEADERS
        ${HEADERS}
    )
    set (SOURCES
        ${SOURCES}
    )

endif ()

set (HEADERS
    ${HEADERS}
    data_builder.h
    hal_outputs.h
    message_layer.h
    ml_math_func.h
    mpl.h
    results_holder.h
    start_manager.h
    storage_manager.h
)
set (SOURCES
    ${SOURCES}
    data_builder.c
    hal_outputs.c
    message_layer.c
    ml_math_func.c
    mpl.c
    results_holder.c
    start_manager.c
    storage_manager.c
)

# coveniently add this file to the resources for easy access within IDEs
set (RESOURCES
    Engineering.cmake
)

if (CMAKE_TESTING_SUPPORT)

    message("Including Testing support")
    include_directories(
        ${SOLUTION_SOURCE_DIR}/mltools/debugsupport
    )
    add_definitions(
        -DSELF_VERIFICATION
    )
    set(
        HEADERS 
        ${HEADERS}
        ${SOLUTION_SOURCE_DIR}/mltools/debugsupport/testsupport.h
    )
    set(
        SOURCES
        ${SOURCES}
        ${SOLUTION_SOURCE_DIR}/mltools/debugsupport/testsupport.c
    )

endif ()

############
## TARGET ##
############
add_library(
    mllite STATIC
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)
set_target_properties(
    mllite
    PROPERTIES CLEAN_DIRECT_OUTPUT 1
)

if (CMAKE_SYSTEM_NAME MATCHES "(Android|Linux)")

    add_library(
        mllite_shared SHARED
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
    )
    FIX_SHARED_LIBRARY_NAME("mllite_shared" "mllite")

    install (
        TARGETS mllite_shared
        DESTINATION lib
    )

endif () 


