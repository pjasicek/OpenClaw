/*
 * Copyright 2003 - 2016, The libsigc++ Development Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef SIGCXX_SIGCXX_H
#define SIGCXX_SIGCXX_H

/** @mainpage libsigc++ Reference Manual
 *
 * @section description Description
 *
 * libsigc++ provides a typesafe (at compile time) callback system for standard
 * C++. It allows you to define signals and to connect those signals to any
 * callback function, either a global or a member function, regardless of whether
 * it is static or virtual. It also contains adaptor classes for connection of
 * dissimilar callbacks.
 *
 * For instance, see the @ref signal "Signals", @ref sigcfunctors "Functors",
 * @ref slot "Slots" and @ref adaptors "Adaptors".
 *
 * See also the
 * <a href="http://libsigc.sourceforge.net/libsigc2/docs/manual/html/index.html">libsigc++
 * tutorial</a>,
 * the <a href="http://libsigc.sourceforge.net/">libsigc++ website</a>, and
 * the <a href="http://library.gnome.org/devel/gtkmm-tutorial/unstable/chapter-signals.html">Signals
 * appendix of the Programming with gtkmm book</a>.
 *
 * @section features Features
 *
 * - Compile-time typesafe callbacks (also faster than run time checks)
 * - Type-safety violations report the line number correctly with template names
 *   (no tracing template failures into headers)
 * - No compiler extensions or meta compilers required
 * - Proper handling of dynamic objects and signals (deleted objects will not
 *   cause crashes)
 * - Extendable API at any level: signal, slot, connection and trackable
 * - Extensions do not require alteration of basic components
 * - User-definable accumulators
 * - A variety of adaptors to change the callback signature: bind, hide,
 *   retype, and compose
 *
 * @section basics Basic Usage
 *
 * Include the libsigc++ header:
 * @code
 * #include <sigc++/sigc++.h>
 * @endcode
 * (You may include individual headers, such as @c sigc++/bind.h instead.)
 *
 * If your source file is @c program.cc, you can compile it with:
 * @code
 * g++ program.cc -o program `pkg-config --cflags --libs sigc++-3.0`
 * @endcode
 *
 * @subsection autotools Using Autotools
 *
 * Alternatively, if using autoconf, use the following in @c configure.ac:
 * @code
 * PKG_CHECK_MODULES([DEPS], [sigc++-3.0])
 * @endcode
 * Then use the generated @c DEPS_CFLAGS and @c DEPS_LIBS variables
 * in the project @c Makefile.am files. For example:
 * @code
 * yourprogram_CPPFLAGS = $(DEPS_CFLAGS)
 * yourprogram_LDADD = $(DEPS_LIBS)
 * @endcode
 *
 * Your @c PKG_CHECK_MODULES() call should also mention any other libraries that
 * you need to use via pkg-config.
 *
 * @subsection cmake Using CMake
 *
 * If using CMake, use the following in @c CMakeList.txt:
 * @code
 * include(FindPkgConfig)
 * pkg_check_modules(DEPS REQUIRED sigc++-3.0)
 * include_directories(${DEPS_INCLUDE_DIRS})
 * target_link_libraries(yourprogram ${DEPS_LIBRARIES})
 * @endcode
 *
 * Your @c pkg_check_modules() call should also mention any other libraries that
 * you need to use via pkg-config.
 */

#include <sigc++/signal.h>
#include <sigc++/connection.h>
#include <sigc++/trackable.h>
#include <sigc++/adaptors/adaptors.h>
#include <sigc++/functors/functors.h>

#endif /* SIGCXX_SIGCXX_H */
