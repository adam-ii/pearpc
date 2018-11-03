/*
 *	PearPC
 *	timer.cc
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

#include "system/timer.h"
#include "system/sysclk.h"
#include "tools/snprintf.h"
#include "debug/tracers.h"
#include <set>
#include <list>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <algorithm>

namespace pearpc {

	class TimerImpl
	{
	public:
		
		TimerImpl(const TimerCallback &cb)
		: m_expires(-1)
		, m_callback(cb)
		{}
		
		bool isExpired(int64_t now) const { return m_expires <= now; }

		int64_t getExpiry() const { return m_expires; }
		void setExpiry(int64_t t) { m_expires = t; }
		
		const TimerCallback &getCallback() const { return m_callback; }
		
	private:
		int64_t m_expires;
		TimerCallback m_callback;
	};

	namespace {
		
		struct CompareExpires
		{
			bool operator()(const Timer &lhs, const Timer &rhs) const {
				return lhs->getExpiry()  < rhs->getExpiry();
			}
		};
		
		class ActiveTimerSet
		{
		public:
			void erase(const Timer &t) {
				std::lock_guard<std::recursive_mutex> lock(m_mutex);
				
				eraseLocked(t);
			}
			
			void modify(const Timer &t, int64_t expires) {
				SYS_TIMER_TRACE("timer %p new expiration in %lld ns\n", t.get(), expires - (int64_t)sys_get_elapsed_ns());
				
				{
					std::lock_guard<std::recursive_mutex> lock(m_mutex);
					
					eraseLocked(t);
					t->setExpiry(expires);
					m_set.insert(t);
				}
				
				signalWaitingThread();
			}
			
			void update() {
				std::list<Timer> expired;

				{
					// Remove expired timers from the active set
					std::lock_guard<std::recursive_mutex> lock(m_mutex);
					
					const auto now = sys_get_elapsed_ns();

					auto itr = m_set.begin();
					while (itr != m_set.end()) {
						auto timer = *itr;
						
						if (timer->isExpired(now)) {
							SYS_TIMER_TRACE("updating timer %p, expired %lld ns ago\n", timer.get(), now - timer->getExpiry());

							timer->setExpiry(-1);
							expired.push_back(timer);
							itr = m_set.erase(itr);
						} else {
							break;
						}
					}
				}

				// Now process the timers, which may add them back to the list
				for (const auto& timer : expired) {
					timer->getCallback()();
				}
			}
			
			void waitForTimer() {
				const int64_t now = sys_get_elapsed_ns();
				std::chrono::nanoseconds waitTime(std::chrono::nanoseconds::max().count());

				std::unique_lock<std::mutex> lock(m_waitMutex);

				{
					// Check expiration of next timer
					std::lock_guard<std::recursive_mutex> lock(m_mutex);
					if (!m_set.empty()) {
						const auto next = (*m_set.begin())->getExpiry();
						
						if (next > now) {
							waitTime = std::chrono::nanoseconds(next - now);
						} else {
							waitTime = std::chrono::nanoseconds(0);
						}
						SYS_TIMER_TRACE("timer thread snoozing %lld ns\n", waitTime.count());
					} else {
						SYS_TIMER_TRACE("timer thread snoozing forever (no active timers)\n");
					}
				}

				m_waitSignal.wait_for(lock, waitTime);
				
				SYS_TIMER_TRACE("timer thread woken up\n");
			}
			
			void signalWaitingThread() {
				std::lock_guard<std::mutex> lock(m_waitMutex);
				m_waitSignal.notify_all();
			}
			
		private:
			void eraseLocked(const Timer &t) {
				auto itr = std::find(m_set.begin(), m_set.end(), t);
				if (itr != m_set.end()) {
					m_set.erase(itr);
				}
			}

		private:
			std::set<Timer, CompareExpires> m_set;
			
			std::recursive_mutex m_mutex;
			
			std::mutex m_waitMutex;
			std::condition_variable m_waitSignal;
		};
		
		static std::list<Timer> s_timers;
		static ActiveTimerSet s_active;

		static std::thread s_timerThread;
		static std::atomic<bool> s_timerThreadExit;

		ActiveTimerSet& getActiveSet() { return s_active; }
		
		void timerThread() {
			while (!s_timerThreadExit) {
				getActiveSet().waitForTimer();
				getActiveSet().update();
			}
			
			SYS_TIMER_TRACE("timer thread exited\n");
		}
	}

	void initTimer() {
		s_timerThreadExit = false;
		s_timerThread = std::thread(timerThread);
	}
	
	void doneTimer() {
		s_timerThreadExit = true;
		getActiveSet().signalWaitingThread();
		s_timerThread.join();
	}

	Timer createTimer(const TimerCallback &cb) {
		auto t = std::make_shared<TimerImpl>(cb);
		s_timers.push_back(t);
		return t;
	}

	void deleteTimer(const Timer &t) {
		getActiveSet().erase(t);
	}
	
	void cancelTimer(const Timer &t) {
		getActiveSet().erase(t);
	}

	void modifyTimer(const Timer &t, int64_t expireTime) {
		getActiveSet().modify(t, expireTime);
	}
	
	void processTimers() {
		getActiveSet().update();
	}

}
