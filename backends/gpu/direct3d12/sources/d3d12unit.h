#ifndef KORE_D3D12_UNIT_HEADER
#define KORE_D3D12_UNIT_HEADER

#include <kore3/global.h>

#ifdef KORE_WINDOWS
// Windows 7
#define WINVER 0x0601
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0601

#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCOMM
#ifndef KORE_PIX
#define NOCTLMGR
#endif
#define NODEFERWINDOWPOS
#define NODRAWTEXT
#define NOGDI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOKANJI
#define NOKEYSTATES
#define NOMB
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMINMAX
// #define NOMSG
#define NONLS
#define NOOPENFILE
#define NOPROFILER
#define NORASTEROPS
#define NOSCROLL
#define NOSERVICE
#ifndef KORE_PIX
#define NOSHOWWINDOW
#endif
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOTEXTMETRIC
// #define NOUSER
#define NOVIRTUALKEYCODES
#define NOWH
#ifndef KORE_PIX
#define NOWINMESSAGES
#endif
#define NOWINOFFSETS
#define NOWINSTYLES
#define WIN32_LEAN_AND_MEAN

#include <d3d12.h>
#include <dxgi.h>

#else

#include <kore3/backend/d3d12_turbo_chapmion_edition.h>

#endif

#include <stdbool.h>

#ifdef __cplusplus
#define COM_CALL_VOID(object, method)     object->method()
#define COM_CALL_VOID_RET(object, method) ret = object->method()
#define COM_CALL(object, method, ...)     object->method(__VA_ARGS__)
#define COM_OUT(id, out)                  __uuidof(**(out)), (static_cast<IGraphicsUnknown *>(*(out)), reinterpret_cast<void **>(out))
#define COM_CREATE(object, method, id, out, ...) \
	object->method(__VA_ARGS__, __uuidof(**(out)), (static_cast<IGraphicsUnknown *>(*(out)), reinterpret_cast<void **>(out)))
#else
#define COM_CALL_VOID(object, method)            object->lpVtbl->method(object)
#define COM_CALL_VOID_RET(object, method, ret)   object->lpVtbl->method(object, &ret)
#define COM_CALL(object, method, ...)            object->lpVtbl->method(object, __VA_ARGS__)
#define COM_OUT(id, out)                         &IID_##id, out
#define COM_CREATE(object, method, id, out, ...) object->lpVtbl->method(object, __VA_ARGS__, &IID_##id, out)
#endif

// https://learn.microsoft.com/en-us/windows/win32/direct3d12/subresources
static inline UINT D3D12CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice, UINT MipLevels, UINT ArraySize) {
	return MipSlice + (ArraySlice * MipLevels) + (PlaneSlice * MipLevels * ArraySize);
}

struct kore_gpu_device;

static void wait_for_fence(struct kore_gpu_device *device, ID3D12Fence *fence, HANDLE event, UINT64 completion_value);
static bool check_for_fence(ID3D12Fence *fence, UINT64 completion_value);

#endif
