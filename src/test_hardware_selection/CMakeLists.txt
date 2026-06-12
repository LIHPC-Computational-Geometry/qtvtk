find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common_qt.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

add_executable (hardware_selection hardware_selection.cpp QtHardwareSelectionWindow.cpp VTKHardwareSelectionInteractorStyle.cpp)
set_property (TARGET hardware_selection PROPERTY AUTOMOC ON)

set (ALL_TARGETS hardware_selection)

foreach (exe ${ALL_TARGETS})
	target_include_directories (${exe} PRIVATE ../QtVtk/public ${CMAKE_CURRENT_SOURCE_DIR})
	target_compile_options (${exe} PRIVATE ${SHARED_CFLAGS})
	target_link_libraries (${exe} PRIVATE QtVtk)
	set_target_properties (${exe} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_PACKAGE_RPATH_DIR})
endforeach ( )
