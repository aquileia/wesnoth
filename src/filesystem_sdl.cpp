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
RWops::RWops() :
	buffer_(static_cast<std::string*>(NULL)),
	rwops_(SDL_RWFromConstMem(NULL, 0))
{
}

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
		SDL_RWclose(rwops_);
}

RWops RWops::open_mem(const std::string &path) {
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

static int SDLCALL ifs_seek(struct SDL_RWops *context, int offset, int whence);
static int SDLCALL ifs_read(struct SDL_RWops *context, void *ptr, int size, int maxnum);
static int SDLCALL ifs_write(struct SDL_RWops *context, const void *ptr, int size, int num);
static int SDLCALL ifs_close(struct SDL_RWops *context);

RWops RWops::open_stream(const std::string &path) {
	SDL_RWops *rw = SDL_AllocRW();
	RWops wrap = RWops();
	wrap.rwops_ = rw;
	rw->seek = &ifs_seek;
	rw->read = &ifs_read;
	rw->write = &ifs_write;
	rw->close = &ifs_close;

	rw->hidden.unknown.data1 = istream_file(path);

	return wrap;
}

static int SDLCALL ifs_seek(struct SDL_RWops *context, int offset, int whence) {
	std::ios_base::seekdir seekdir;
	switch(whence){
	case RW_SEEK_SET:
		seekdir = std::ios_base::beg;
		break;
	case RW_SEEK_CUR:
		seekdir = std::ios_base::cur;
		break;
	case RW_SEEK_END:
		seekdir = std::ios_base::end;
		break;
	default:
		assert(false);
	}
	return static_cast<std::istream*>(context->hidden.unknown.data1)->seekg(offset, seekdir).tellg();
}
static int SDLCALL ifs_read(struct SDL_RWops *context, void *ptr, int size, int maxnum) {
	std::istream *ifs = static_cast<std::istream*>(context->hidden.unknown.data1);
	std::streamsize pos = ifs->tellg();
	ifs->seekg(maxnum * size, std::ios_base::cur);
	std::streamsize num = ((ifs->tellg() - pos)/size);

	ifs->seekg(pos, std::ios_base::beg);
	ifs->read(static_cast<char*>(ptr), num * size);
	return num;
}
static int SDLCALL ifs_write(struct SDL_RWops * /*context*/, const void * /*ptr*/, int /*size*/, int /*num*/) {
	SDL_SetError("Writing not implemented");
	return 0;
}
static int SDLCALL ifs_close(struct SDL_RWops *context) {
	if (context) {
		std::istream *ifs = static_cast<std::istream*>(context->hidden.unknown.data1);
		delete ifs;
		SDL_FreeRW(context);
	}
	return 0;
}
}
