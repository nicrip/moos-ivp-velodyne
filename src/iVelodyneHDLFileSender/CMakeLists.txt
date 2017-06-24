#--------------------------------------------------------
# The CMakeLists.txt for:                       iVelodyneHDLFileSender
# Author(s):                              Nick Rypkema
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  VelodyneHDLFileSender.cpp
  VelodyneHDLFileSender_Info.cpp
  main.cpp
)

INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})

ADD_EXECUTABLE(iVelodyneHDLFileSender ${SRC})

TARGET_LINK_LIBRARIES(iVelodyneHDLFileSender
   ${MOOS_LIBRARIES}
   boost_system
   boost_thread
   pcap
   mbutil
   m
   pthread
   rt
)

