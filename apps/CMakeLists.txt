


SET(OpenPointCloud_APPLICATION_DIR ${PROJECT_SOURCE_DIR})

	
INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR} 
	${QT_QTCORE_INCLUDE_DIR} 
	${ACTUAL_3RDPARTY_DIR}/Eigen3
)

SET(TARGET_COMMON_LIBRARIES 
        Qt5::Core
        DCCore
        DCView
        DCGa
        DCGui
        DCConsole
				
)

ADD_SUBDIRECTORY(QtGuiApplication-OnlyDCView)
ADD_SUBDIRECTORY(QtGuiApplication-DCView)
