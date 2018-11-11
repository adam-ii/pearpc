/*
 *	PearPC
 *	profiling.cc
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

#include "profiling.h"
#include "tools/snprintf.h"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace pearpc {
	
	namespace profiling {
		
		static Metrics s_metrics;
		
		Metrics::Metrics()
		: lastCollect(sys_get_elapsed_ns())
		, lastDuration(0)
		{}

		Metrics& getMetrics() {
			return s_metrics;
		}
		
		void collect(Metrics &metrics) {
			const auto now = sys_get_elapsed_ns();
			metrics.lastDuration = now - metrics.lastCollect;
			metrics.lastCollect = now;
			
			#if JITC_PROFILING
			metrics.jitc.createClientPageTime.collect();
			metrics.jitc.newEntryPointTime.collect();
			metrics.jitc.tcfMissCount.collect();
			metrics.jitc.entryPointHitCount.collect();
			metrics.jitc.entryPointMissCount.collect();
			metrics.ideTime.collect();
			metrics.cpuIdleTime.collect();
			#endif
		}
		
		void report(const Metrics &metrics) {
			#if JITC_PROFILING
			const double jitcPct = (convertToPercent(metrics.jitc.createClientPageTime, metrics.lastDuration) +
									convertToPercent(metrics.jitc.newEntryPointTime, metrics.lastDuration));
			const double idePct = convertToPercent(metrics.ideTime, metrics.lastDuration);
			const double idlePct = convertToPercent(metrics.cpuIdleTime, metrics.lastDuration);
			
			const double tcfMissRate = convertToRate(metrics.jitc.tcfMissCount, metrics.lastDuration);
			const double entryHitRate = convertToRate(metrics.jitc.entryPointHitCount, metrics.lastDuration);
			const double entryMissRate = convertToRate(metrics.jitc.entryPointMissCount, metrics.lastDuration);
			
			std::ostringstream s;
			
			s << std::fixed << std::setprecision(1);
			
			s << "JITC " << std::setw(4) << jitcPct << "% ";
			s << "IDE "  << std::setw(4) << idePct << "% ";
			s << "Idle " << std::setw(4) << idlePct << "%; ";
			
			s << std::setprecision(0);
			
			s << "TFC " << tcfMissRate << " miss/s, ";
			s << "EntryPt " << entryMissRate << " miss/s " << entryHitRate << " hit/s";
			
			ht_printf("%s\n", s.str().c_str());
			#endif
		}
		
		double convertToPercent(const DurationDataPoint &dataPoint, uint64_t totalTime) {
			
			if (totalTime != 0) {
				return static_cast<double>(dataPoint.getLast()) / static_cast<double>(totalTime) * 100.0;
			}
			return 0.0;
		}
		
	}
	
}
