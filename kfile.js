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
	project.addFile('sources/' + name + '/*unit.m');
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

function addBackend(name) {
	project.addIncludeDir('backends/' + name + '/includes');
	project.addFile('backends/' + name + '/includes/**');
	project.addFile('backends/' + name + '/sources/*', {nocompile: true});
	project.addFile('backends/' + name + '/sources/*unit.c*');
	project.addFile('backends/' + name + '/sources/*unit.m');
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

	const directshow = (graphics !== GraphicsApi.Direct3D12 && graphics !== GraphicsApi.Vulkan && graphics !== GraphicsApi.Default);
	if (directshow) {
		project.addFile('backends/system/windows/libraries/directshow/**');
		project.addIncludeDir('backends/system/windows/libraries/directshow/BaseClasses');
		project.addLib('strmiids');
		project.addLib('winmm');
	}
	else {
		project.addDefine('KORE_NO_DIRECTSHOW');
	}

	project.addLib('wbemuuid');

	if (graphics === GraphicsApi.OpenGL) {
		addBackend('gpu/opengl');
		addKoreDefine('OPENGL');
		project.addDefine('GLEW_STATIC');
		project.addLib('opengl32');
		project.addLib('glu32');
	}
	else if (graphics === GraphicsApi.Direct3D11) {
		addBackend('gpu/direct3d11');
		addKoreDefine('DIRECT3D');
		addKoreDefine('DIRECT3D11');
		project.addLib('d3d11');
	}
	else if (graphics === GraphicsApi.Direct3D12 || graphics === GraphicsApi.Default) {
		addBackend('gpu/direct3d12');
		addKoreDefine('DIRECT3D');
		addKoreDefine('DIRECT3D12');
		project.addLib('dxgi');
		project.addLib('d3d12');
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
		project.addLibFor('x64', 'backends/system/windows/libraries/oculus/LibOVR/Lib/Windows/x64/Release/VS2017/LibOVR');
		project.addLibFor('Win32', 'backends/system/windows/libraries/oculus/LibOVR/Lib/Windows/Win32/Release/VS2017/LibOVR');
		project.addFile('backends/system/windows/libraries/oculus/LibOVRKernel/Src/GL/**');
		project.addIncludeDir('backends/system/windows/libraries/oculus/LibOVR/Include/');
		project.addIncludeDir('backends/system/windows/libraries/oculus/LibOVRKernel/Src/');
	}
	else if (vr === VrApi.SteamVR) {
		addKoreDefine('VR');
		addKoreDefine('STEAMVR');
		project.addDefine('VR_API_PUBLIC');
		project.addFile('backends/system/windows/libraries/steamvr/src/**');
		project.addIncludeDir('backends/system/windows/libraries/steamvr/src');
		project.addIncludeDir('backends/system/windows/libraries/steamvr/src/vrcommon');
		project.addIncludeDir('backends/system/windows/libraries/steamvr/headers');
	}
	else if (vr === VrApi.None) {

	}
	else {
		throw new Error('VR API ' + vr + ' is not available for Windows.');
	}

	if (Options.pix) {
		project.addDefine('KORE_PIX');
		project.addIncludeDir('backends/gpu/direct3d12/pix/Include');
		project.addLib('backends/gpu/direct3d12/pix/bin/x64/WinPixEventRuntime');
	}
}
else if (platform === Platform.WindowsApp) {
	addKoreDefine('WINDOWSAPP');
	addBackend('system/windowsapp');
	addBackend('system/microsoft');
	addBackend('gpu/direct3d11');
	addSimpleBackend('audio/wasapi_winrt');
	project.addDefine('_CRT_SECURE_NO_WARNINGS');
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
		project.addLib('MetalKit');
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
		project.addDefine('KORE_EGL');
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
	project.addLib('USE_GLFW=2');
	if (graphics === GraphicsApi.WebGPU) {
		addBackend('gpu/webgpu');
		addKoreDefine('WEBGPU');
	}
	else if (graphics === GraphicsApi.OpenGL || graphics === GraphicsApi.Default) {
		addBackend('gpu/opengl');
		project.addExclude('backends/Graphics4/opengl/sources/GL/**');
		addKoreDefine('OPENGL');
		addKoreDefine('OPENGL_ES');
		project.addLib('USE_WEBGL2=1');
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
		project.addExclude('backends/Graphics4/opengl/sources/GL/**');
		addKoreDefine('OPENGL');
		addKoreDefine('OPENGL_ES');
	}
	else {
		throw new Error('Graphics API ' + graphics + ' is not available for Wasm.');
	}
}
else if (platform === Platform.Linux || platform === Platform.FreeBSD) {
	if (platform === Platform.FreeBSD) { // TODO
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
			project.addDefine('KORE_NO_WAYLAND');
		}
	}
	else if (platform === Platform.FreeBSD) {
		addBackend('system/linux');
		addBackend('system/posix');
		addBackend('system/freebsd');
		project.addDefine('KORE_NO_WAYLAND');
	}

	if (graphics === GraphicsApi.Vulkan || (platform === Platform.Linux && graphics === GraphicsApi.Default)) {
		addBackend('gpu/vulkan');
		project.addLib('vulkan');
		addKoreDefine('VULKAN');
	}
	else if (graphics === GraphicsApi.OpenGL || (platform === Platform.FreeBSD && graphics === GraphicsApi.Default)) {
		addBackend('gpu/opengl');
		project.addExclude('backends/Graphics4/opengl/sources/GL/glew.c');
		project.addLib('GL');
		addKoreDefine('OPENGL');
		project.addLib('EGL');
		project.addDefine('KORE_EGL');
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
else if (platform === Platform.Pi) {
	addKoreDefine('RASPBERRY_PI');
	addBackend('system/pi');
	addBackend('system/posix');
	addBackend('gpu/opengl');
	project.addExclude('backends/Graphics4/opengl/sources/GL/**');
	addKoreDefine('OPENGL');
	addKoreDefine('OPENGL_ES');
	addKoreDefine('POSIX');
	project.addDefine('_POSIX_C_SOURCE=200112L');
	project.addDefine('_XOPEN_SOURCE=600');
	project.addIncludeDir('/opt/vc/include');
	project.addIncludeDir('/opt/vc/include/interface/vcos/pthreads');
	project.addIncludeDir('/opt/vc/include/interface/vmcs_host/linux');
	project.addLib('dl');
	project.addLib('GLESv2');
	project.addLib('EGL');
	project.addLib('bcm_host');
	project.addLib('asound');
	project.addLib('X11');
}
else {
	plugin = true;
}

if (plugin) {
	let backend = 'Unknown';
	if (platform === Platform.PS4) {
		backend = 'PlayStation4';
	}
	else if (platform === Platform.XboxOne) {
		backend = 'Xbox';
	}
	else if (platform === Platform.Switch) {
		backend = 'Switch';
	}
	else if (platform === Platform.XboxSeries) {
		backend = 'Xbox';
	}
	else if (platform === Platform.PS5) {
		backend = 'PlayStation5'
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
