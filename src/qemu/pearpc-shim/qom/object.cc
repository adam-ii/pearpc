/*
 *	PearPC
 *	qom/object.cc
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
 * QEMU Object Model
 *
 * Copyright IBM, Corp. 2011
 *
 * Authors:
 *  Anthony Liguori   <aliguori@us.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#include "qemu/osdep.h"
#include "qom/object.h"
#include "hw/qdev.h"

// PearPC
#include "debug/tracers.h"

#include <map>
#include <list>
#include <string>
#include <functional>
#include <memory>

namespace {

	using std::shared_ptr;
	using qemu::ObjectFactory;
	
	struct TypeEntry
	{
		TypeEntry()
		: m_type(nullptr)
		, m_class(nullptr)
		, m_factory(nullptr)
		{}
		
		TypeEntry(const TypeInfo *type, DeviceClass *cls, const ObjectFactory &factory = nullptr)
		: m_type(type)
		, m_class(cls)
		, m_factory(factory)
		{}

		const char* getName() const { return m_type->name; }
		
		void callClassInit(DeviceClass *cls) {
			if (m_type->class_init) {
				m_type->class_init(cls, m_type->class_data);
			}
		}

		void callParentClassInit(const shared_ptr<TypeEntry> &parent, DeviceClass *cls) {
			if (parent->m_type->class_base_init) {
				parent->m_type->class_base_init(cls, m_type->class_data);
			}
		}

	public:
		const TypeInfo *m_type;
		DeviceClass *m_class;
		ObjectFactory m_factory;
	};
		
	typedef std::map<size_t, shared_ptr<TypeEntry>> TypesByName;
	static TypesByName *s_types = NULL;
	
	typedef std::list<shared_ptr<TypeInfo>> BuiltinTypeInfos;
	static BuiltinTypeInfos *s_builtins = NULL;

	static bool s_initialized = false;
	
	size_t typeInfoKey(const char *name) {
		return std::hash<std::string>()(name);
	}

	const shared_ptr<TypeEntry> &getTypeEntry(const char* name) {
		return s_types->at(typeInfoKey(name));
	}
	
	shared_ptr<TypeEntry> getParentType(const shared_ptr<TypeEntry> &t) {
		return t->m_type->parent ? getTypeEntry(t->m_type->parent) : nullptr;
	}

	void initDeviceClass(const shared_ptr<TypeEntry> &t) {
		SYS_QEMU_TRACE("type_init: %s\n", t->getName());
		
		// from qemu type_initialize()
		DeviceClass *dc = t->m_class;
		
		if (auto parent = getParentType(t)) {
			parent->callClassInit(dc);
			
			while (parent) {
				t->callParentClassInit(parent, dc);
				parent = getParentType(parent);
			}
		}
		
		t->callClassInit(dc);
	}
	
	void initClasses() {
		if (!s_initialized) {
			for (const auto& t : *s_types) {
				initDeviceClass(t.second);
			}
			s_initialized = true;
		}
	}

	void populateBuiltinTypes(TypesByName& map) {
		// Register types that are implemented by the shim layer
		if (!s_builtins) {
			s_builtins = new BuiltinTypeInfos();
		
			for (auto i : { "device", "bus", "sys-bus-device" }) {
				auto type = std::make_shared<TypeInfo>();
				type->name = i;
				s_builtins->push_back(type);
				
				map[typeInfoKey(i)] = std::make_shared<TypeEntry>(type.get(), nullptr, nullptr);
			}
		}
	}
}


ObjectClass::ObjectClass(const TypeInfo *type)
: m_typeInfo(type)
{}

ObjectClass::~ObjectClass() {}

void* ObjectClass::operator new(size_t size) {
	// Initialise memory to zero to match QEMU object initialization
	auto ptr = ::new uint8_t[size];
	memset(ptr, 0, size);
	return ptr;
}


Object::Object()
: m_class(NULL)
, m_container(NULL)
{}

Object::Object(ObjectClass* cls)
: m_class(cls)
, m_container(NULL)
{}

Object::~Object() {}

void* Object::operator new(size_t size) {
	// Initialise memory to zero to match QEMU object initialization
	auto ptr = ::new uint8_t[size];
	memset(ptr, 0, size);
	return ptr;
}


namespace qemu {
	
	void registerType(const TypeInfo *info, struct DeviceClass *cls, const ObjectFactory& factory) {
		SYS_QEMU_TRACE("type_register: name=%s, parent=%s, class=0x%p\n", info->name, info->parent, cls);

		if (s_types == NULL) {
			s_types = new TypesByName();
			populateBuiltinTypes(*s_types);
		}
		
		const auto key = typeInfoKey(info->name);
		
		if (s_types->find(key) != s_types->end()) {
			// already registered
		} else {
			(*s_types)[key] = std::make_shared<TypeEntry>(info, cls, factory);
		}
	}

	void initObject(::Object *object, const TypeInfo *info) {
		// from qemu object_init_with_type()
		if (info->parent) {
			initObject(object, findTypeInfo(info->parent));
		}
			
		if (info->instance_init) {
			info->instance_init(object);
		}
	}

	const TypeInfo *findTypeInfo(const char *name) {
		return getTypeEntry(name)->m_type;
	}

	DeviceClass *findDeviceClass(const char *name) {
		initClasses();
		return getTypeEntry(name)->m_class;
	}
	
	::Object* createObject(const char *type) {
		auto entry = getTypeEntry(type);
	
		if (entry->m_factory != nullptr) {
			::Object* object = entry->m_factory(entry->m_type);
			return object;
		}
		
		return NULL;
	}

}
