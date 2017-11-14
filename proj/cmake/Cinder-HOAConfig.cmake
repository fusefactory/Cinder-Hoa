if( NOT TARGET Cinder-HOA )
	get_filename_component( hoa_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( hoa_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../include" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )
	get_filename_component( lib_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../lib/HoaLibrary-Light/Sources" ABSOLUTE )

	FILE(GLOB hoa_SOURCES ${hoa_SOURCE_PATH}/*.cpp)

	add_library( Cinder-HOA ${hoa_SOURCES} )

	target_include_directories( Cinder-HOA PUBLIC "${hoa_INCLUDE_PATH}" )
	target_include_directories( Cinder-HOA PUBLIC "${lib_INCLUDE_PATH}" )
	target_include_directories( Cinder-HOA SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		    find_package( cinder REQUIRED PATHS
		        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
	target_link_libraries( Cinder-HOA PRIVATE cinder )
endif()
