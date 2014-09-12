option(LIBEV_USE_SYSTEM "Use the system's libev.so?  Recommend OFF for embedded.")

include(ExternalProject)

if (LIBEV_USE_SYSTEM)
	find_library(ev REQUIRED)
else()
	ExternalProject_add(libev-external
		URL http://dist.schmorp.de/libev/Attic/libev-4.15.tar.gz
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
		BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
		)
	set(LIBEV_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/include PARENT_SCOPE)
	add_library(libev STATIC IMPORTED GLOBAL)
	set_property(TARGET libev PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/lib/libev.a)
	add_dependencies(libev libev-external)
endif()
