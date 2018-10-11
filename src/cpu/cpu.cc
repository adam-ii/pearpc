/*
 *	PearPC
 *	cpu.cc
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

#include "cpu.h"
#include "io/io.h"
#include "tools/except.h"
#include <vector>
#include <atomic>
#include <thread>

namespace pearpc {
	
	namespace {
		
		struct ThreadState
		{
			std::atomic_bool m_cpu_running;
			std::thread m_thread;
		};
		
		typedef std::vector<std::shared_ptr<ThreadState>> ThreadStateList;
		ThreadStateList *s_cpuState = NULL;
		
		void cpu_thread(std::shared_ptr<ThreadState> state, std::function<void ()> starting, std::function<void ()> exited)
		{
			if (starting != nullptr) {
				starting();
			}
			
			ppc_cpu_map_framebuffer(IO_GCARD_FRAMEBUFFER_PA_START, IO_GCARD_FRAMEBUFFER_EA);
			ppc_cpu_run();
			
			state->m_cpu_running = false;

			if (exited != nullptr) {
				exited();
			}
			
			ht_printf("cpu thread exited\n");
		}
	}

	
	void cpu_thread_start(int index, const std::function<void ()>& starting, const std::function<void ()>& exited)
	{
		const int kMaxCPU = 1;
		
		if (s_cpuState == NULL) {
			s_cpuState = new ThreadStateList(kMaxCPU, nullptr);
		}
		
		if (s_cpuState->at(index)) {
			throw MsgfException("CPU %d has already been started", index);
		}
		
		auto state = std::make_shared<ThreadState>();
		s_cpuState->at(index) = state;
		state->m_cpu_running = true;
		state->m_thread = std::thread(cpu_thread, state, starting, exited);
	}

	void cpu_thread_wait(int index) {
		if (s_cpuState) {
			auto state = s_cpuState->at(index);
			
			if (state->m_thread.joinable()) {
				state->m_thread.join();
			}
		}
	}

	bool cpu_thread_is_running(int index) {
		if (s_cpuState) {
			if (auto state = s_cpuState->at(index)) {
				return state->m_cpu_running;
			}
		}
		
		return false;
	}
	
}

