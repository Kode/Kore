#ifdef KORE_OPENGL
#include <GL/glfw.h>
#endif

#include <emscripten/emscripten.h>

#include <kore3/audio/audio.h>
//#include <kore3/graphics4/graphics.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/window.h>

#include <stdio.h>
#include <stdlib.h>

static int html5_argc;
static char **html5_argv;
static bool initialized = false;

static void drawfunc() {
	if (!initialized)
		return;
	kore_internal_update_callback();
	kore_audio_update();
#ifdef KORE_OPENGL
	glfwSwapBuffers();
#endif
}

#define KEY_DOWN(GLFW_KEYCODE, KORE_KEY)                                                                                                                       \
	case GLFW_KEYCODE:                                                                                                                                         \
		kore_internal_keyboard_trigger_key_down(KORE_KEY);                                                                                                     \
		break;

#define KEY_UP(GLFW_KEYCODE, KORE_KEY)                                                                                                                         \
	case GLFW_KEYCODE:                                                                                                                                         \
		kore_internal_keyboard_trigger_key_up(KORE_KEY);                                                                                                       \
		break;

#ifdef KORE_OPENGL
// glfw mappings as state here: https://www.glfw.org/docs/3.3/group__keys.html
static void onKeyPressed(int key, int action) {
	if (action == GLFW_PRESS) {
		switch (key) {
			KEY_DOWN(32, KORE_KEY_SPACE)
			KEY_DOWN(39, KORE_KEY_QUOTE)
			KEY_DOWN(44, KORE_KEY_COMMA)
			KEY_DOWN(45, KORE_KEY_SUBTRACT)
			KEY_DOWN(46, KORE_KEY_PERIOD)
			KEY_DOWN(47, KORE_KEY_SLASH)
			KEY_DOWN(48, KORE_KEY_0)
			KEY_DOWN(49, KORE_KEY_1)
			KEY_DOWN(50, KORE_KEY_2)
			KEY_DOWN(51, KORE_KEY_3)
			KEY_DOWN(52, KORE_KEY_4)
			KEY_DOWN(53, KORE_KEY_5)
			KEY_DOWN(54, KORE_KEY_6)
			KEY_DOWN(55, KORE_KEY_7)
			KEY_DOWN(56, KORE_KEY_8)
			KEY_DOWN(57, KORE_KEY_9)
			KEY_DOWN(59, KORE_KEY_SEMICOLON)
			KEY_DOWN(61, KORE_KEY_EQUALS)
			KEY_DOWN(65, KORE_KEY_A)
			KEY_DOWN(66, KORE_KEY_B)
			KEY_DOWN(67, KORE_KEY_C)
			KEY_DOWN(68, KORE_KEY_D)
			KEY_DOWN(69, KORE_KEY_E)
			KEY_DOWN(70, KORE_KEY_F)
			KEY_DOWN(71, KORE_KEY_G)
			KEY_DOWN(72, KORE_KEY_H)
			KEY_DOWN(73, KORE_KEY_I)
			KEY_DOWN(74, KORE_KEY_J)
			KEY_DOWN(75, KORE_KEY_K)
			KEY_DOWN(76, KORE_KEY_L)
			KEY_DOWN(77, KORE_KEY_M)
			KEY_DOWN(78, KORE_KEY_N)
			KEY_DOWN(79, KORE_KEY_O)
			KEY_DOWN(80, KORE_KEY_P)
			KEY_DOWN(81, KORE_KEY_Q)
			KEY_DOWN(82, KORE_KEY_R)
			KEY_DOWN(83, KORE_KEY_S)
			KEY_DOWN(84, KORE_KEY_T)
			KEY_DOWN(85, KORE_KEY_U)
			KEY_DOWN(86, KORE_KEY_V)
			KEY_DOWN(87, KORE_KEY_W)
			KEY_DOWN(88, KORE_KEY_X)
			KEY_DOWN(89, KORE_KEY_Y)
			KEY_DOWN(90, KORE_KEY_Z)
			KEY_DOWN(92, KORE_KEY_BACK_SLASH)
			KEY_DOWN(256, KORE_KEY_ESCAPE)
			KEY_DOWN(257, KORE_KEY_RETURN)
			KEY_DOWN(258, KORE_KEY_TAB)
			KEY_DOWN(259, KORE_KEY_BACKSPACE)
			KEY_DOWN(260, KORE_KEY_INSERT)
			KEY_DOWN(261, KORE_KEY_DELETE)
			KEY_DOWN(262, KORE_KEY_RIGHT)
			KEY_DOWN(263, KORE_KEY_LEFT)
			KEY_DOWN(264, KORE_KEY_DOWN)
			KEY_DOWN(265, KORE_KEY_UP)
			KEY_DOWN(268, KORE_KEY_HOME)
			KEY_DOWN(269, KORE_KEY_END)
			KEY_DOWN(284, KORE_KEY_PAUSE)
			KEY_DOWN(290, KORE_KEY_F1)
			KEY_DOWN(291, KORE_KEY_F2)
			KEY_DOWN(292, KORE_KEY_F3)
			KEY_DOWN(293, KORE_KEY_F4)
			KEY_DOWN(294, KORE_KEY_F5)
			KEY_DOWN(295, KORE_KEY_F6)
			KEY_DOWN(296, KORE_KEY_F7)
			KEY_DOWN(297, KORE_KEY_F8)
			KEY_DOWN(298, KORE_KEY_F9)
			KEY_DOWN(299, KORE_KEY_F10)
			KEY_DOWN(300, KORE_KEY_F11)
			KEY_DOWN(301, KORE_KEY_F12)
			KEY_DOWN(302, KORE_KEY_F13)
			KEY_DOWN(303, KORE_KEY_F14)
			KEY_DOWN(304, KORE_KEY_F15)
			KEY_DOWN(305, KORE_KEY_F16)
			KEY_DOWN(306, KORE_KEY_F17)
			KEY_DOWN(307, KORE_KEY_F18)
			KEY_DOWN(308, KORE_KEY_F19)
			KEY_DOWN(309, KORE_KEY_F20)
			KEY_DOWN(310, KORE_KEY_F21)
			KEY_DOWN(311, KORE_KEY_F22)
			KEY_DOWN(312, KORE_KEY_F23)
			KEY_DOWN(313, KORE_KEY_F24)
			KEY_DOWN(348, KORE_KEY_CONTEXT_MENU)
		}
	}
	else {
		switch (key) {
			KEY_UP(32, KORE_KEY_SPACE)
			KEY_UP(39, KORE_KEY_QUOTE)
			KEY_UP(44, KORE_KEY_COMMA)
			KEY_UP(45, KORE_KEY_SUBTRACT)
			KEY_UP(46, KORE_KEY_PERIOD)
			KEY_UP(47, KORE_KEY_SLASH)
			KEY_UP(48, KORE_KEY_0)
			KEY_UP(49, KORE_KEY_1)
			KEY_UP(50, KORE_KEY_2)
			KEY_UP(51, KORE_KEY_3)
			KEY_UP(52, KORE_KEY_4)
			KEY_UP(53, KORE_KEY_5)
			KEY_UP(54, KORE_KEY_6)
			KEY_UP(55, KORE_KEY_7)
			KEY_UP(56, KORE_KEY_8)
			KEY_UP(57, KORE_KEY_9)
			KEY_UP(59, KORE_KEY_SEMICOLON)
			KEY_UP(61, KORE_KEY_EQUALS)
			KEY_UP(65, KORE_KEY_A)
			KEY_UP(66, KORE_KEY_B)
			KEY_UP(67, KORE_KEY_C)
			KEY_UP(68, KORE_KEY_D)
			KEY_UP(69, KORE_KEY_E)
			KEY_UP(70, KORE_KEY_F)
			KEY_UP(71, KORE_KEY_G)
			KEY_UP(72, KORE_KEY_H)
			KEY_UP(73, KORE_KEY_I)
			KEY_UP(74, KORE_KEY_J)
			KEY_UP(75, KORE_KEY_K)
			KEY_UP(76, KORE_KEY_L)
			KEY_UP(77, KORE_KEY_M)
			KEY_UP(78, KORE_KEY_N)
			KEY_UP(79, KORE_KEY_O)
			KEY_UP(80, KORE_KEY_P)
			KEY_UP(81, KORE_KEY_Q)
			KEY_UP(82, KORE_KEY_R)
			KEY_UP(83, KORE_KEY_S)
			KEY_UP(84, KORE_KEY_T)
			KEY_UP(85, KORE_KEY_U)
			KEY_UP(86, KORE_KEY_V)
			KEY_UP(87, KORE_KEY_W)
			KEY_UP(88, KORE_KEY_X)
			KEY_UP(89, KORE_KEY_Y)
			KEY_UP(90, KORE_KEY_Z)
			KEY_UP(92, KORE_KEY_BACK_SLASH)
			KEY_UP(256, KORE_KEY_ESCAPE)
			KEY_UP(257, KORE_KEY_RETURN)
			KEY_UP(258, KORE_KEY_TAB)
			KEY_UP(259, KORE_KEY_BACKSPACE)
			KEY_UP(260, KORE_KEY_INSERT)
			KEY_UP(261, KORE_KEY_DELETE)
			KEY_UP(262, KORE_KEY_RIGHT)
			KEY_UP(263, KORE_KEY_LEFT)
			KEY_UP(264, KORE_KEY_DOWN)
			KEY_UP(265, KORE_KEY_UP)
			KEY_UP(268, KORE_KEY_HOME)
			KEY_UP(269, KORE_KEY_END)
			KEY_UP(284, KORE_KEY_PAUSE)
			KEY_UP(290, KORE_KEY_F1)
			KEY_UP(291, KORE_KEY_F2)
			KEY_UP(292, KORE_KEY_F3)
			KEY_UP(293, KORE_KEY_F4)
			KEY_UP(294, KORE_KEY_F5)
			KEY_UP(295, KORE_KEY_F6)
			KEY_UP(296, KORE_KEY_F7)
			KEY_UP(297, KORE_KEY_F8)
			KEY_UP(298, KORE_KEY_F9)
			KEY_UP(299, KORE_KEY_F10)
			KEY_UP(300, KORE_KEY_F11)
			KEY_UP(301, KORE_KEY_F12)
			KEY_UP(302, KORE_KEY_F13)
			KEY_UP(303, KORE_KEY_F14)
			KEY_UP(304, KORE_KEY_F15)
			KEY_UP(305, KORE_KEY_F16)
			KEY_UP(306, KORE_KEY_F17)
			KEY_UP(307, KORE_KEY_F18)
			KEY_UP(308, KORE_KEY_F19)
			KEY_UP(309, KORE_KEY_F20)
			KEY_UP(310, KORE_KEY_F21)
			KEY_UP(311, KORE_KEY_F22)
			KEY_UP(312, KORE_KEY_F23)
			KEY_UP(313, KORE_KEY_F24)
			KEY_UP(348, KORE_KEY_CONTEXT_MENU)
		}
	}
}

