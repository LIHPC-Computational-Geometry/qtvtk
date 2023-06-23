include (${GUIToolkitsVariables_CMAKE_DIR}/organization.cmake)


# Compilation :
add_definitions (-DQT_VTK_VERSION="${QT_VTK_VERSION}")


# Rem : des flags VTK importants sont hérités de VtkContrib :
if (QVTK_WIDGET)
	message (STATUS "=============== > Utilisation de la classe QVTKWidget (obsolète, VTK < 8) pour afficher la scène.")
	if (NOT VTK_7)
		message (FATAL_ERROR "== > Utilisation de la classe QVTKWidget impossible en dehors de VTK 7.")	# Versions < VTK 7 non maintenues
	endif (NOT VTK_7)
	if (USE_OPENGL_BACKEND)
		set (MANDATORY_VTK_OPTIONS -DVTK_WIDGET -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	else (USE_OPENGL_BACKEND)
		set (MANDATORY_VTK_OPTIONS -DVTK_WIDGET2 -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	endif (USE_OPENGL_BACKEND)
else (QVTK_WIDGET)
	if (VTK_7)
		message (STATUS "=============== > Utilisation de la classe QVTKWidget2 pour afficher la scène.")
		set (MANDATORY_VTK_OPTIONS -DVTK_WIDGET2 -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	else (VTK_7)
		message (STATUS "=============== > Utilisation de la classe QVTKOpenGLWidget pour afficher la scène.")
		set (MANDATORY_VTK_OPTIONS -DEXPORT_VTK_MULTIBLOCK_DATASETS)
	endif (VTK_7)
endif (QVTK_WIDGET)
set (MANDATORY_VTK_PRIVATE_OPTIONS -DPATCH_VTK_MATH_SOLVE_LINEAR_SYSTEM)

