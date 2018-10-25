/*
 *	PearPC
 *	main.h
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

#ifndef __MAIN_H__
#define __MAIN_H__

#include "clientconfig.h"
#include <memory>

namespace pearpc {
	namespace main {
		
		struct ClientCPUState;
		
		/// Load named configuration file
		std::shared_ptr<ClientConfig> loadConfig(const char *filename);
		
		/// Initialise host UI and client based on client configuration
		void initClient(const std::shared_ptr<ClientConfig>& clientConfig);
		
		/// Start client CPU
		std::shared_ptr<ClientCPUState> startCPU(const std::shared_ptr<ClientConfig>& clientConfig);
		
		/// Returns true if client CPU is running
		bool isCPURunning(const std::shared_ptr<ClientCPUState>& state);
		
		/// Wait for client CPU to terminate
		void waitForCPU(const std::shared_ptr<ClientCPUState>& state);
		
	}
}

#endif
