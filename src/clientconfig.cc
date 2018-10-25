/*
 *	PearPC
 *	clientconfig.cc
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
 
#include "clientconfig.h"
#include "tools/except.h"
#include "io/graphic/gcard.h"

namespace pearpc {

	KeyboardCharacteristics LoadKeyConfig(ConfigParser& config)
	{
		String key_compose_dialog_string;
		String key_toggle_mouse_grab_string;
		String key_toggle_full_screen_string;
		KeyboardCharacteristics keyConfig;
		
		config.getConfigString("key_compose_dialog", key_compose_dialog_string);
		config.getConfigString("key_toggle_mouse_grab", key_toggle_mouse_grab_string);
		config.getConfigString("key_toggle_full_screen", key_toggle_full_screen_string);
		
		if (!SystemKeyboard::convertStringToKeycode(keyConfig.key_compose_dialog, key_compose_dialog_string)) {
			throw MsgfException("Invalid '%s'", "key_compose_dialog");
		}
		if (!SystemKeyboard::convertStringToKeycode(keyConfig.key_toggle_mouse_grab, key_toggle_mouse_grab_string)) {
			throw MsgfException("Invalid '%s'", "key_toggle_mouse_grab");
		}
		if (!SystemKeyboard::convertStringToKeycode(keyConfig.key_toggle_full_screen, key_toggle_full_screen_string)) {
			throw MsgfException("Invalid '%s'", "key_toggle_full_screen");
		}
		
		return keyConfig;
	}

	DisplayCharacteristics LoadDisplayConfig(ConfigParser& config)
	{
		String chr;
		DisplayCharacteristics gm;
	
		config.getConfigString("ppc_start_resolution", chr);
		if (!displayCharacteristicsFromString(gm, chr)) {
			throw MsgfException("Invalid 'ppc_start_resolution'");
		}
	
		switch (gm.bytesPerPixel) {
				/*
				 *	Are we confusing bytesPerPixel with bitsPerPixel?
				 *	Yes! And I am proud of it!
				 */
			case 15:
				gm.bytesPerPixel = 2;
				break;
			case 32:
				gm.bytesPerPixel = 4;
				break;
			default:
				throw MsgException("Invalid depth in 'ppc_start_resolution'");
		}
		
		if (!gcard_finish_characteristic(gm)) {
			throw MsgException("Invalid 'ppc_start_resolution'");
		}
		
		return gm;
	}


	ClientConfig::ClientConfig()
	: m_memory_size(0)
	, m_redraw_interval(0)
	, m_fullscreen(false)
	{
		m_config.acceptConfigEntryStringDef("ppc_start_resolution", "800x600x15");
		m_config.acceptConfigEntryIntDef("ppc_start_full_screen", 0);
		m_config.acceptConfigEntryIntDef("memory_size", 128*1024*1024);
		m_config.acceptConfigEntryIntDef("page_table_pa", 0x00300000);
		m_config.acceptConfigEntryIntDef("redraw_interval_msec", 20);
		m_config.acceptConfigEntryStringDef("key_compose_dialog", "F11");
		m_config.acceptConfigEntryStringDef("key_change_cd_0", "none");
		m_config.acceptConfigEntryStringDef("key_change_cd_1", "none");
		m_config.acceptConfigEntryStringDef("key_toggle_mouse_grab", "F12");
		m_config.acceptConfigEntryStringDef("key_toggle_full_screen", "Ctrl+Alt+Return");

		// TODO: pass configuration as parameter to *_init_config() and *_init()
		gConfig = &m_config;
	}
	
	ClientConfig::~ClientConfig()
	{
		if (gConfig == &m_config) {
			gConfig = NULL;
		}
	}
	
	void ClientConfig::Load(const char* filename)
	{
		LocalFile file(filename);
		m_config.loadConfig(file);

		m_memory_size = m_config.getConfigInt("memory_size");
		if (m_memory_size < 64*1024*1024) {
			throw MsgException("'memory_size' must be >= 64MB.");
		}
		
		m_redraw_interval = m_config.getConfigInt("redraw_interval_msec");
		if (m_redraw_interval < 10 || m_redraw_interval > 500) {
			throw MsgException("'redraw_interval_msec' must be between 10 and 500 (inclusive).");
		}
		
		if (m_config.haveKey("prom_loadfile"))
		{
			m_config.getConfigString("prom_loadfile", m_prom_file);
		}

		m_keyConfig = LoadKeyConfig(m_config);
		m_displayConfig = LoadDisplayConfig(m_config);
	}
	
}
