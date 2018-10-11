/*
 *	PearPC
 *	rendermetal.mm
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

#import "rendermetal.h"
#import "shader_types.h"

extern uint8* gFrameBuffer;

namespace pearpc {
	
	static const NSUInteger kMaxBuffersInFlight = 3;
	static const size_t kAlignedUniformsSize = (sizeof(Uniforms) & ~0xFF) + 0x100;
	
	namespace {
		
		static const std::vector<uint16_t> kQuadIndices { 0, 1, 3, 3, 1, 2 };
		static const simd_uchar4 kColorWhite = simd_make_uchar4(255, 255, 255, 255);
		
		MTLVertexDescriptor* CreateVertex2dDescriptor()
		{
			MTLVertexDescriptor* desc = [[MTLVertexDescriptor alloc] init];
			
			desc.attributes[VertexAttributePosition].format = MTLVertexFormatFloat2;
			desc.attributes[VertexAttributePosition].offset = offsetof(Vertex2d, m_pos);
			desc.attributes[VertexAttributePosition].bufferIndex = BufferIndexMeshPositions;
			
			desc.attributes[VertexAttributeTexcoord].format = MTLVertexFormatFloat2;
			desc.attributes[VertexAttributeTexcoord].offset = offsetof(Vertex2d, m_texcoord);
			desc.attributes[VertexAttributeTexcoord].bufferIndex = BufferIndexMeshPositions;
			
			desc.attributes[VertexAttributeColor].format = MTLVertexFormatUChar4Normalized;
			desc.attributes[VertexAttributeColor].offset = offsetof(Vertex2d, m_color);
			desc.attributes[VertexAttributeColor].bufferIndex = BufferIndexMeshPositions;
			
			desc.layouts[BufferIndexMeshPositions].stride = sizeof(Vertex2d);
			desc.layouts[BufferIndexMeshPositions].stepRate = 1;
			desc.layouts[BufferIndexMeshPositions].stepFunction = MTLVertexStepFunctionPerVertex;
			
			return desc;
		}
		
		matrix_float4x4 MakeOrthoProjection(const CGRect& rect, float zNear, float zFar)
		{
			const float left = rect.origin.x;
			const float right = rect.size.width;
			const float top = rect.origin.y;
			const float bottom = rect.size.height;
			
			const float tx = -(right + left) / (right - left);
			const float ty = -(top + bottom) / (top - bottom);
			const float tz = -(zFar + zNear) / (zFar - zNear);
			
			matrix_float4x4 m;
			m.columns[0] = simd_make_float4(2.f / (right - left), 0.f, 0.f, 0.f);
			m.columns[1] = simd_make_float4(0.f, 2.f / (top - bottom), 0.f, 0.f);
			m.columns[2] = simd_make_float4(0.f, 0.f, -2.f / (zFar - zNear), 0.f);
			m.columns[3] = simd_make_float4(tx, ty, tz, 1.f);
			return m;
		}
		
		id<MTLRenderPipelineState> CreateOrthoPipelineState(id<MTLDevice> device, id<MTLFunction> vertexFunc, id<MTLFunction> fragmentFunc, MTKView* view, bool enableBlend)
		{
			MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
			desc.label = @"Screen";
			desc.sampleCount = 1;
			desc.vertexFunction = vertexFunc;
			desc.fragmentFunction = fragmentFunc;
			desc.vertexDescriptor = CreateVertex2dDescriptor();
			
			desc.colorAttachments[0].pixelFormat = view.colorPixelFormat;
			desc.colorAttachments[0].blendingEnabled = enableBlend;
			if (enableBlend)
			{
				desc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
				desc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
				desc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
				desc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
			}
			
			desc.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
			desc.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;
			
			NSError* error = NULL;
			id<MTLRenderPipelineState> state = [device newRenderPipelineStateWithDescriptor:desc error:&error];
			if (!state)
			{
				NSLog(@"Failed to created pipeline state, error %@", error);
			}
			
			return state;
		}
		
		std::vector<Vertex2d> CreateQuad2d(float x, float y, float width, float height, simd_uchar4 color)
		{
			std::vector<Vertex2d> v(4, Vertex2d());
			
			v.at(0).m_pos = vector2(x        , y);
			v.at(1).m_pos = vector2(x + width, y);
			v.at(2).m_pos = vector2(x + width, y + height);
			v.at(3).m_pos = vector2(x        , y + height);
			
			v.at(0).m_texcoord = vector2(0.f, 0.f);
			v.at(1).m_texcoord = vector2(1.f, 0.f);
			v.at(2).m_texcoord = vector2(1.f, 1.f);
			v.at(3).m_texcoord = vector2(0.f, 1.f);
			
			v.at(0).m_color = color;
			v.at(1).m_color = color;
			v.at(2).m_color = color;
			v.at(3).m_color = color;
			
			return v;
		}
		
		std::shared_ptr<IndexedPrimitiveMetal> CreateIndexedPrimitive(id<MTLDevice> device, id<MTLRenderPipelineState> state, const std::vector<Vertex2d>& verts, const std::vector<uint16_t>& indices, MTLPrimitiveType type)
		{
			id<MTLBuffer> buffer = [device newBufferWithBytes:&verts.at(0)
													   length:verts.size() * sizeof(verts[0])
													  options:MTLResourceStorageModeShared];
			
			id<MTLBuffer> indexBuffer = [device newBufferWithBytes:&indices.at(0)
															length:indices.size() * sizeof(indices[0])
														   options:MTLResourceStorageModeShared];
			
			return std::make_shared<IndexedPrimitiveMetal>(state, buffer, indexBuffer, type);
		}
		
		std::shared_ptr<IndexedPrimitiveMetal> CreateScreenPrimitive(id<MTLDevice> device, id<MTLRenderPipelineState> state, const DisplayCharacteristics& chr)
		{
			auto verts = CreateQuad2d(0.f, 0.f, chr.width, chr.height, kColorWhite);
			return CreateIndexedPrimitive(device, state, verts, kQuadIndices, MTLPrimitiveTypeTriangle);
		}
		
		id<MTLTexture> CreateScreenTexture(id<MTLDevice> device, const DisplayCharacteristics& chr)
		{
			const MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;
			
			MTLTextureDescriptor* descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
																								  width:chr.width
																								 height:chr.height
																							  mipmapped:NO];
			descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
			
			return [device newTextureWithDescriptor:descriptor];
		}
		
		id<MTLTexture> CreateDefaultTexture(id<MTLDevice> device)
		{
			simd_uchar4 data[1] = { kColorWhite };
			
			MTLTextureDescriptor* descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
																								  width:1
																								 height:1
																							  mipmapped:NO];
			
			id<MTLTexture> texture = [device newTextureWithDescriptor:descriptor];
			[texture replaceRegion:MTLRegionMake2D(0, 0, 1, 1)
					   mipmapLevel:0
						 withBytes:data
					   bytesPerRow:sizeof(data)];
			return texture;
		}
		
		id<MTLComputePipelineState> CreateComputePipelineState(id<MTLDevice> device, id<MTLLibrary> library, NSString* functionName)
		{
			id<MTLFunction> function = [library newFunctionWithName:functionName];
			
			NSError* error = NULL;
			auto state = [device newComputePipelineStateWithFunction:function error:&error];
			if (!state)
			{
				[NSException raise:NSGenericException format:@"Failed to created pipeline state (%@)", error];
			}
			return state;
		}
		
		void DrawIndexedPrimitive(id<MTLRenderCommandEncoder> renderEncoder, const std::shared_ptr<IndexedPrimitiveMetal>& primitive)
		{
			[renderEncoder setRenderPipelineState:primitive->getState()];
			
			[renderEncoder setVertexBuffer:primitive->getBuffer()
									offset:0
								   atIndex:BufferIndexMeshPositions];
			
			[renderEncoder drawIndexedPrimitives:primitive->getType()
									  indexCount:primitive->getIndexCount()
									   indexType:primitive->getIndexType()
									 indexBuffer:primitive->getIndexBuffer()
							   indexBufferOffset:0];
		}
	}
	
	RenderMetal::RenderMetal(MTKView* view)
	: m_view(view)
	, m_device(view.device)
	, m_library([m_device newDefaultLibrary])
	, m_inFlightSemaphore(dispatch_semaphore_create(kMaxBuffersInFlight))
	{
		view.depthStencilPixelFormat = MTLPixelFormatInvalid;
		view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
		view.sampleCount = 1;
	}
	
	void RenderMetal::loadResources(MTKView* view)
	{
		// One time resource creation
		m_commandQueue = [m_device newCommandQueue];
		
		m_defaultTexture = CreateDefaultTexture(m_device);
		
		auto orthoVert = [m_library newFunctionWithName:@"orthoVertexShader"];
		auto orthoFrag = [m_library newFunctionWithName:@"fragmentShader"];
		m_orthoPipelineState = CreateOrthoPipelineState(m_device, orthoVert, orthoFrag, view, true);
		
		// Shared frame buffer conversion compute functions
		m_convert15bppState = CreateComputePipelineState(m_device, m_library, @"convert15bpp");
		m_convert32bppState = CreateComputePipelineState(m_device, m_library, @"convert32bpp");
		
		// Create uniform buffers
		const NSUInteger uniformBufferSize = kAlignedUniformsSize * kMaxBuffersInFlight;
		
		m_dynamicUniformBuffer = [m_device newBufferWithLength:uniformBufferSize
													   options:MTLResourceStorageModeShared];
		m_dynamicUniformBuffer.label = @"UniformBuffer";
	}
	
	byte* RenderMetal::createFrameBuffer(const DisplayCharacteristics& chr)
	{
		// Pad buffer size to 4k to suit metal compute
		const size_t frameBufferBytes = ((chr.width * chr.height * chr.bytesPerPixel) & ~4095) + 4096;
		
		m_sharedFrameBuffer = [m_device newBufferWithLength:frameBufferBytes
													options:MTLResourceStorageModeShared];
		
		return reinterpret_cast<byte*>(m_sharedFrameBuffer.contents);
	}
	
	bool RenderMetal::destroyFrameBuffer(byte* buffer)
	{
		if (m_sharedFrameBuffer && m_sharedFrameBuffer.contents == buffer)
		{
			m_sharedFrameBuffer = nil;
			return true;
		}
		return false;
	}
	
	void RenderMetal::clientResolutionChanged(const DisplayCharacteristics& chr)
	{
		m_screenPrimitive = CreateScreenPrimitive(m_device, m_orthoPipelineState, chr);
		m_screenTexture = CreateScreenTexture(m_device, chr);
		
		if (m_onClientResolutionChanged) {
			m_onClientResolutionChanged(chr);
		}
	}
	
	void RenderMetal::draw(MTKView* view)
	{
		dispatch_semaphore_wait(m_inFlightSemaphore, DISPATCH_TIME_FOREVER);
		
		id <MTLCommandBuffer> commandBuffer = [m_commandQueue commandBuffer];
		commandBuffer.label = @"PearPC";
		
		__block dispatch_semaphore_t block_sema = m_inFlightSemaphore;
		[commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
		 {
			 dispatch_semaphore_signal(block_sema);
		 }];
		
		updateUniforms();
		
		if(MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor) {
			if (m_sharedFrameBuffer)
			{
				// Convert shared frame buffer into m_screenTexture
				id<MTLComputeCommandEncoder> computeEncoder = [commandBuffer computeCommandEncoder];
				computeEncoder.label = @"ComputeEncoder";
				
				auto pipelineState = (gDisplay->mClientChar.bytesPerPixel == 2) ? m_convert15bppState : m_convert32bppState;
				[computeEncoder setComputePipelineState:pipelineState];
				
				[computeEncoder setTexture:m_screenTexture atIndex:0];
				[computeEncoder setBuffer:m_sharedFrameBuffer offset:0 atIndex:0];
				[computeEncoder setBuffer:m_dynamicUniformBuffer
								   offset:m_uniformBufferOffset
								  atIndex:BufferIndexUniforms];
				
				const auto w = pipelineState.threadExecutionWidth;
				const auto h = pipelineState.maxTotalThreadsPerThreadgroup / w;
				const auto threadsPerThreadgroup = MTLSizeMake(w, h, 1);
				
				const auto width = (m_screenTexture.width + w - 1) / w;
				const auto height = (m_screenTexture.height + h - 1) / h;
				const auto threadgroupsPerGrid = MTLSizeMake(width, height, 1);
				
				[computeEncoder dispatchThreadgroups:threadgroupsPerGrid threadsPerThreadgroup:threadsPerThreadgroup];
				[computeEncoder endEncoding];
			}
			
			{
				id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
				renderEncoder.label = @"RenderEncoder";
				
				[renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
				[renderEncoder setCullMode:MTLCullModeNone];
				
				[renderEncoder setVertexBuffer:m_dynamicUniformBuffer
										offset:m_uniformBufferOffset
									   atIndex:BufferIndexUniforms];
				
				[renderEncoder setFragmentBuffer:m_dynamicUniformBuffer
										  offset:m_uniformBufferOffset
										 atIndex:BufferIndexUniforms];
				
				// Draw client screen
				[renderEncoder setViewport:m_screenViewPort];
				[renderEncoder setFragmentTexture:m_screenTexture
										  atIndex:TextureIndexColor];
				
				DrawIndexedPrimitive(renderEncoder, m_screenPrimitive);
				
				[renderEncoder endEncoding];
			}
			
			[commandBuffer presentDrawable:view.currentDrawable];
		}
		
		[commandBuffer commit];
	}
	
	void RenderMetal::drawableSizeWillChange(CGSize size)
	{
		const float zNear = -1.f;
		const float zFar = 1.f;
		
		// TODO: aspect fit
		m_screenViewPort.originX = 0;
		m_screenViewPort.originY = 0;
		m_screenViewPort.width = size.width;
		m_screenViewPort.height = size.height;
		m_screenViewPort.znear = zNear;
		m_screenViewPort.zfar = zFar;
		
		m_orthoProjection = MakeOrthoProjection(m_view.bounds, zNear, zFar);
	}
	
	void RenderMetal::updateUniforms()
	{
		m_uniformBufferIndex = (m_uniformBufferIndex + 1) % kMaxBuffersInFlight;
		m_uniformBufferOffset = kAlignedUniformsSize * m_uniformBufferIndex;
		m_uniformBufferAddress = ((uint8_t*)m_dynamicUniformBuffer.contents) + m_uniformBufferOffset;
		
		Uniforms* uniforms = (Uniforms*)m_uniformBufferAddress;
		
		uniforms->orthoProjection = m_orthoProjection;
		
		uniforms->clientMode.width = gDisplay->mClientChar.width;
		uniforms->clientMode.height = gDisplay->mClientChar.height;
		
		uniforms->clientMode.redSize = gDisplay->mClientChar.redSize;
		uniforms->clientMode.redShift = gDisplay->mClientChar.redShift;
		uniforms->clientMode.greenSize = gDisplay->mClientChar.greenSize;
		uniforms->clientMode.greenShift = gDisplay->mClientChar.greenShift;
		uniforms->clientMode.blueSize = gDisplay->mClientChar.blueSize;
		uniforms->clientMode.blueShift = gDisplay->mClientChar.blueShift;
	}
	
}
