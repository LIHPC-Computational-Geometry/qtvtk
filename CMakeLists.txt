cmake_minimum_required(VERSION 3.12)

# Rem : langage C ajouté pour VTK 9 car sinon il refuse de charger MPI_C demandé par ParallelMPI ...
project (QtVtk CXX C)

include (GNUInstallDirs)
include (cmake/version.cmake)
include (cmake/common.cmake)


set (QtVtk_VERSION ${QT_VTK_VERSION})

set (CMAKE_VERBOSE_MAKEFILE ON)
option (BUILD_SHARED_LIBS "Creation de bibliotheques dynamiques (defaut : ON)" ON)
add_subdirectory (src)
