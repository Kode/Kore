const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

const excludes = [
	'backends/Graphics4/OpenGL/Sources/GL',
	'backends/gpu/direct3d12/sources/d3dx12.h',
	'backends/gpu/direct3d12/pix',
	'backends/system/android/sources/Android',
	'backends/system/linux/sources/kinc/backend/wayland',
	'includes/kore3/util/offalloc',
	'sources/util/offalloc',
	'tests/shader/sources/stb_image_write.h'
];

function excludeMatches(filepath) {
	for (const exclude of excludes) {
		if (filepath.startsWith(exclude)) {
			return true;
		}
	}
	return false;
}

function isExcluded(filepath) {
	filepath = filepath.replace(/\\/g, '/');
	return excludeMatches(filepath)
	|| filepath.indexOf('Libraries') >= 0
	|| filepath.indexOf('lz4') >= 0
	|| filepath.indexOf('Tools') >= 0
	|| filepath.indexOf('libs') >= 0
	|| filepath.endsWith('.winrt.cpp');
}

function format(dir) {
	const files = fs.readdirSync(dir);
	for (let file of files) {
		if (file.startsWith('.')) continue;
		let filepath = path.join(dir, file);
		let info = fs.statSync(filepath);
		if (info.isDirectory()) {
			format(filepath);
		}
		else {
			if (isExcluded(filepath)) continue;
			if (file.endsWith('.c') || file.endsWith('.cpp') || file.endsWith('.h') || file.endsWith('.m') || file.endsWith('.mm')) {
				console.log('Format ' + filepath);
				child_process.execFileSync('clang-format', ['-style=file', '-i', filepath]);
			}
		}
	}
}

format('.');
