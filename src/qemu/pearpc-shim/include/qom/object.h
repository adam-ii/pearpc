/*
 *	PearPC
 *	include/qom/object.h
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

#ifndef QEMU_OBJECT_H
#define QEMU_OBJECT_H

#include <functional>

struct Object;
struct ObjectClass;
struct DeviceClass;


///	\class TypeInfo
///	\brief struct TypeInfo from QEMU
struct TypeInfo
{
	const char *name;
	const char *parent;
	
	size_t instance_size;
	void (*instance_init)(Object *obj);
	void (*instance_post_init)(Object *obj);
	void (*instance_finalize)(Object *obj);
	
	bool abstract;
	size_t class_size;
	
	void (*class_init)(ObjectClass *klass, void *data);
	void (*class_base_init)(ObjectClass *klass, void *data);
	void (*class_finalize)(ObjectClass *klass, void *data);
	void *class_data;
};


///	\class ObjectClass
///	\brief C++ implementation of QEMU ObjectClass
struct ObjectClass
{
	ObjectClass(const TypeInfo *type);
	virtual ~ObjectClass() = 0;
	
	const TypeInfo *getTypeInfo() const { return m_typeInfo; }

	void *operator new(size_t size);

private:
	const TypeInfo *m_typeInfo;
};


///	\class Object
///	\brief C++ implementation of QEMU Object
struct Object
{
	Object();
	Object(ObjectClass* cls);
	virtual ~Object() = 0;
	
	ObjectClass *getClass() const { return m_class; }
	
	Object *getContainer() const { return m_container; }
	void setContainer(Object *o) { m_container = o; }
	
	const char *getTypeName() const { return m_class->getTypeInfo()->name; }

	void *operator new(size_t size);

private:
	ObjectClass *m_class;
	Object *m_container;
};


#define OBJECT_GET_CLASS(cls, obj, name) \
	dynamic_cast<cls*>(obj->getClass())

#define OBJECT_CLASS_CHECK(class_type, klass, name) \
	dynamic_cast<class_type*>(klass)

#define OBJECT_CHECK(type, obj, name) \
	dynamic_cast<type*>(obj)


// From include/qemu/module.h
#define type_init(function) \
	static void __attribute__((constructor)) do_qemu_init_ ## function(void) { function(); }


// C++ extensions
namespace qemu {

	typedef std::function<Object *(const TypeInfo *info)> ObjectFactory;

	/// Register a type
	void registerType(const TypeInfo *info, DeviceClass *cls, const ObjectFactory& factory);
	
	/// Initialize an instance
	void initObject(Object *object, const TypeInfo *info);

	/// Register a type with a DeviceClass object
	template <typename class_type, typename instance_type>
	void type_register(const TypeInfo *info)
	{
		class_type* cls = new class_type(info);
		registerType(info, cls, [cls] (const TypeInfo *info) {
			Object* obj = new instance_type(cls);
			initObject(obj, info);
			return obj;
		});
	}

	/// Find TypeInfo by name
	const TypeInfo *findTypeInfo(const char *name);
	
	/// Find a DeviceClass object by name
	DeviceClass *findDeviceClass(const char *name);
	
	/// Create an object with a given type name
	Object* createObject(const char *type);
	
}

#endif