static int mouseX = 0;
static int mouseY = 0;

static void onMouseClick(int button, int action) {
	if (action == GLFW_PRESS) {
		if (button == 0) {
			kore_internal_mouse_trigger_press(0, 0, mouseX, mouseY);
		}
		else if (button == 1) {
			kore_internal_mouse_trigger_press(0, 1, mouseX, mouseY);
		}
	}
	else {
		if (button == 0) {
			kore_internal_mouse_trigger_release(0, 0, mouseX, mouseY);
		}
		else if (button == 1) {
			kore_internal_mouse_trigger_release(0, 1, mouseX, mouseY);
		}
	}
}

static void onMouseMove(int x, int y) {
	mouseX = x;
	mouseY = y;
	kore_internal_mouse_trigger_move(0, x, y);
}
#endif

static int with, height;

extern int kore_internal_window_width;
extern int kore_internal_window_height;

int kore_init(const char *name, int width, int height, kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	kore_window_parameters defaultWin;
	if (win == NULL) {
		kore_window_options_set_defaults(&defaultWin);
		win = &defaultWin;
	}
	kore_framebuffer_parameters defaultFrame;
	if (frame == NULL) {
		kore_framebuffer_options_set_defaults(&defaultFrame);
		frame = &defaultFrame;
	}
	win->width = width;
	win->height = height;

#ifdef KORE_OPENGL
	glfwInit();
	glfwOpenWindow(width, height, 8, 8, 8, 0, frame->depth_bits, frame->stencil_bits, GLFW_WINDOW);
	glfwSetWindowTitle(name);
	glfwSetKeyCallback(onKeyPressed);
	glfwSetMousePosCallback(onMouseMove);
	glfwSetMouseButtonCallback(onMouseClick);
#endif
	kore_internal_window_width = width;
	kore_internal_window_height = height;
	//kore_g4_internal_init();
	//kore_g4_internal_init_window(0, frame->depth_bits, frame->stencil_bits, true);

	return 0;
}

