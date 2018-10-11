/*
 *	PearPC
 *	rendermetal.h
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

#ifdef __OBJC__

#import "system/renderer.h"
#import "system/display.h"
#import <MetalKit/MetalKit.h>
#import <memory>
#import <vector>

namespace pearpc {
	
	template <typename _pos, typename _texcoord, typename _color>
	struct ShaderVertex
	{
		typedef _pos pos_type;
		typedef _texcoord texcoord_type;
		typedef _color color_type;
		
		ShaderVertex() {}
		
		ShaderVertex(const pos_type& pos, const texcoord_type& texcoord, const color_type& color)
		: m_pos(pos)
		, m_texcoord(texcoord)
		, m_color(color)
		{}
		
		pos_type m_pos;
		texcoord_type m_texcoord;
		color_type m_color;
	};
	
	typedef ShaderVertex<simd_float2, simd_float2, simd_uchar4> Vertex2d;
	
	class IndexedPrimitiveMetal
	{
	public:
		IndexedPrimitiveMetal(id<MTLRenderPipelineState> state, id<MTLBuffer> buffer, id<MTLBuffer> indexBuffer, MTLPrimitiveType type)
		: m_state(state)
		, m_buffer(buffer)
		, m_indexBuffer(indexBuffer)
		, m_type(type)
		{}
		
		id<MTLBuffer> getBuffer() const { return m_buffer; }
		id<MTLBuffer> getIndexBuffer() const { return m_indexBuffer; }
		id<MTLRenderPipelineState> getState() const { return m_state; }
		
		MTLPrimitiveType getType() const { return m_type; }
		MTLIndexType getIndexType() const { return MTLIndexTypeUInt16; }
		size_t getIndexCount() const { return m_indexBuffer.length / 2; }
		
	private:
		id<MTLRenderPipelineState> m_state;
		id<MTLBuffer> m_buffer;
		id<MTLBuffer> m_indexBuffer;
		MTLPrimitiveType m_type;
	};
	
	class RenderMetal : public SystemRenderer
	{
	public:
		RenderMetal(MTKView* view);
		
		void loadResources(MTKView* view);
		
		virtual byte* createFrameBuffer(const DisplayCharacteristics&);
		virtual bool destroyFrameBuffer(byte* buffer);

		virtual void clientResolutionChanged(const DisplayCharacteristics& display);
		
		/// Register a block to be called when the client resolution changes
		void onClientResolutionChange(void (^block)(const DisplayCharacteristics&)) { m_onClientResolutionChanged = block; }
		
		void draw(MTKView* view);
		void drawableSizeWillChange(CGSize size);
		
	private:
		void updateUniforms();
		
	private:
		MTKView* m_view;
		id<MTLDevice> m_device;
		id<MTLLibrary> m_library;
		
		id<MTLCommandQueue> m_commandQueue;
		dispatch_semaphore_t m_inFlightSemaphore;
		
		id<MTLTexture> m_defaultTexture;
		
		// Orthographic render state
		id<MTLRenderPipelineState> m_orthoPipelineState;
		matrix_float4x4 m_orthoProjection;
		
		// Client screen display
		std::shared_ptr<IndexedPrimitiveMetal> m_screenPrimitive;
		id<MTLTexture> m_screenTexture;
		MTLViewport m_screenViewPort;
		
		// Frame buffer conversion
		id<MTLBuffer> m_sharedFrameBuffer;
		id<MTLComputePipelineState> m_convert15bppState;
		id<MTLComputePipelineState> m_convert32bppState;
		
		// Uniforms
		id<MTLBuffer> m_dynamicUniformBuffer;
		uint32_t m_uniformBufferOffset;
		uint8_t m_uniformBufferIndex;
		void* m_uniformBufferAddress;
		
		// Callback blocks
		void (^m_onClientResolutionChanged)(const DisplayCharacteristics&);
	};
	
}

#endif
