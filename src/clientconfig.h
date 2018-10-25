/*
 *	PearPC
 *	clientconfig.h
 *
 *	Copyright (C) 2018 Adam Iarossi
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __CLIENTCONFIG_H__
#define __CLIENTCONFIG_H__

#include "configparser.h"
#include "system/keyboard.h"
#include "system/display.h"

namespace pearpc {

	class ClientConfig
	{
	public:
		ClientConfig();
		~ClientConfig();

		void Load(const char* filename);

		int32_t getMemorySize() const { return m_memory_size; }
		int32_t getRedrawInterval() const { return m_redraw_interval; }
		bool getFullScreen() const { return m_fullscreen; }
		
		const String& getPromFile() const { return m_prom_file; }

		const KeyboardCharacteristics& getKeyConfig() const { return m_keyConfig; }
		const DisplayCharacteristics& getDisplayConfig() const { return m_displayConfig; }

	private:
		ConfigParser m_config;
		
		int32_t m_memory_size;
		int32_t m_redraw_interval;
		bool m_fullscreen;
		String m_prom_file;
		
		KeyboardCharacteristics m_keyConfig;
		DisplayCharacteristics m_displayConfig;
	};

}

#endif
