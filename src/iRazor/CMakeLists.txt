#--------------------------------------------------------
# The CMakeLists.txt for:                       iRazor
# Author(s):                              Nick Rypkema
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Razor.cpp
  Razor_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(iRazor ${SRC})

TARGET_LINK_LIBRARIES(iRazor
   ${MOOS_LIBRARIES}
   boost_system
   mbutil
   m
   pthread)

