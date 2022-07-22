const project = new Project('Kore');

await project.addProject('Kinc');

project.addFile('Sources/**');
project.addFile('Shaders/**');

project.addIncludeDir('Sources');

resolve(project);