bool kore_internal_handle_messages() {
	return true;
}

void kore_set_keep_screen_on(bool on) {}

double kore_frequency(void) {
	return 1000.0;
}

kore_ticks kore_timestamp(void) {
#ifdef KORE_OPENGL
	return (kore_ticks)(glfwGetTime() * 1000.0);
#else
	return (kore_ticks)(0.0);
#endif
}

double kore_time(void) {
#ifdef KORE_OPENGL
	return glfwGetTime();
#else
	return 0.0;
#endif
}

int kore_cpu_cores(void) {
	return 4;
}

int kore_hardware_threads(void) {
	return 4;
}

extern int kickstart(int argc, char **argv);

#ifdef KORE_WEBGPU
EMSCRIPTEN_KEEPALIVE void kore_internal_webgpu_initialized() {
	kickstart(html5_argc, html5_argv);
	initialized = true;
}
#endif

int main_(int argc, char **argv) {
	html5_argc = argc;
	html5_argv = argv;
#ifdef KORE_WEBGPU
	char *code = "(async () => {\
		const adapter = await navigator.gpu.requestAdapter();\
		const device = await adapter.requestDevice();\
		Module.preinitializedWebGPUDevice = device;\
		_kore_internal_webgpu_initialized();\
	})();";
	emscripten_run_script(code);
#else
	kickstart(argc, argv);
	initialized = true;
#endif
	emscripten_set_main_loop(drawfunc, 0, 1);

	return 0;
}

