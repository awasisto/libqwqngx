# Debian
if("${PACKAGE}" STREQUAL "Debian")

   # Settings
   set(REVISION                           0)
   set(CPACK_GENERATOR                    "DEB" PARENT_SCOPE)
   set(CPACK_PACKAGE_VERSION              ${CPACK_PACKAGE_VERSION}-${REVISION} PARENT_SCOPE)

   # Dependencies
   set(CPACK_DEBIAN_PACKAGE_DEPENDS       "libftdi1" PARENT_SCOPE)
   set(DEBIAN_PACKAGE_BUILDS_DEPENDS      "cmake, libftdi1-dev" PARENT_SCOPE)

   # Bundles
   message("-- Installing udev rules to /etc/udev/rules.d")
   install(FILES 45-libqwqng.rules
           DESTINATION /etc/udev/rules.d)

endif("${PACKAGE}" STREQUAL "Debian")

# General RPM rules
set(CPACK_RPM_PACKAGE_DEPENDS       "libftdi1" PARENT_SCOPE)

# Bundles
message("-- Installing udev rules to /etc/udev/rules.d")
install(FILES 45-libqwqng.rules
	DESTINATION /etc/udev/rules.d)
