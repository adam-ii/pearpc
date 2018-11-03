/*
 *	PearPC
 *	timer.h
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
 *
 */

#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

#include "system/sysclk.h"
#include <memory>
#include <functional>
#include <chrono>

namespace pearpc {

	typedef std::shared_ptr<class TimerImpl> Timer;
	
	typedef std::function<void ()> TimerCallback;

	/// Initialize the timer system
	void initTimer();
	
	/// Shutdown the timer system
	void doneTimer();
	
	/// Create a new timer
	Timer createTimer(const TimerCallback &cb);

	/// Delete a timer
	void deleteTimer(const Timer &t);

	/// Cancel a timer, stopping it from firing if it has not already
	void cancelTimer(const Timer &t);

	/// Change the firing time of an existing timer
	void modifyTimer(const Timer &t, int64_t expireTime);
	
	/// Change the firing time of an existing timer to a duration from the current time
	template <typename duration_type>
	void modifyTimer(const Timer &t, const std::chrono::duration<duration_type> &expireTime) {
		modifyTimer(t, sys_get_elapsed_ns() + std::chrono::duration_cast<std::chrono::nanoseconds>(expireTime).count());
	}
	
}

#endif