#include <webgpu/webgpu.h>
#include <emscripten/html5_webgpu.h>

static WGPUInstance instance;

static const char shaderCode[] = "\
    @vertex\n\
    fn main_v(@builtin(vertex_index) idx: u32) -> @builtin(position) vec4<f32> {\n\
        var pos = array<vec2<f32>, 3>(\n\
            vec2<f32>(0.0, 0.5), vec2<f32>(-0.5, -0.5), vec2<f32>(0.5, -0.5));\n\
        return vec4<f32>(pos[idx], 0.0, 1.0);\n\
    }\n\
\n\
    @fragment\n\
    fn main_f() -> @location(0) vec4<f32> {\n\
        return vec4<f32>(0.0, 0.502, 1.0, 1.0); // 0x80/0xff ~= 0.502\n\
    }\n\
";

static WGPUAdapter adapter;
static WGPUDevice wgpu_device;
static WGPUQueue queue;
static WGPURenderPipeline pipeline;
static int testsCompleted = 0;

void run();

void main3(void) {
    run();
}

void device_callback(WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * userdata) {
    if (message) {
        printf("RequestDevice: %s\n", message);
    }
    assert(status == WGPURequestDeviceStatus_Success);

    wgpu_device = device;

    main3();
}

void GetDevice() {
    wgpuAdapterRequestDevice(adapter, NULL, device_callback, NULL);
}

void main2(void) {
    WGPUAdapterInfo info;
    wgpuAdapterGetInfo(adapter, &info);
    printf("adapter vendor: %s\n", info.vendor);
    printf("adapter architecture: %s\n", info.architecture);
    printf("adapter device: %s\n", info.device);
    printf("adapter description: %s\n", info.description);

    GetDevice();
}

void adapter_callback(WGPURequestAdapterStatus status, WGPUAdapter _adapter, char const * message, void * userdata) {
    if (message) {
        printf("RequestAdapter: %s\n", message);
    }
    assert(status == WGPURequestAdapterStatus_Success);

    adapter = _adapter;

    main2();
}

void GetAdapter() {
	instance = wgpuCreateInstance(NULL);
    wgpuInstanceRequestAdapter(instance, NULL, adapter_callback, NULL);
}

void error_callback(WGPUErrorType errorType, const char* message, void* userdata) {
    printf("%d: %s\n", errorType, message);
}

