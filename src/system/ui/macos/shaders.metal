/*
 *	PearPC
 *	shaders.metal
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

#include <metal_stdlib>
#include <simd/simd.h>
#import "shader_types.h"

using namespace metal;

typedef struct
{
	vector_float3 position [[attribute(VertexAttributePosition)]];
	vector_float2 texCoord [[attribute(VertexAttributeTexcoord)]];
	vector_float4 color [[attribute(VertexAttributeColor)]];
} OrthoVertex;

typedef struct
{
	float4 position [[position]];
	float2 texCoord;
	float4 diffuse;
} ColorInOut;

vertex ColorInOut orthoVertexShader(OrthoVertex in [[stage_in]],
									constant Uniforms & uniforms [[ buffer(BufferIndexUniforms) ]])
{
	ColorInOut out;
	out.position = uniforms.orthoProjection * float4(in.position, 1.0f);
	out.texCoord = in.texCoord;
	out.diffuse = in.color;
	return out;
}

fragment float4 fragmentShader(ColorInOut in [[stage_in]],
							   constant Uniforms & uniforms [[ buffer(BufferIndexUniforms) ]],
							   texture2d<half> colorMap     [[ texture(TextureIndexColor) ]])
{
	constexpr sampler colorSampler(mip_filter::linear,
								   mag_filter::linear,
								   min_filter::linear);
	
	half4 colorSample = colorMap.sample(colorSampler, in.texCoord.xy);
	
	return float4(colorSample) * in.diffuse;
}

kernel void convert15bpp(texture2d<float, access::write> outTexture [[ texture(0) ]],
						 device uint16_t* buffer15bpp [[ buffer(0) ]],
						 constant Uniforms & uniforms [[ buffer(BufferIndexUniforms) ]],
						 uint2 position [[ thread_position_in_grid ]])
{
	if (position.x >= outTexture.get_width() || position.y >= outTexture.get_height()) {
		return;
	}
	
	// The frame buffer is big endian so swap the bytes
	const uint16_t pixel15 = buffer15bpp[position.y * uniforms.clientMode.width + position.x];
	const uint16_t sample = (pixel15 >> 8) | (pixel15 << 8);
	
	const uchar red = ((sample >> uniforms.clientMode.redShift) & ((1 << uniforms.clientMode.redSize) - 1)) << (8 - uniforms.clientMode.redSize);
	const uchar green = ((sample >> uniforms.clientMode.greenShift) & ((1 << uniforms.clientMode.greenSize) - 1)) << (8 - uniforms.clientMode.greenSize);
	const uchar blue = ((sample >> uniforms.clientMode.blueShift) & ((1 << uniforms.clientMode.blueSize) - 1)) << (8 - uniforms.clientMode.blueSize);
	
	const float4 color(red / 255.f, green / 255.f, blue / 255.f, 1.f);
	outTexture.write(color, position);
}

kernel void convert32bpp(texture2d<float, access::write> outTexture [[ texture(0) ]],
						 device uchar4* buffer32bpp [[ buffer(0) ]],
						 constant Uniforms & uniforms [[ buffer(BufferIndexUniforms) ]],
						 uint2 position [[ thread_position_in_grid ]])
{
	if (position.x >= outTexture.get_width() || position.y >= outTexture.get_height()) {
		return;
	}
	
	const uchar4 sample = buffer32bpp[position.y * uniforms.clientMode.width + position.x];
	
	const uchar red = sample.y;
	const uchar green = sample.z;
	const uchar blue = sample.w;
	
	const float4 color(red / 255.f, green / 255.f, blue / 255.f, 1.f);
	outTexture.write(color, position);
}
