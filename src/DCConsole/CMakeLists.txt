
IF(DYNAMIC_OpenPointCloud)
    ADD_DEFINITIONS(-DDCCONSOLE_LIB)
ENDIF()

SET(LIB_NAME DCConsole)
SET(HEADER_PATH ${OpenPointCloud_SOURCE_DIR}/src/${LIB_NAME})

SET(SOURCES_H
    #${HEADER_PATH}/AppSettings.h

)

#QT5_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} OPTIONS "-f" )
#qt5_wrap_ui( generated_ui_list ${ui_list} )
#qt5_add_resources( generated_qrc_list ${qrc_list} )

## method 2
file( GLOB header_list *.h  impl/*.hxx)
SET(TARGET_H ${header_list})


file( GLOB cpp_list *.cpp impl/*.cpp impl/*.cxx )
SET(TARGET_SRC
    ${cpp_list}
    ${SOURCES_H_MOC}
    ${OpenPointCloud_VERSIONINFO_RC}
)


IF(CMAKE_COMPILER_IS_GNUCXX)
    # Remove -pedantic flag as it barfs on Qt headers
   STRING(REGEX REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF()

SET(TARGET_LIBRARIES
    ${TARGET_LIBRARIES}
    Qt5::Core
	Qt5::Gui 
	Qt5::Widgets 
	DCCore
)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})

SETUP_LIBRARY(${LIB_NAME})

