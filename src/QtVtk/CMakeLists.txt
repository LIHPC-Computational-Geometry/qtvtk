set (CURRENT_PACKAGE_NAME "QtVtk")
set (CURRENT_PACKAGE_MACRO_NAME "QT_VTK")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package (VtkContrib 5 REQUIRED)
include (${CMAKE_SOURCE_DIR}/cmake/qtvtk_common.cmake)	# Après VtkContrib qui positionne les variables VTK_7, VTK_8 et VTK_9
find_package (QtUtil 6 REQUIRED)
find_package(Qt5Core NO_CMAKE_SYSTEM_PATH)				# In order to enable moc ...
if (VTK_9)
	find_package (VTK COMPONENTS GUISupportQt IOExportGL2PS FiltersExtraction)
endif (VTK_9)

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
add_library (QtVtk ${CPP_SOURCES} ${HEADERS})
set (ALL_TARGETS QtVtk)
set_property (TARGET QtVtk PROPERTY AUTOMOC ON)
set_property (TARGET QtVtk PROPERTY VERSION ${QT_VTK_VERSION})
set_property (TARGET QtVtk PROPERTY SOVERSION ${QT_VTK_MAJOR_VERSION})
# Rem : en Qt v 4.* on utilise -DUSE_QT_WEBKIT
set (QT_VTK_PUBLIC_FLAGS "${MANDATORY_CXX_OPTIONS}")
list (APPEND QT_VTK_PUBLIC_FLAGS "${MANDATORY_VTK_OPTIONS}")
set (QT_VTK_PRIVATE_FLAGS -DQT_VTK_VERSION="${QT_VTK_VERSION}" "${MANDATORY_VTK_PRIVATE_OPTIONS}")
set (QT_VTK_PRIVATE_HELP_FLAGS )

target_include_directories (QtVtk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
if (VTK_9)
	target_link_libraries (QtVtk PUBLIC ${VTK_LIBRARIES})
elseif (VTK_8)
	target_link_libraries (QtVtk PUBLIC ${VTK_LIBRARIES})
else (VTK_8)	# => VTK_7
	if (NOT QVTK_WIDGET)
		message (FATAL_ERROR "==> UTILISEZ -DQVTK_WIDGET:BOOL=ON EN VTK 7 A LA LIGNE DE COMMANDE (vtkGenericOpenGLRenderWindow non disponible en VTK 7).")
	endif (NOT QVTK_WIDGET)
	if (USE_OPENGL_BACKEND)
		target_link_libraries (QtVtk PUBLIC vtkGUISupportQt)		# OpenGL backend, QVTKWidget
	else (USE_OPENGL_BACKEND)
		target_link_libraries (QtVtk PUBLIC vtkGUISupportQtOpenGL)	# OpenGL2 backend, vtkGenericOpenGLRenderWindow (voire QVTKWidget2)
	endif (USE_OPENGL_BACKEND)
endif(VTK_9)

target_compile_definitions (QtVtk PUBLIC ${QT_VTK_PUBLIC_FLAGS})
target_compile_definitions (QtVtk PRIVATE ${QT_VTK_PRIVATE_FLAGS} ${QT_VTK_PRIVATE_HELP_FLAGS})
target_compile_options (QtVtk PRIVATE ${SHARED_CFLAGS})	# Requested by Qt ...
target_link_libraries (QtVtk PUBLIC QtUtil::QtUtil VtkContrib::VtkContrib)

# Etre capable une fois installée de retrouver vtk*, ... :
# (Rem : en son absence on a Set runtime path of "/tmp/pignerol/install/lib/libQtVtk.so.5.0.0" to "") ...
set_target_properties (QtVtk PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/QtVtkDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/QtVtkDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/QtVtkDistribution.tmp  ${CMAKE_CURRENT_BINARY_DIR}/QtVtkDistribution.h)

# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# La génération des règles d'installation :
install (TARGETS ${ALL_TARGETS} EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
         
# Recopie des en-têtes dans le répertoire d'installation :
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})
# Dont QtVtkDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QtVtkDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${QT_VTK_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de QtVtk :
set (INCLUDE_INSTALL_DIR include/)			# QtVtk_INCLUDE_DIR avec QtVtkConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})		# QtVtk_LIB_DIR avec QtVtkConfig.cmake.in
set (RUNTIME_INSTALL_DIR bin/)				# QtVtk_RUNTIME_DIR avec QtVtkConfig.cmake.in
# Création du fichier QtVtkConfig.cmake qui définira QtVtk_INCLUDE_DIR, QtVtk_LIB_DIR et QtVtk_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier QtVtkTargets.cmake dans prefix/lib/QtVtk/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
# Copie des fichiers QtVtkConfig.cmake et QtVtkConfigVersion.cmake dans prefix/lib/QtVtk/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)


