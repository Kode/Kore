const fs = require('fs');
const path = require('path');

const project = new Project('Kore');

function addKoreDefine(name) {
	project.addDefine('KORE_' + name);
}

// Setting lz4x to false adds a BSD 2-Clause licensed component,
// which is a little more restrictive than Kore's zlib license.
const lz4x = true;

project.addFile('includes/**');
project.addIncludeDir('includes');

function addUnit(name) {
	project.addFile('sources/' + name + '/**', {nocompile: true});
	project.addFile('sources/' + name + '/*unit.c*');
}

function addSimpleUnit(name) {
	project.addFile('sources/' + name + '/*');
}

addUnit('2d');
addSimpleUnit('audio');
addSimpleUnit('framebuffer');
addUnit('gpu');
addUnit('input');
addUnit('io');
addUnit('math');
addUnit('mixer');
addUnit('network');
addUnit('root');
addUnit('util');

if (lz4x) {
	addKoreDefine('LZ4X');
}
else {
	project.addFile('sources/libs/lz4/*');
}

project.addFile('sources/libs/offalloc/*');

function addBackendBase(name) {
	project.addIncludeDir('backends/' + name + '/includes');
	project.addFile('backends/' + name + '/includes/**');
	project.addFile('backends/' + name + '/sources/*', {nocompile: true});
}

function addBackend(name) {
	addBackendBase(name);
	project.addFile('backends/' + name + '/sources/*unit.c*');
	project.addFile('backends/' + name + '/sources/*unit.winrt.c*');
	project.addFile('backends/' + name + '/sources/*unit.m');
}

function addCBackend(name) {
	addBackendBase(name);
	project.addFile('backends/' + name + '/sources/*unit.c');
}

function addSimpleBackend(name) {
	project.addFile('backends/' + name + '/sources/*');
}

let plugin = false;

