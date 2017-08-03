##############################################################################
# FILE:        moos-ivp-velodyne/src/CMakeLists.txt
# DATE:        2017
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-velodyne source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* ../lib)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})
LINK_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(iVelodyneHDL)
ADD_SUBDIRECTORY(pVelodyneHDLDecoder)
ADD_SUBDIRECTORY(iVelodyneHDLFileSender)
ADD_SUBDIRECTORY(iRazor)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

