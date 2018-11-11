/*
 *	PearPC
 *	profiling.h
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

#ifndef __TOOLS_PROFILING_H__
#define __TOOLS_PROFILING_H__

#include "system/sysclk.h"
#include <chrono>
#include <atomic>

#ifdef JITC_PROFILING
#define PEARPC_PROFILING_STOPWATCH(v) ::pearpc::profiling::Stopwatch stopwatch(v)
#define PEARPC_PROFILING_INCREMENT(v) ++v.counter
#else
#define PEARPC_PROFILING_STOPWATCH(v) {}
#define PEARPC_PROFILING_INCREMENT(v) {}
#endif

namespace pearpc {
	
	namespace profiling {

		template <typename T>
		struct DataPoint {
			typedef T value_type;
			
			DataPoint() : last(0) {}
			
			/// Atomically store current counter and reset value
			void collect() { last = counter.exchange(0); }
			
			/// Return the counter value when last collected
			const T& getLast() const { return last; }
			
			std::atomic<value_type> counter;
			value_type last;
		};
		
		typedef DataPoint<uint64_t> DurationDataPoint;

		struct Metrics {
			
			Metrics();
			
			/// Time of last collection
			uint64_t lastCollect;
			
			/// Duration of last collection period
			uint64_t lastDuration;
			
			struct JITC {
				/// Time spent in createClientPage
				DurationDataPoint createClientPageTime;
				
				/// Time spent in newEntryPoint
				DurationDataPoint newEntryPointTime;

				DataPoint<uint32_t> tcfMissCount;
				DataPoint<uint32_t> entryPointHitCount;
				DataPoint<uint32_t> entryPointMissCount;
			};
			
			JITC jitc;
			
			/// Time spent servicing IDE requests
			DurationDataPoint ideTime;
			
			/// Time spent idling the CPU
			DurationDataPoint cpuIdleTime;
		};
		
		
		Metrics& getMetrics();
		void collect(Metrics &metrics);
		void report(const Metrics &metrics);
		
		/// Convert a duration data point to percentage of the total time
		double convertToPercent(const DurationDataPoint &dataPoint, uint64_t totalTime);

		/// Convert an accumulation data point to a rate per second
		template <typename data_type>
		double convertToRate(const DataPoint<data_type> &dataPoint, uint64_t totalTime) {
			static const double nsToSec = static_cast<double>(std::nano::den);
			
			if (totalTime != 0) {
				const double totalSec = static_cast<double>(totalTime) / nsToSec;
				const double rate = static_cast<double>(dataPoint.getLast()) / totalSec;
				return rate;
			}
			return 0.0;
		}
		
		
		///	\class Stopwatch
		///	\brief RAII mechanism for accumulating elapsed time to a DurationDataPoint
		class Stopwatch {
		public:
			Stopwatch(DurationDataPoint& dataPoint)
			: m_counter(dataPoint.counter)
			, m_start(sys_get_elapsed_ns())
			{}
			
			~Stopwatch() {
				m_counter += sys_get_elapsed_ns() - m_start;
			}
			
		private:
			std::atomic<uint64_t> &m_counter;
			uint64_t m_start;
			
			// noncopyable
			Stopwatch(const Stopwatch &);
			const Stopwatch &operator=(const Stopwatch &);
		};
		
	}
	
}

#endif
