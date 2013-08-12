# Ajustez en fonction de votre version de CMake
cmake_minimum_required (VERSION 2.8)
 
# Nom du projet
project (numbook)
 
find_package (OpenCV REQUIRED)
 
# Exécutable "numbook", compilé à partir du fichier main.c
add_executable (numbook main.c)
 
# numbook sera linké avec les bibliothèques d'OpenCV
target_link_libraries (numbook ${OpenCV_LIBS})
