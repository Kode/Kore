#ifndef KORE_WINDOWS_OPTIONAL_HEADER
#define KORE_WINDOWS_OPTIONAL_HEADER

#include "win32.h"

#ifdef __cplusplus
extern "C" {
#endif

// Some types for features exclusive to later versions of Windows are copied in here.
// Use with care, make sure not to break backwards-compatibility when using them.

typedef DWORD POINTER_INPUT_TYPE;

typedef UINT32 POINTER_FLAGS;

typedef enum tagPOINTER_BUTTON_CHANGE_TYPE {
	POINTER_CHANGE_NONE,
	POINTER_CHANGE_FIRSTBUTTON_DOWN,
	POINTER_CHANGE_FIRSTBUTTON_UP,
	POINTER_CHANGE_SECONDBUTTON_DOWN,
	POINTER_CHANGE_SECONDBUTTON_UP,
	POINTER_CHANGE_THIRDBUTTON_DOWN,
	POINTER_CHANGE_THIRDBUTTON_UP,
	POINTER_CHANGE_FOURTHBUTTON_DOWN,
	POINTER_CHANGE_FOURTHBUTTON_UP,
	POINTER_CHANGE_FIFTHBUTTON_DOWN,
	POINTER_CHANGE_FIFTHBUTTON_UP,
} POINTER_BUTTON_CHANGE_TYPE;

typedef struct tagPOINTER_INFO {
	POINTER_INPUT_TYPE         pointerType;
	UINT32                     pointerId;
	UINT32                     frameId;
	POINTER_FLAGS              pointerFlags;
	HANDLE                     sourceDevice;
	HWND                       hwndTarget;
	POINT                      ptPixelLocation;
	POINT                      ptHimetricLocation;
	POINT                      ptPixelLocationRaw;
	POINT                      ptHimetricLocationRaw;
	DWORD                      dwTime;
	UINT32                     historyCount;
	INT32                      InputData;
	DWORD                      dwKeyStates;
	UINT64                     PerformanceCount;
	POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;
} POINTER_INFO;

typedef UINT32 PEN_FLAGS;

typedef UINT32 PEN_MASK;

typedef struct tagPOINTER_PEN_INFO {
	POINTER_INFO pointerInfo;
	PEN_FLAGS    penFlags;
	PEN_MASK     penMask;
	UINT32       pressure;
	UINT32       rotation;
	INT32        tiltX;
	INT32        tiltY;
} POINTER_PEN_INFO;

#define WM_POINTERUPDATE 0x0245
#define WM_POINTERDOWN   0x0246
#define WM_POINTERUP     0x0247

#define GET_POINTERID_WPARAM(wParam) (LOWORD(wParam))

enum tagPOINTER_INPUT_TYPE {
	PT_POINTER  = 1, // Generic pointer
	PT_TOUCH    = 2, // Touch
	PT_PEN      = 3, // Pen
	PT_MOUSE    = 4, // Mouse
	PT_TOUCHPAD = 5, // Touchpad
};

typedef enum { MDT_EFFECTIVE_DPI = 0, MDT_ANGULAR_DPI = 1, MDT_RAW_DPI = 2, MDT_DEFAULT = MDT_EFFECTIVE_DPI } MONITOR_DPI_TYPE;

typedef BOOL(WINAPI *get_pointer_info_type)(UINT32 pointerId, POINTER_INFO *pointerInfo);
get_pointer_info_type get_pointer_info;

typedef BOOL(WINAPI *get_pointer_pen_info_type)(UINT32 pointerId, POINTER_PEN_INFO *penInfo);
get_pointer_pen_info_type get_pointer_pen_info;

typedef BOOL(WINAPI *enable_non_client_dpi_scaling_type)(HWND hwnd);
enable_non_client_dpi_scaling_type enable_non_client_dpi_scaling;

typedef HRESULT(WINAPI *get_dpi_for_monitor_type)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);
get_dpi_for_monitor_type get_dpi_for_monitor;

void kore_windows_optional_init(void);

#ifdef __cplusplus
}
#endif

#endif
