/*
 *	PearPC
 *	shader_types.h
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

#ifndef __SHADER_TYPES_H__
#define __SHADER_TYPES_H__

#ifdef __METAL_VERSION__
#define NS_ENUM(_type, _name) enum _name : _type _name; enum _name : _type
#define NSInteger metal::int32_t
#else
#import <Foundation/Foundation.h>
#endif

#include <simd/simd.h>

typedef NS_ENUM(NSInteger, VertexAttribute)
{
	VertexAttributePosition  = 0,
	VertexAttributeTexcoord  = 1,
	VertexAttributeColor     = 2,
};

typedef NS_ENUM(NSInteger, BufferIndex)
{
	BufferIndexMeshPositions = 0,
	BufferIndexUniforms      = 1
};

typedef NS_ENUM(NSInteger, TextureIndex)
{
	TextureIndexColor    = 0,
};

typedef struct
{
	matrix_float4x4 orthoProjection;
	
	struct
	{
		uint32_t width;
		uint32_t height;
		
		uint8_t redSize;
		uint8_t redShift;
		uint8_t blueSize;
		uint8_t blueShift;
		uint8_t greenSize;
		uint8_t greenShift;
	} clientMode;
} Uniforms;

#endif
