#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([vulkan], [0.0.1], [dg.briseno@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_LIB([glfw], [glfwInit], [], [AC_MSG_ERROR([Missing library 'glfw'])])
AC_CHECK_LIB([vulkan], [vkEnumerateInstanceExtensionProperties], [], [AC_MSG_ERROR([Missing vulkan SDK])])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT32_T

# Checks for library functions.

AC_OUTPUT
