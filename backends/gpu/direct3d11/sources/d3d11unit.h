#ifndef KORE_D3D11_UNIT_HEADER
#define KORE_D3D11_UNIT_HEADER

#include <d3d11.h>
#include <dxgi.h>

typedef enum command_type {
	COMMAND_SET_INDEX_BUFFER,
	COMMAND_SET_VERTEX_BUFFER,
	COMMAND_DRAW_INDEXED,
	COMMAND_SET_RENDER_PIPELINE,
	COMMAND_COPY_TEXTURE_TO_BUFFER,
	COMMAND_PRESENT,
	COMMAND_BEGIN_RENDER_PASS,
	COMMAND_END_RENDER_PASS,
} command_type;

#endif
