FIND_PACKAGE( Threads REQUIRED )

ENABLE_TESTING()

INCLUDE( ExternalProject )

ExternalProject_Add(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  UPDATE_COMMAND ""
  GIT_TAG e7e591764baba0a0c3c9ad0014430e7a27331d16
  INSTALL_COMMAND ""
)

SET( CMAKE_CXX_FLAGS "-DTESTDIR=\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\" ${CMAKE_CXX_FLAGS}" )

SET( TEST_NAME ${PROJECT_NAME}-tests )

# assume built-in pthreads on MacOS
IF( APPLE )
    SET( CMAKE_THREAD_LIBS_INIT "-lpthread" )
    SET( CMAKE_HAVE_THREADS_LIBRARY 1 )
    SET( CMAKE_USE_WIN32_THREADS_INIT 0 )
    SET( CMAKE_USE_PTHREADS_INIT 1 )
    SET( THREADS_PREFER_PTHREAD_FLAG ON )
ENDIF()

ExternalProject_Get_Property( googletest source_dir )
SET( GTEST_INCLUDE_DIRS ${source_dir}/googletest/include )
SET( GMOCK_INCLUDE_DIRS ${source_dir}/googlemock/include )

ExternalProject_Get_Property( googletest binary_dir )
SET( GTEST_LIBRARY_PATH  ${binary_dir}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a )
SET(GMOCK_LIBRARY_PATH ${binary_dir}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a )
SET( GTEST_MAIN_LIBRARY_PATH ${binary_dir}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a )
SET( GMOCK_MAIN_LIBRARY_PATH ${binary_dir}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main.a )
SET( GTEST_LIBRARY gtest )
SET( GMOCK_LIBRARY gmock)
SET( GTEST_MAIN_LIBRARY gtest_main )
SET( GMOCK_MAIN_LIBRARY gmock_main )

add_library( ${GTEST_LIBRARY} UNKNOWN IMPORTED )
add_library( ${GMOCK_LIBRARY} UNKNOWN IMPORTED )
add_library( ${GTEST_MAIN_LIBRARY} UNKNOWN IMPORTED )
add_library( ${GMOCK_MAIN_LIBRARY} UNKNOWN IMPORTED )

SET_TARGET_PROPERTIES( ${GTEST_LIBRARY} PROPERTIES
    IMPORTED_LOCATION ${GTEST_LIBRARY_PATH}
    IMPORTED_LINK_INTERFACE_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
SET_TARGET_PROPERTIES( ${GMOCK_LIBRARY} PROPERTIES
    IMPORTED_LOCATION ${GMOCK_LIBRARY_PATH}
    IMPORTED_LINK_INTERFACE_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
SET_TARGET_PROPERTIES( ${GTEST_MAIN_LIBRARY} PROPERTIES
    IMPORTED_LOCATION ${GTEST_MAIN_LIBRARY_PATH}
    IMPORTED_LINK_INTERFACE_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
SET_TARGET_PROPERTIES( ${GMOCK_MAIN_LIBRARY} PROPERTIES
    IMPORTED_LOCATION ${GMOCK_MAIN_LIBRARY_PATH}
    IMPORTED_LINK_INTERFACE_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
ADD_DEPENDENCIES( ${GTEST_LIBRARY} googletest )
ADD_DEPENDENCIES( ${GMOCK_LIBRARY} googletest )
ADD_DEPENDENCIES( ${GTEST_MAIN_LIBRARY} googletest )
ADD_DEPENDENCIES( ${GMOCK_MAIN_LIBRARY} googletest )

FILE(GLOB_RECURSE src_unit "test_*.cxx")

ADD_EXECUTABLE( ${TEST_NAME} ${src_unit} )
ADD_COMPILE_OPTIONS( ${TEST_NAME} ${BUILD_TYPE_COMPILE_FLAGS} )

TARGET_INCLUDE_DIRECTORIES( ${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include
                            ${TEST_NAME} PUBLIC ${pybind11_INCLUDE_DIRS}
                            ${TEST_NAME} PUBLIC ${PYTHON_INCLUDE_DIRS}
                            ${TEST_NAME} PRIVATE ${GTEST_INCLUDE_DIRS}
                            ${TEST_NAME} PRIVATE ${GMOCK_INCLUDE_DIRS} )

MESSAGE(STATUS "----- Configuring Test Build -----")
MESSAGE(STATUS "[Libraries]:")
MESSAGE(STATUS "\tAPI C++ BINDINGS: ${CMAKE_SOURCE_DIR}")
MESSAGE(STATUS "\tGTEST: ${GTEST_INCLUDE_DIRS}")
MESSAGE(STATUS "\tGMOCK: ${GMOCK_INCLUDE_DIRS}")
MESSAGE(STATUS "\tFLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "----------------------------------")

TARGET_LINK_LIBRARIES( ${TEST_NAME} PRIVATE ${PROJECT_NAME} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} )

INCLUDE( GoogleTest )
gtest_discover_tests( ${TEST_NAME} )