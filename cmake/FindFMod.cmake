FIND_PATH(FMOD_INCLUDE_DIR fmod.hpp)
FIND_LIBRARY(FMOD_LIBRARY NAMES FMOD fmod PATH_SUFFIXES lib)

SET(FMOD_INCLUDE_DIRS ${FMOD_INCLUDE_DIR})
SET(FMOD_LIBRARIES ${FMOD_LIBRARY})

INCLUDE( FindPackageHandleStandardArgs )

find_package_handle_standard_args(FMOD FMOD_INCLUDE_DIR FMOD_LIBRARY)
MARK_AS_ADVANCED(FMOD_INCLUDE_DIR FMOD_LIBRARY)

GET_FILENAME_COMPONENT(FMOD_LIBRARY_DIR ${FMOD_LIBRARY} DIRECTORY)