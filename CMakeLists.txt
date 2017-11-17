FIND_PACKAGE(YARP REQUIRED)
# add include directories
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
ADD_EXECUTABLE(demo main.cpp)
# we now add the YARP libraries to our project.
TARGET_LINK_LIBRARIES(demo ${YARP_LIBRARIES})
