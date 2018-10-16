/*
 *	PearPC
 *	rtl8139.h
 *
 *	Copyright (C) 2003 Stefan Weyergraf
 *	Copyright (C) 2004 Eric Estabrooks (estabroo@battlefoundry.net)
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

#ifndef __IO_RTL8139_H__
#define __IO_RTL8139_H__

namespace pearpc {

extern bool rtl8139_installed;

void rtl8139_init();
void rtl8139_done();
void rtl8139_init_config();

}

#endif
