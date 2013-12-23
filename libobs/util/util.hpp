/******************************************************************************
    Copyright (C) 2013 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/* Useful C++ classes/bindings for util data and pointers */

#pragma once

#include <string.h>
#include <stdarg.h>

#include "config-file.h"
#include "text-lookup.h"

/* RAII wrappers */

template<typename T> class BPtr {
	T *ptr;

	BPtr(BPtr const&) = delete;

	BPtr &operator=(BPtr const&) = delete;

public:
	inline BPtr(T *p=nullptr) : ptr(p)         {}
	inline BPtr(BPtr &&other) : ptr(other.ptr) {other.ptr = nullptr;}
	inline ~BPtr()                             {bfree(ptr);}

	inline T *operator=(T *p)   {bfree(ptr); ptr = p; return p;}
	inline operator T*()        {return ptr;}
	inline T **operator&()      {bfree(ptr); ptr = nullptr; return &ptr;}

	inline bool operator!()     {return ptr == NULL;}
	inline bool operator==(T p) {return ptr == p;}
	inline bool operator!=(T p) {return ptr != p;}
};

class ConfigFile {
	config_t config;

	ConfigFile(ConfigFile const&) = delete;
	ConfigFile &operator=(ConfigFile const&) = delete;

public:
	inline ConfigFile() : config(NULL) {}
	inline ConfigFile(ConfigFile &&other) : config(other.config)
	{
		other.config = nullptr;
	}
	inline ~ConfigFile()
	{
		config_close(config);
	}

	inline bool Create(const char *file)
	{
		Close();
		config = config_create(file);
		return config != NULL;
	}

	inline int Open(const char *file, config_open_type openType)
	{
		Close();
		return config_open(&config, file, openType);
	}

	inline int Save()
	{
		return config_save(config);
	}

	inline void Close()
	{
		config_close(config);
		config = NULL;
	}

	inline operator config_t() const {return config;}
};

class TextLookup {
	lookup_t lookup;

	TextLookup(TextLookup const&) = delete;

	TextLookup &operator=(TextLookup const&) = delete;

public:
	inline TextLookup(lookup_t lookup=nullptr) : lookup(lookup) {}
	inline TextLookup(TextLookup &&other) : lookup(other.lookup)
	{
		other.lookup = nullptr;
	}
	inline ~TextLookup() {text_lookup_destroy(lookup);}

	inline TextLookup& operator=(lookup_t val)
	{
		text_lookup_destroy(lookup);
		lookup = val;
		return *this;
	}

	inline operator lookup_t() const {return lookup;}

	inline const char *GetString(const char *lookupVal) const
	{
		const char *out;
		if (!text_lookup_getstr(lookup, lookupVal, &out))
			return lookupVal;

		return out;
	}
};
