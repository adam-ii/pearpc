/*
 *	PearPC
 *	include/hw/qdev-core.h
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

#ifndef QDEV_CORE_H
#define QDEV_CORE_H

#include "qom/object.h"
#include "hw/irq.h"
#include <vector>

#define TYPE_DEVICE "device"
#define DEVICE(obj) OBJECT_CHECK(DeviceState, (obj), "")
#define DEVICE_CLASS(klass) OBJECT_CLASS_CHECK(DeviceClass, (klass), "")

struct DeviceState;
struct BusState;
struct Property;

typedef enum DeviceCategory {
	DEVICE_CATEGORY_BRIDGE,
	DEVICE_CATEGORY_USB,
	DEVICE_CATEGORY_STORAGE,
	DEVICE_CATEGORY_NETWORK,
	DEVICE_CATEGORY_INPUT,
	DEVICE_CATEGORY_DISPLAY,
	DEVICE_CATEGORY_SOUND,
	DEVICE_CATEGORY_MISC,
	DEVICE_CATEGORY_CPU,
	DEVICE_CATEGORY_MAX
} DeviceCategory;

typedef void (*DeviceRealize)(DeviceState *dev, Error **errp);
typedef void (*DeviceReset)(DeviceState *dev);


///	\class DeviceClass
///	\brief C++ implementation of QEMU DeviceClass
struct DeviceClass : public ObjectClass
{
	DeviceClass(const TypeInfo *type);

	// struct members used by QEMU
	unsigned long categories[1];
	Property *props;
	DeviceRealize realize;
	DeviceReset reset;
	const char *bus_type;
	const struct VMStateDescription *vmsd;
};


///	\class DeviceState
///	\brief C++ placeholder of QEMU DeviceState
struct DeviceState : public Object
{
	DeviceState(ObjectClass *cls);

	/// Return the ObjectClass as a DeviceClass
	DeviceClass *getDeviceClass() const { return m_deviceClass; }
	
	BusState *getParentBus() const { return m_parentBus; }
	void setParentBus(BusState *p) { m_parentBus = p; }

	/// Call the DeviceClass reset function
	void callReset();
	
	/// Call the DeviceClass realize function
	void callRealize();

	uint64_t memoryRead(hwaddr addr, unsigned size) {
		return m_memRead(this, addr, size);
	}
	
	void memoryWrite(hwaddr addr, uint64_t data, unsigned size) {
		m_memWrite(this, addr, data, size);
	}
	
private:
	DeviceClass *m_deviceClass;
	BusState *m_parentBus;
	bool m_realized;
	
public:
	uint64_t (*m_memRead)(void *opaque, hwaddr addr, unsigned size);
	void (*m_memWrite)(void *opaque, hwaddr addr, uint64_t data, unsigned size);
};


#define TYPE_BUS "bus"
#define BUS(obj) OBJECT_CHECK(BusState, (obj), TYPE_BUS)

///	\class BusState
///	\brief C++ implementation of QEMU BusState
struct BusState : public Object
{
	typedef std::vector<DeviceState *> Children;
	
	BusState(ObjectClass *cls);
	
	void addChild(DeviceState *child) { m_children.push_back(child); }
	const Children &getChildren() const { return m_children; }
	
private:
	Children m_children;
};


///	\class Property
///	\brief Placeholder for QEMU Property
struct Property {};


// From include/hw/qdev-core.h
void device_class_set_parent_realize(DeviceClass *dc, DeviceRealize dev_realize, DeviceRealize *parent_realize);
BusState *qdev_get_parent_bus(DeviceState *dev);
void qdev_reset_all(DeviceState *dev);
void qbus_create_inplace(Object *bus, size_t size, const char *_typename,
						 DeviceState *parent, const char *name);


// From include/qemu/bitops.h
inline void set_bit(long nr, unsigned long *addr) {}


// C++ extensions
namespace qemu {
	
	/// Create and initialize a device on the specified bus
	DeviceState *createDevice(BusState *bus, const char *name);
	
}

#endif
