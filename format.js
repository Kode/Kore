const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

const excludes = [
	'Sources/Kore/Audio1/stb_vorbis.c',
	'Sources/Kore/Graphics1/stb_image.h',
	'Sources/Kore/IO/snappy/snappy-c.h',
	'Sources/Kore/IO/snappy/snappy-internal.h',
	'Sources/Kore/IO/snappy/snappy-sinksource.h',
	'Sources/Kore/IO/snappy/snappy-stubs-internal.h',
	'Sources/Kore/IO/snappy/snappy-stubs-public.h',
	'Sources/Kore/IO/snappy/snappy.h'
];

function isExcluded(filepath) {
	return excludes.indexOf(filepath.replace(/\\/g, '/')) >= 0
	|| filepath.indexOf('Kinc') >= 0
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
			if (file.endsWith('.cpp') || file.endsWith('.h') || file.endsWith('.m') || file.endsWith('.mm')) {
				console.log('Format ' + filepath);
				child_process.execFileSync('clang-format', ['-style=file', '-i', filepath]);
			}
		}
	}
}

format('.');
