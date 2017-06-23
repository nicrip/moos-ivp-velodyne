#--------------------------------------------------------
# The CMakeLists.txt for:                       pVelodyneHDLDecoder
# Author(s):                              Nick Rypkema
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  VelodyneHDLDecoder.cpp
  VelodyneHDLDecoder_Info.cpp
  main.cpp
)

INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})

ADD_EXECUTABLE(pVelodyneHDLDecoder ${SRC})

TARGET_LINK_LIBRARIES(pVelodyneHDLDecoder
   ${MOOS_LIBRARIES}
   /home/athena/VelodyneHDL/build/libPacketDecoder.so
   boost_system
   mbutil
   m
   pthread
)

