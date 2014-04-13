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

#include "filesystem_sdl.hpp"

#include "filesystem.hpp"
#include <SDL/SDL_rwops.h>

namespace filesystem {
RWops::RWops(const std::ostringstream &in) :
	buffer_(new std::string(in.str())),
	rwops_(SDL_RWFromConstMem(buffer_->data(), buffer_->size()))
{
	//const std::string tmp = in.str();
	//buffer_ = boost::shared_array<char>(tmp.data(), length_);
	//rwops_ = SDL_RWFromConstMem(buffer_.get(), length_);
}

RWops::RWops(const RWops &in) :
	buffer_(in.buffer_),
	rwops_(in.rwops_)
{
}

RWops& RWops::operator=(const RWops &in) {
	this->~RWops();
	new(this) RWops(in);
	return *this;
}

SDL_RWops* RWops::operator*() {
	return rwops_;
}

RWops::~RWops() {
	if(buffer_.unique())
		SDL_FreeRW(rwops_);
}

RWops load_RWops(const std::string &path) {
	scoped_istream ifs = istream_file(path);
	std::ostringstream oss;
	const int bufsize = 4096;
	char buf[bufsize];
	do {
		ifs->read(buf, bufsize);
		const int count = *ifs ? bufsize : static_cast<int>(ifs->gcount());
		oss.write(buf, count);
	} while(*ifs);
	return RWops(oss);
}
}
