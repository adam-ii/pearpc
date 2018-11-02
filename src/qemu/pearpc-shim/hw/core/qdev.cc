/*
 *	PearPC
 *	hw/core/qdev.cc
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
 *	Dynamic device configuration and creation.
 *
 *	Copyright (c) 2009 CodeSourcery
 *
 */

#include "qemu/osdep.h"
#include "hw/qdev.h"

// PearPC
#include "tools/except.h"

DeviceClass::DeviceClass(const TypeInfo *type)
: ObjectClass(type)
{}


DeviceState::DeviceState(ObjectClass *cls)
: Object(cls)
, m_deviceClass(dynamic_cast<DeviceClass*>(cls))
{}

void DeviceState::callReset() {
	if (m_deviceClass->reset) {
		m_deviceClass->reset(this);
	}
}

void DeviceState::callRealize() {
	if (!m_realized) {
		m_realized = true;
		
		if (m_deviceClass->realize) {
			Error *err = NULL;
			m_deviceClass->realize(this, &err);
		}
	}
}


BusState::BusState(ObjectClass *cls)
: Object(cls)
{}


void device_class_set_parent_realize(DeviceClass *dc, DeviceRealize dev_realize, DeviceRealize *parent_realize) {
	*parent_realize = dc->realize;
	dc->realize = dev_realize;
}

BusState *qdev_get_parent_bus(DeviceState *dev) {
	return dev->getParentBus();
}

void qdev_reset_all(DeviceState *dev) {
	// TODO: reset bus children
	dev->callReset();
}

void qbus_create_inplace(Object *bus, size_t size, const char *_typename,
						 DeviceState *parent, const char *name) {
	// TODO: qbus_realize
	if (auto busState = BUS(bus)) {
		qemu::initObject(busState, qemu::findTypeInfo(_typename));
		busState->setContainer(parent);
	}
}


namespace qemu {
	
	DeviceState *createDevice(BusState *bus, const char *name) {
		if (auto device = dynamic_cast<DeviceState *>(createObject(name))) {
			device->setParentBus(bus);
			bus->addChild(device);
			device->callRealize();
			return device;
		}
		
		throw pearpc::MsgfException("Unknown device '%s' for bus '%s'", name, bus->getTypeName());
	}
	
}
