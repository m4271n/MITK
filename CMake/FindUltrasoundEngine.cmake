# Finds the ultrasound engine library

# find include path
find_path(ULTRASOUNDENGINE_INCLUDE_DIR
   NAMES us_engine.h
   PATHS D:/Projects/trunk/SharedLibraries/UltrasoundEngine/
   DOC "The ultrasound engine include directory")
   
# find include path
find_path(ARMADILLO_INCLUDE_DIR
   NAMES armadillo
   PATHS D:/Projects/deploy/armadillo/5.200.2/include
   DOC "The armadillo include directory")
   
# find include path
find_path(FFTW_INCLUDE_DIR
   NAMES fftw3.h
   PATHS D:/Projects/deploy/FFTW/3.3.4/include
   DOC "The armadillo include directory")

# find libraries
find_library(ULTRASOUNDENGINE_LIBRARY
    NAMES USEngine.lib
    PATHS D:/TerasonIGTLinkServer_BUILD/PrjUltrasoundEngine/Debug
    DOC "ultrasound engine library")

# find libraries
find_library(FFTW_LIBRARY
    NAMES libfftw3-3.lib
    PATHS D:/Projects/deploy/FFTW/3.3.4/lib
    DOC "fftw library")
    
# find runtime path
find_path(ULTRASOUNDENGINE_RUNTIME_PATH
   NAMES USEngine.dll
   PATHS D:/TerasonIGTLinkServer_BUILD/PrjUltrasoundEngine/Debug/
   DOC "The ultrasound engine runtime directory")

# set the REQUIRED argument
find_package_handle_standard_args(ULTRASOUNDENGINE DEFAULT_MSG
   ULTRASOUNDENGINE_INCLUDE_DIR
   ULTRASOUNDENGINE_LIBRARY)

# set include and library variables
if(ULTRASOUNDENGINE_FOUND)
   set(ULTRASOUNDENGINE_INCLUDE_DIR ${ULTRASOUNDENGINE_INCLUDE_DIR} ${ARMADILLO_INCLUDE_DIR} ${FFTW_INCLUDE_DIR})
   set(ULTRASOUNDENGINE_LIBRARY ${ULTRASOUNDENGINE_LIBRARY} ${FFTW_LIBRARY})
   # define runtime path
   #set(ARMADILLO_RUNTIME_PATH ${ARMADILLO_ROOT_DIR}/bin/${COMPILER_VERSION} )
   #message(STATUS "Found ARMADILLO runtime path: ${ARMADILLO_RUNTIME_PATH}")
endif()


# mark some variables as advanced to hide them in the gui
MARK_AS_ADVANCED(ULTRASOUNDENGINE_INCLUDE_DIR)
MARK_AS_ADVANCED(ULTRASOUNDENGINE_LIBRARY)
