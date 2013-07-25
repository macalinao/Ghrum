CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Ghrum)

# Where to save the output files.
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build")

# Boost configuration.
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON) 

# Try to find Boost and set boost enviroment.
FIND_PACKAGE(Boost COMPONENTS log log_setup thread system filesystem program_options REQUIRED)
IF (NOT Boost_FOUND)
   MESSAGE(FATAL_ERROR "Download and install Boost from www.boost.org")
ENDIF()
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIR})

SET(CMAKE_CXX_FLAGS "-O3")        ## Optimize
SET(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
ADD_DEFINITIONS( -DDLL_EXPORT )	  ## Enable DLL Export table.
ADD_DEFINITIONS( -std=c++0x   )   ## Enable C++0x mode
ADD_DEFINITIONS( -D_GHRUM_USE_BOOST  ) ## Use BOOST (Enabled on GhrumAPI)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

MESSAGE("Output directory >> ${EXECUTABLE_OUTPUT_PATH}")
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMakeFinds")

# Add the project includes.
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/../GhrumAPI/include")

# Add the project files.
FILE(GLOB_RECURSE SOURCES 
   "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

ADD_EXECUTABLE(Ghrum ${SOURCES})

# Set the target libraries.
TARGET_LINK_LIBRARIES( Ghrum ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} pthread rt)
