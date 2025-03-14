const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

const startExcludes = [
	'backends/gpu/direct3d12/sources/d3dx12.h',
	'tools'
];

const endExcludes = [
	'.winrt.cpp',
	'android_native_app_glue.h',
	'android_native_app_glue.c',
	'stb_image_write.h'
];

function excludeMatches(filepath) {
	for (const exclude of startExcludes) {
		if (filepath.startsWith(exclude)) {
			return true;
		}
	}

	for (const exclude of endExcludes) {
		if (filepath.endsWith(exclude)) {
			return true;
		}
	}

	return false;
}

function isExcluded(filepath) {
	filepath = filepath.replace(/\\/g, '/');
	return excludeMatches(filepath);
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
