set(CPACK_GENERATOR DEB)

set(CPACK_SET_DESTDIR true) # Use CMAKE_INSTALL_PREFIX

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${libscribble_VERSION})

# Debian package.
set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_DEBIAN_PACKAGE_VERSION ${libscribble_VERSION})
set(CPACK_DEBIAN_PACKAGE_SECTION "libdevel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Nicholas Ng <nickng@imperial.ac.uk>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Scribble programming library for C/C++.
 Syntax (parser) and semantics (well-formed checking) API for Scribble/Pabble programming.
 Developed for Imperial College London, see http://mrg.doc.ic.ac.uk/ for details about Session Types research.")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.doc.ic.ac.uk/~cn06/pabble")
set(CPACK_DEBIAN_PACKAGE_LICENSE "${CMAKE_HOME_DIRECTORY}/LICENSE.md")
set(CPACK_DEBIAN_PACKAGE_LICENSE "mit")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "")

include(CPack)
