#ifndef KORE_D3D12_UNIT_HEADER
#define KORE_D3D12_UNIT_HEADER

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

#include <stdbool.h>

#ifndef IID_GRAPHICS_PPV_ARGS
#define IID_GRAPHICS_PPV_ARGS(x) IID_PPV_ARGS(x)
#endif

#ifdef __cplusplus
#define COM_CALL0(object, method)                                           object->method()
#define COM_CALL0RET(object, method, ret)                                   ret = object->method()
#define COM_CALL1(object, method, arg0)                                     object->method(arg0)
#define COM_CALL2(object, method, arg0, arg1)                               object->method(arg0, arg1)
#define COM_CALL3(object, method, arg0, arg1, arg2)                         object->method(arg0, arg1, arg2)
#define COM_CALL4(object, method, arg0, arg1, arg2, arg3)                   object->method(arg0, arg1, arg2, arg3)
#define COM_CALL5(object, method, arg0, arg1, arg2, arg3, arg4)             object->method(arg0, arg1, arg2, arg3, arg4)
#define COM_CALL6(object, method, arg0, arg1, arg2, arg3, arg4, arg5)       object->method(arg0, arg1, arg2, arg3, arg4, arg5)
#define COM_CALL7(object, method, arg0, arg1, arg2, arg3, arg4, arg5, arg6) object->method(arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#else
#define COM_CALL0(object, method)                                           object->lpVtbl->method(object)
#define COM_CALL0RET(object, method, ret)                                   object->lpVtbl->method(object, &ret)
#define COM_CALL1(object, method, arg0)                                     object->lpVtbl->method(object, arg0)
#define COM_CALL2(object, method, arg0, arg1)                               object->lpVtbl->method(object, arg0, arg1)
#define COM_CALL3(object, method, arg0, arg1, arg2)                         object->lpVtbl->method(object, arg0, arg1, arg2)
#define COM_CALL4(object, method, arg0, arg1, arg2, arg3)                   object->lpVtbl->method(object, arg0, arg1, arg2, arg3)
#define COM_CALL5(object, method, arg0, arg1, arg2, arg3, arg4)             object->lpVtbl->method(object, arg0, arg1, arg2, arg3, arg4)
#define COM_CALL6(object, method, arg0, arg1, arg2, arg3, arg4, arg5)       object->lpVtbl->method(object, arg0, arg1, arg2, arg3, arg4, arg5)
#define COM_CALL7(object, method, arg0, arg1, arg2, arg3, arg4, arg5, arg6) object->lpVtbl->method(object, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#endif

// https://learn.microsoft.com/en-us/windows/win32/direct3d12/subresources
static inline UINT D3D12CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice, UINT MipLevels, UINT ArraySize) {
	return MipSlice + (ArraySlice * MipLevels) + (PlaneSlice * MipLevels * ArraySize);
}

struct kore_gpu_device;

static void wait_for_fence(struct kore_gpu_device *device, ID3D12Fence *fence, HANDLE event, UINT64 completion_value);
static bool check_for_fence(ID3D12Fence *fence, UINT64 completion_value);

#endif
