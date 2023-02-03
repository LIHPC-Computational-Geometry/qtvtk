#
# common.cmake : instructions de compilation partagées par tous les CMakeLists.txt.
# include (version.cmake) ici ne fonctionnenent pas. Why ??? Ce fichier est déjà un
#fichier déjà inclus ???
# => include version.cmake avant celui-ci.
#

include (${CMAKE_SOURCE_DIR}/cmake/organization.cmake)


# Compilation :
add_definitions (-DQT_VTK_VERSION="${QT_VTK_VERSION}")
set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions (-DNDEBUG)
endif()

# Rem : des flags VTK importants sont hérités de VtkContrib :
set (MANDATORY_CXX_OPTIONS -DMULTITHREADED_APPLICATION -DXERCES_3)

if (QVTK_WIDGET)
	message (STATUS "=============== > Utilisation de la classe QVTKWidget (obsolète) pour afficher la scène.")
	if (USE_OPENGL_BACKEND)
		set (MANDATORY_VTK_OPTIONS -DVTKGL -DVTK_WIDGET -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	else (USE_OPENGL_BACKEND)
		set (MANDATORY_VTK_OPTIONS -DVTKGL -DVTK_WIDGET2 -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	endif (USE_OPENGL_BACKEND)
else (QVTK_WIDGET)
	if (VTK_7)
		message (STATUS "=============== > Utilisation de la classe QVTKWidget2 pour afficher la scène.")
		set (MANDATORY_VTK_OPTIONS -DVTKGL -DVTK_WIDGET2 -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	else (VTK_7)
		message (STATUS "=============== > Utilisation de la classe QVTKOpenGLWidget pour afficher la scène.")
		set (MANDATORY_VTK_OPTIONS -DVTKGL -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	endif (VTK_7)
endif (QVTK_WIDGET)
set (MANDATORY_VTK_PRIVATE_OPTIONS -DPATCH_VTK_MATH_SOLVE_LINEAR_SYSTEM)

# Edition des liens :
# A l'installation les RPATHS utilisés seront ceux spécifiés ci-dessous (liste
# de répertoires séparés par des ;) :
#set (CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${MT_INSTALL_SHLIB_DIR})
#set (CMAKE_BUILD_WITH_INSTALL_RPATH ON)
# CMAKE_SKIP_BUILD_RPATH : avoir le rpath dans l'arborescence de developpement
set (CMAKE_SKIP_BUILD_RPATH OFF)
set (CMAKE_SKIP_RPATH OFF)

set (CMAKE_SKIP_INSTALL_RPATH OFF)
#set (CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

# ATTENTION : enleve le build tree du rpath :
#set (CMAKE_BUILD_WITH_INSTALL_RPATH ON)