if (platform === Platform.Windows) {
	addBackend('system/windows');
	addBackend('system/microsoft');
	project.addLib('dxguid');
	project.addLib('dsound');
	project.addLib('dinput8');

	project.addDefine('_CRT_SECURE_NO_WARNINGS');
	project.addDefine('_WINSOCK_DEPRECATED_NO_WARNINGS');
	project.addLib('ws2_32');
	project.addLib('Winhttp');

	const directshow = false;
	if (directshow) {
		project.addFile('backends/system/windows/libs/directshow/**');
		project.addIncludeDir('backends/system/windows/libs/directshow/BaseClasses');
		project.addLib('strmiids');
		project.addLib('winmm');
	}
	else {
		addKoreDefine('NO_DIRECTSHOW');
	}

	project.addLib('wbemuuid');

	if (graphics === GraphicsApi.OpenGL) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		project.addDefine('GLEW_STATIC');
		project.addLib('opengl32');
		project.addLib('glu32');
		project.addFile('backends/gpu/opengl/libs/glew/**');
	}
	else if (graphics === GraphicsApi.Direct3D11) {
		addBackend('gpu/direct3d11');
		addKoreDefine('DIRECT3D');
		addKoreDefine('DIRECT3D11');
		project.addLib('d3d11');
	}
	else if (graphics === GraphicsApi.Direct3D12 || graphics === GraphicsApi.Default) {
		addCBackend('gpu/direct3d12');
		addKoreDefine('DIRECT3D');
		addKoreDefine('DIRECT3D12');
		project.addLib('dxgi');
		project.addLib('d3d12');

		if (Options.pix) {
			addKoreDefine('PIX');
			project.addIncludeDir('backends/gpu/direct3d12/libs/pix/Include');
			project.addLib('backends/gpu/direct3d12/libs/pix/bin/x64/WinPixEventRuntime');
		}
	}
	else if (graphics === GraphicsApi.Vulkan) {
		addBackend('gpu/vulkan');
		addKoreDefine('VULKAN');
		project.addDefine('VK_USE_PLATFORM_WIN32_KHR');
		if (!process.env.VULKAN_SDK) {
			throw 'Could not find a Vulkan SDK';
		}
		project.addLibFor('x64', path.join(process.env.VULKAN_SDK, 'Lib', 'vulkan-1'));
		let libs = fs.readdirSync(path.join(process.env.VULKAN_SDK, 'Lib'));
		for (const lib of libs) {
			if (lib.startsWith('VkLayer_')) {
				project.addLibFor('x64', path.join(process.env.VULKAN_SDK, 'Lib', lib.substr(0, lib.length - 4)));
			}
		}
		project.addIncludeDir(path.join(process.env.VULKAN_SDK, 'Include'));
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for Windows.');
	}

	if (audio === AudioApi.DirectSound) {
		addSimpleBackend('audio/directsound');
	}
	else if (audio === AudioApi.WASAPI || audio === AudioApi.Default) {
		addSimpleBackend('audio/wasapi');
	}
	else {
		throw new Error('Audio API ' + audio + ' is not available for Windows.');
	}

	if (vr === VrApi.Oculus) {
		addKoreDefine('VR');
		addKoreDefine('OCULUS');
		project.addLibFor('x64', 'backends/system/windows/libs/oculus/LibOVR/Lib/Windows/x64/Release/VS2017/LibOVR');
		project.addLibFor('Win32', 'backends/system/windows/libs/oculus/LibOVR/Lib/Windows/Win32/Release/VS2017/LibOVR');
		project.addFile('backends/system/windows/libs/oculus/LibOVRKernel/Src/GL/**');
		project.addIncludeDir('backends/system/windows/libs/oculus/LibOVR/Include/');
		project.addIncludeDir('backends/system/windows/libs/oculus/LibOVRKernel/Src/');
	}
	else if (vr === VrApi.SteamVR) {
		addKoreDefine('VR');
		addKoreDefine('STEAMVR');
		project.addDefine('VR_API_PUBLIC');
		project.addFile('backends/system/windows/libs/steamvr/src/**');
		project.addIncludeDir('backends/system/windows/libs/steamvr/src');
		project.addIncludeDir('backends/system/windows/libs/steamvr/src/vrcommon');
		project.addIncludeDir('backends/system/windows/libs/steamvr/headers');
	}
	else if (vr === VrApi.None) {

	}
	else {
		throw new Error('VR API ' + vr + ' is not available for Windows.');
	}
}
else if (platform === Platform.WindowsApp) {
	addKoreDefine('WINDOWSAPP');
	addBackend('system/windowsapp');
	addBackend('system/microsoft');
	addBackend('gpu/direct3d11');
	addSimpleBackend('audio/wasapi_winrt');
	project.addDefine('_CRT_SECURE_NO_WARNINGS');
	addKoreDefine('DIRECT3D');
	addKoreDefine('DIRECT3D11');
	project.vsdeploy = true;

	if (vr === VrApi.HoloLens) {
		addKoreDefine('VR');
		addKoreDefine('HOLOLENS');
	}
	else if (vr === VrApi.None) {

	}
	else {
		throw new Error('VR API ' + vr + ' is not available for Windows Universal.');
	}
}
else if (platform === Platform.OSX) {
	addBackend('system/apple');
	addBackend('system/macos');
	addBackend('system/posix');
	if (graphics === GraphicsApi.Metal || graphics === GraphicsApi.Default) {
		addBackend('gpu/metal');
		addKoreDefine('METAL');
		project.addLib('Metal');
		project.addLib('QuartzCore');
	}
	else if (graphics === GraphicsApi.OpenGL) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		project.addLib('OpenGL');
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for macOS.');
	}
	project.addLib('IOKit');
	project.addLib('Cocoa');
	project.addLib('AppKit');
	project.addLib('CoreAudio');
	project.addLib('CoreData');
	project.addLib('CoreMedia');
	project.addLib('CoreVideo');
	project.addLib('AVFoundation');
	project.addLib('Foundation');
}
else if (platform === Platform.iOS || platform === Platform.tvOS) {
	if (platform === Platform.tvOS) {
		addKoreDefine('TVOS');
	}
	addBackend('system/apple');
	addBackend('system/ios');
	addBackend('system/posix');
	if (graphics === GraphicsApi.Metal || graphics === GraphicsApi.Default) {
		addBackend('gpu/metal');
		addKoreDefine('METAL');
		project.addLib('Metal');
	}
	else if (graphics === GraphicsApi.OpenGL) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		addKoreDefine('OPENGL_ES');
		project.addLib('OpenGLES');
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for iOS.');
	}
	project.addLib('UIKit');
	project.addLib('Foundation');
	project.addLib('CoreGraphics');
	project.addLib('QuartzCore');
	project.addLib('CoreAudio');
	project.addLib('AudioToolbox');
	project.addLib('CoreMotion');
	project.addLib('AVFoundation');
	project.addLib('CoreFoundation');
	project.addLib('CoreVideo');
	project.addLib('CoreMedia');
}
else if (platform === Platform.Android) {
	addKoreDefine('ANDROID');
	addBackend('system/android');
	addBackend('system/posix');
	if (graphics === GraphicsApi.Vulkan || graphics === GraphicsApi.Default) {
		addBackend('gpu/vulkan');
		addKoreDefine('VULKAN');
		project.addDefine('VK_USE_PLATFORM_ANDROID_KHR');
		project.addLib('vulkan');
		addKoreDefine('ANDROID_API=24');
	}
	else if (graphics === GraphicsApi.OpenGL) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		addKoreDefine('OPENGL_ES');
		addKoreDefine('ANDROID_API=19');
		addKoreDefine('EGL');
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for Android.');
	}
	project.addLib('log');
	project.addLib('android');
	project.addLib('EGL');
	project.addLib('GLESv3');
	project.addLib('OpenSLES');
	project.addLib('OpenMAXAL');
}
else if (platform === Platform.Emscripten) {
	addKoreDefine('EMSCRIPTEN');
	//project.addLib('websocket.js -sPROXY_POSIX_SOCKETS -sUSE_PTHREADS -sPROXY_TO_PTHREAD');
	addBackend('system/emscripten');
	project.addLinkerFlag('-sUSE_GLFW=2');
	if (graphics === GraphicsApi.WebGPU) {
		addBackend('gpu/webgpu');
		addKoreDefine('WEBGPU');

		project.addCFlag('--use-port=emdawnwebgpu');
		project.addCppFlag('--use-port=emdawnwebgpu');
		project.addLinkerFlag('--use-port=emdawnwebgpu');

		project.addLinkerFlag('-sASYNCIFY');
		project.addLinkerFlag('-sEXIT_RUNTIME');
	}
	else if (graphics === GraphicsApi.OpenGL || graphics === GraphicsApi.Default) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		addKoreDefine('OPENGL_ES');
		addKoreDefine('WEBGL');
		project.addLinkerFlag('-sUSE_WEBGL2=1');
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for Emscripten.');
	}
}
else if (platform === Platform.Wasm) {
	addKoreDefine('WASM');
	addBackend('system/wasm');
	project.addIncludeDir('miniclib');
	project.addFile('miniclib/**');
	if (graphics === GraphicsApi.WebGPU) {
		addBackend('gpu/webgpu');
		addKoreDefine('WEBGPU');
	}
	else if (graphics === GraphicsApi.OpenGL || graphics === GraphicsApi.Default) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		addKoreDefine('OPENGL_ES');
		addKoreDefine('WEBGL');
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for Wasm.');
	}
}
else if (platform === Platform.Linux || platform === Platform.FreeBSD) {
	if (platform === Platform.FreeBSD) { // global.h sets this for Linux
		addKoreDefine('LINUX');
	}
	addBackend('system/linux');
	addBackend('system/posix');
	project.addLib('asound');
	project.addLib('dl');

	if (platform === Platform.Linux) {
		project.addLib('udev');

		try {
			if (!fs.existsSync(targetDirectory)) {
				fs.mkdirSync(targetDirectory);
			}
			if (!fs.existsSync(path.join(targetDirectory, 'wayland'))) {
				fs.mkdirSync(path.join(targetDirectory, 'wayland'));
			}
			const waylandDir = path.join(targetDirectory, 'wayland', 'wayland-generated');
			if (!fs.existsSync(waylandDir)) {
				fs.mkdirSync(waylandDir);
			}

			const child_process = require('child_process');

			let good_wayland = false;

			const wayland_call = child_process.spawnSync('wayland-scanner', ['--version'], {encoding: 'utf-8'});
			if (wayland_call.status !== 0) {
				throw 'Could not run wayland-scanner to ask for its version';
			}
			const wayland_version = wayland_call.stderr;

			try {
				const scanner_versions = wayland_version.split(' ')[1].split('.');
				const w_x = parseInt(scanner_versions[0]);
				const w_y = parseInt(scanner_versions[1]);
				const w_z = parseInt(scanner_versions[2]);

				if (w_x > 1) {
					good_wayland = true;
				}
				else if (w_x === 1) {
					if (w_y > 17) {
						good_wayland = true;
					}
					else if (w_y === 17) {
						if (w_z >= 91) {
							good_wayland = true;
						}
					}
				}
			}
			catch (err) {
				log.error('Could not parse wayland-version ' + wayland_version);
			}

			let c_ending = '.c';
			if (good_wayland) {
				c_ending = '.c.h';
			}

			let chfiles = [];

			function wl_protocol(protocol, file) {
				chfiles.push(file);
				const backend_path = path.resolve(waylandDir);
				const protocol_path = path.resolve('/usr/share/wayland-protocols', protocol);
				if (child_process.spawnSync('wayland-scanner', ['private-code', protocol_path, path.resolve(backend_path, file + c_ending)]).status !== 0) {
					throw 'Failed to generate wayland protocol files for' + protocol;
				}
				if (child_process.spawnSync('wayland-scanner', ['client-header', protocol_path, path.resolve(backend_path, file + '.h')]).status !== 0) {
					throw 'Failed to generate wayland protocol header for' + protocol;
				}
			}

			if (child_process.spawnSync('wayland-scanner', ['private-code', '/usr/share/wayland/wayland.xml', path.resolve(waylandDir, 'wayland-protocol' + c_ending)]).status !== 0) {
				throw 'Failed to generate wayland protocol files for /usr/share/wayland/wayland.xml';
			}
			if (child_process.spawnSync('wayland-scanner', ['client-header', '/usr/share/wayland/wayland.xml', path.resolve(waylandDir, 'wayland-protocol.h')]).status !== 0) {
				throw 'Failed to generate wayland protocol header for /usr/share/wayland/wayland.xml';
			}
			wl_protocol('stable/viewporter/viewporter.xml', 'wayland-viewporter');
			wl_protocol('stable/xdg-shell/xdg-shell.xml', 'xdg-shell');
			wl_protocol('unstable/xdg-decoration/xdg-decoration-unstable-v1.xml', 'xdg-decoration');
			wl_protocol('unstable/tablet/tablet-unstable-v2.xml', 'wayland-tablet');
			wl_protocol('unstable/pointer-constraints/pointer-constraints-unstable-v1.xml', 'wayland-pointer-constraint');
			wl_protocol('unstable/relative-pointer/relative-pointer-unstable-v1.xml', 'wayland-relative-pointer');
			wl_protocol('staging/fractional-scale/fractional-scale-v1.xml', 'wayland-fractional-scale');

			if (good_wayland) {
				let cfile = '#include "wayland-protocol.c.h"\n';
				for (const chfile of chfiles) {
					cfile += '#include "' + chfile + '.c.h"\n';
				}
				fs.writeFileSync(path.resolve(waylandDir, 'waylandunit.c'), cfile);
			}

			project.addIncludeDir(path.join(targetDirectory, 'wayland'));
			project.addFile(path.resolve(waylandDir, '**'));
		}
		catch (err) {
			log.error('Failed to include wayland-support, setting KORE_NO_WAYLAND.');
			log.error('Wayland error was: ' + err);
			addKoreDefine('NO_WAYLAND');
		}
	}
	else if (platform === Platform.FreeBSD) {
		addBackend('system/linux');
		addBackend('system/posix');
		addBackend('system/freebsd');
		project.addKoreDefine('NO_WAYLAND');
	}

	if (graphics === GraphicsApi.Vulkan || (platform === Platform.Linux && graphics === GraphicsApi.Default)) {
		addBackend('gpu/vulkan');
		project.addLib('vulkan');
		addKoreDefine('VULKAN');
		project.addDefine('VK_USE_PLATFORM_WAYLAND_KHR');
		project.addDefine('VK_USE_PLATFORM_XLIB_KHR');
	}
	else if (graphics === GraphicsApi.OpenGL || (platform === Platform.FreeBSD && graphics === GraphicsApi.Default)) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		addKoreDefine('EGL');
		project.addLib('GL');
		project.addLib('EGL');
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for Linux.');
	}
	if (platform === Platform.FreeBSD) { // TODO
		addKoreDefine('POSIX');
	}
	project.addDefine('_POSIX_C_SOURCE=200112L');
	project.addDefine('_XOPEN_SOURCE=600');
}
else if (platform === Platform.Kompjuta) {
	addKoreDefine('KOMPJUTA');
	addBackend('system/kompjuta');
	addBackend('gpu/kompjuta');
	project.addIncludeDir('miniclib');
	project.addFile('miniclib/**');
}
else {
	plugin = true;
}

if (plugin) {
	let backend = 'Unknown';
	if (platform === Platform.PS4) {
		backend = 'ps4';
	}
	else if (platform === Platform.PS5) {
		backend = 'ps5'
	}
	else if (platform === Platform.XboxOne || platform === Platform.XboxSeries) {
		backend = 'xbox';
	}
	else if (platform === Platform.Switch) {
		backend = 'switch';
	}
	else if (platform === Platform.Switch2) {
		backend = 'switch2';
	}

	let ancestor = project;
	while (ancestor.parent != null) {
		ancestor = ancestor.parent;
	}

	let pluginPath = path.join(ancestor.basedir, 'backends', backend);
	if (!fs.existsSync(pluginPath)) {
		pluginPath = path.join(__dirname, '..', 'backends', backend);
	}
	if (!fs.existsSync(pluginPath)) {
		log.error('Was looking for a backend in ' + pluginPath + ' but it wasn\'t there.');
		throw 'backend not found';
	}
	await project.addProject(pluginPath);
	resolve(project);
}
else {
	resolve(project);
}
