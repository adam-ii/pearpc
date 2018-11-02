/*
 *	PearPC
 *	include/trace.h
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

/*
 * QEMU System Emulator
 *
 * Copyright (c) 2003-2008 Fabrice Bellard
 */

#ifndef __TRACE_H__
#define __TRACE_H__

#include "debug/tracers.h"

#define PRId64 "lld"
#define PRIx64 "llx"

// From {build-dir}/hw/misc/trace.h
inline void trace_mos6522_set_counter(int index, unsigned int val) {
	IO_CUDA_TRACE("mos6522_set_counter " "T%d.counter=%d" "\n", index, val);
}

inline void trace_mos6522_get_next_irq_time(uint16_t latch, int64_t d, int64_t delta) {
	IO_CUDA_TRACE("mos6522_get_next_irq_time " "latch=%d counter=0x%" PRId64 " delta_next=0x%" PRId64 "\n", latch, d, delta);
}

inline void trace_mos6522_set_sr_int(void) {
	IO_CUDA_TRACE("mos6522_set_sr_int " "set sr_int" "\n");
}

inline void trace_mos6522_write(uint64_t addr, uint64_t val) {
	IO_CUDA_TRACE("mos6522_write " "reg=0x%" PRIx64 " val=0x%" PRIx64 "\n", addr, val);
}

inline void trace_mos6522_read(uint64_t addr, unsigned val) {
	IO_CUDA_TRACE("mos6522_read " "reg=0x%" PRIx64 " val=0x%x" "\n", addr, val);
}

// From {build-dir}/hw/misc/macio/trace.h
inline void trace_cuda_delay_set_sr_int(void) {
	IO_CUDA_TRACE("cuda_delay_set_sr_int " "" "\n");
}

inline void trace_cuda_data_send(uint8_t data) {
	IO_CUDA_TRACE("cuda_data_send " "send: 0x%02x" "\n", data);
}

inline void trace_cuda_data_recv(uint8_t data) {
	IO_CUDA_TRACE("cuda_data_recv " "recv: 0x%02x" "\n", data);
}

inline void trace_cuda_receive_packet_cmd(const char * cmd) {
	IO_CUDA_TRACE("cuda_receive_packet_cmd " "handling command %s" "\n", cmd);
}

inline void trace_cuda_packet_receive(int len) {
	IO_CUDA_TRACE("cuda_packet_receive " "length %d" "\n", len);
}

inline void trace_cuda_packet_receive_data(int i, const uint8_t data) {
	IO_CUDA_TRACE("cuda_packet_receive_data " "[%d] 0x%02x" "\n", i, data);
}

inline void trace_cuda_packet_send(int len) {
	IO_CUDA_TRACE("cuda_packet_send " "length %d" "\n", len);
}

inline void trace_cuda_packet_send_data(int i, const uint8_t data) {
	IO_CUDA_TRACE("cuda_packet_send_data " "[%d] 0x%02x" "\n", i, data);
}

// From {build-dir}/hw/input/trace.h
inline void trace_adb_kbd_no_key(void) {
	IO_ADB_TRACE("adb_kbd_no_key " "Ignoring NO_KEY" "\n");
}

inline void trace_adb_kbd_writereg(int reg, uint8_t val) {
	IO_ADB_TRACE("adb_kbd_writereg " "reg %d val 0x%2.2x" "\n", reg, val);
}

inline void trace_adb_kbd_readreg(int reg, uint8_t val0, uint8_t val1) {
	IO_ADB_TRACE("adb_kbd_readreg " "reg %d obuf[0] 0x%2.2x obuf[1] 0x%2.2x" "\n", reg, val0, val1);
}

inline void trace_adb_kbd_request_change_addr(int devaddr) {
	IO_ADB_TRACE("adb_kbd_request_change_addr " "change addr to 0x%x" "\n", devaddr);
}

inline void trace_adb_kbd_request_change_addr_and_handler(int devaddr, int handler) {
	IO_ADB_TRACE("adb_kbd_request_change_addr_and_handler " "change addr and handler to 0x%x, 0x%x" "\n", devaddr, handler);
}

inline void trace_adb_mouse_flush(void) {
	IO_ADB_TRACE("adb_mouse_flush " "flush" "\n");
}

inline void trace_adb_mouse_writereg(int reg, uint8_t val) {
	IO_ADB_TRACE("adb_mouse_writereg " "reg %d val 0x%2.2x" "\n", reg, val);
}

inline void trace_adb_mouse_readreg(int reg, uint8_t val0, uint8_t val1) {
	IO_ADB_TRACE("adb_mouse_readreg " "reg %d obuf[0] 0x%2.2x obuf[1] 0x%2.2x" "\n", reg, val0, val1);
}

inline void trace_adb_mouse_request_change_addr(int devaddr) {
	IO_ADB_TRACE("adb_mouse_request_change_addr " "change addr to 0x%x" "\n", devaddr);
}

inline void trace_adb_mouse_request_change_addr_and_handler(int devaddr, int handler) {
	IO_ADB_TRACE("adb_mouse_request_change_addr_and_handler " "change addr and handler to 0x%x, 0x%x" "\n", devaddr, handler);
}

#endif
