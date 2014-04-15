/*
   Copyright (C) 2014 by Alexander van Gessel <ai0867@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file
 * SDL 1.2 helper functions.
 */

#ifndef FILESYSTEM_SDL_HPP_INCLUDED
#define FILESYSTEM_SDL_HPP_INCLUDED

#include <boost/smart_ptr/shared_ptr.hpp>
#include <sstream>
#include <string>

struct SDL_RWops;

namespace filesystem {
	class RWops {
	public:
		explicit RWops();
		RWops(const RWops&);
		RWops& operator=(const RWops&);
		// RWops remains owner of the internal structure
		// Callers should not use any auto-free functionality
		SDL_RWops* operator*();
		~RWops();
	private:
		RWops(const std::ostringstream &);

		boost::shared_ptr<std::string> buffer_;
		SDL_RWops *rwops_;

	public:
		static RWops open_stream(const std::string &path);
		static RWops open_mem(const std::string &path);
	};
}

#endif