void compilation_info_callback(WGPUCompilationInfoRequestStatus status, const WGPUCompilationInfo* info, void* userdata) {
    assert(status == WGPUCompilationInfoRequestStatus_Success);
    assert(info->messageCount == 0);
    printf("Shader compile succeeded\n");
}

void init() {
    wgpuDeviceSetUncapturedErrorCallback(wgpu_device, error_callback, NULL);

    queue = wgpuDeviceGetQueue(wgpu_device);

    WGPUShaderModule shaderModule;
    {
        WGPUShaderModuleWGSLDescriptor wgslDesc = {0};
        wgslDesc.code = shaderCode;
        wgslDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;

        WGPUShaderModuleDescriptor descriptor = {0};
        descriptor.nextInChain = (WGPUChainedStruct *)&wgslDesc;
        shaderModule = wgpuDeviceCreateShaderModule(wgpu_device, &descriptor);
        wgpuShaderModuleGetCompilationInfo(shaderModule, compilation_info_callback, NULL);
    }

    {
        WGPUPipelineLayoutDescriptor pl = {0};
        pl.bindGroupLayoutCount = 0;
        pl.bindGroupLayouts = NULL;

        WGPUColorTargetState colorTargetState = {0};
        colorTargetState.format = WGPUTextureFormat_BGRA8Unorm;
        colorTargetState.writeMask = WGPUColorWriteMask_All;

        WGPUFragmentState fragmentState = {0};
        fragmentState.module = shaderModule;
        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTargetState;

        WGPUMultisampleState multisample_state = {
            .count = 1,
            .mask = 0xffffffff,
            .alphaToCoverageEnabled = false,
        };

        WGPURenderPipelineDescriptor descriptor = {0};
        descriptor.layout = wgpuDeviceCreatePipelineLayout(wgpu_device, &pl);
        descriptor.vertex.module = shaderModule;
        descriptor.fragment = &fragmentState;
        descriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        descriptor.multisample = multisample_state;
        pipeline = wgpuDeviceCreateRenderPipeline(wgpu_device, &descriptor);
    }
}

void render(WGPUTextureView view) {
    WGPURenderPassColorAttachment attachment = {0};
    attachment.view = view;
    attachment.loadOp = WGPULoadOp_Clear;
    attachment.storeOp = WGPUStoreOp_Store;
    attachment.clearValue.r = 0.5f;
	attachment.clearValue.g = 0.0f;
	attachment.clearValue.b = 0.0f;
	attachment.clearValue.a = 1.0f;
    attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    WGPURenderPassDescriptor renderpass = {0};
    renderpass.colorAttachmentCount = 1;
    renderpass.colorAttachments = &attachment;

    WGPUCommandBuffer commands;
    {
        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpu_device, NULL);
        {
            WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &renderpass);
            wgpuRenderPassEncoderSetPipeline(pass, pipeline);
            wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
            wgpuRenderPassEncoderEnd(pass);
        }
        commands = wgpuCommandEncoderFinish(encoder, NULL);
    }

    wgpuQueueSubmit(queue, 1, &commands);
}


WGPUSurface surface;

const uint32_t kWidth = 300;
const uint32_t kHeight = 150;

void frame() {
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);

    WGPUTextureView backbuffer = wgpuTextureCreateView(surfaceTexture.texture, NULL);
    render(backbuffer);
}

void run() {
    init();

    {
        WGPUSurfaceDescriptorFromCanvasHTMLSelector canvasDesc = {0};
        canvasDesc.selector = "#canvas";
        canvasDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;

        WGPUSurfaceDescriptor surfDesc = {0};
        surfDesc.nextInChain = (WGPUChainedStruct *)&canvasDesc;
        surface = wgpuInstanceCreateSurface(instance, &surfDesc);

        WGPUSurfaceCapabilities capabilities;
        wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);

        WGPUSurfaceConfiguration config = {
            .device = wgpu_device,
            .format = capabilities.formats[0],
            .usage = WGPUTextureUsage_RenderAttachment,
            .alphaMode = WGPUCompositeAlphaMode_Auto,
            .width = kWidth,
            .height = kHeight,
            .presentMode = WGPUPresentMode_Fifo};
        wgpuSurfaceConfigure(surface, &config);
    }
    emscripten_set_main_loop(frame, 0, false);
}

int main() {
    GetAdapter();
    return 0;
}
