# Following
# http://stackoverflow.com/a/29214327/3214598

file(GLOB_RECURSE HEADER_FILES include/tclap/*.h)

ivw_group("Header Files" BASE include/tclap ${HEADER_FILES})
source_group("CMake Files" FILES CMakeLists.txt)

add_library(tclap INTERFACE)
add_library(inviwo::tclap ALIAS tclap)
target_include_directories(tclap INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

#  HACK: have the files showing in the IDE, under the name 'flags'
add_custom_target(tclap_dummy SOURCES ${HEADER_FILES})
set_target_properties(tclap_dummy PROPERTIES VERSION 1.2.1 SOVERSION 1.2)

# Creates VS folder structure
ivw_folder(tclap_dummy ext)