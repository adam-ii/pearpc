/*
 *	PearPC
 *	cuda.cc
 *
 *	Copyright (C) 2003-2004 Sebastian Biallas (sb@biallas.net)
 *	Copyright (C) 2004 Stefan Weyergraf
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
 *	From Linux 2.6.4:
 *	The VIA (versatile interface adapter) interfaces to the CUDA,
 *	a 6805 microprocessor core which controls the ADB (Apple Desktop
 *	Bus) which connects to the keyboard and mouse.  The CUDA also
 *	controls system power and the RTC (real time clock) chip.
 *
 *	See also:
 *	http://www.howell1964.freeserve.co.uk/parts/6522_VIA.htm
 *
 *	References:
 *	[1] http://bbc.nvg.org/doc/datasheets/R6522_r9.zip
 */

#include "cpu/cpu.h"
#include "cuda.h"

#include "qemu/osdep.h"
#include "hw/input/adb.h"
#include "hw/misc/mos6522.h"
#include "hw/misc/macio/cuda.h"

namespace pearpc {

	static ::CUDAState* s_cuda;
	static ::ADBDevice *s_adbMouse;
	static ::ADBDevice *s_adbKeyboard;
	
	static ::IRQState cudaIRQ = {
		.n = IO_PIC_IRQ_CUDA
	};
	
	void cuda_write(uint32 addr, uint32 data, int size)
	{
		s_cuda->memoryWrite(addr - IO_CUDA_PA_START, data, size);
	}
	
	void cuda_read(uint32 addr, uint32 &data, int size)
	{
		uint64_t mem = s_cuda->memoryRead(addr - IO_CUDA_PA_START, size);
		data = static_cast<uint32>(mem);
	}
	
	bool cuda_prom_get_key(uint32 &key)
	{
		// TODO: need alternative method for getting keyboard input to prom
		return false;
	}
	
	void cuda_pre_init()
	{
	}
	
	void cuda_init()
	{
		auto obj = qemu::createObject(TYPE_CUDA);
		s_cuda = CUDA(obj);
		s_cuda->callRealize();
		
		auto adb_bus = &s_cuda->adb_bus;
		s_adbMouse = ADB_DEVICE(qemu::createDevice(adb_bus, TYPE_ADB_MOUSE));
		s_adbKeyboard = ADB_DEVICE(qemu::createDevice(adb_bus, TYPE_ADB_KEYBOARD));
		
		for (const auto& device : s_cuda->adb_bus.getChildren()) {
			device->callReset();
		}
		
		// TODO: qbus resets child objects on main system bus
		s_cuda->callReset();
		s_cuda->mos6522_cuda.callReset();
		
		s_cuda->tb_frequency = ppc_get_timebase_frequency(0);
		s_cuda->mos6522_cuda.irq = &cudaIRQ;
	}
	
	void cuda_done()
	{
	}
	
	void cuda_init_config()
	{
	}
	
}
