#--------------------------------------------------------
# The CMakeLists.txt for:                       iVelodyneHDL
# Author(s):                              Nick Rypkema
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  VelodyneHDL.cpp
  VelodyneHDL_Info.cpp
  main.cpp
)

INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})

ADD_EXECUTABLE(iVelodyneHDL ${SRC})

TARGET_LINK_LIBRARIES(iVelodyneHDL
   ${MOOS_LIBRARIES}
   ${MOOS_DEPEND_LIBRARIES}
   PacketDriver
   PacketDecoder
   pcap
   boost_system
   mbutil
   m
   pthread)

