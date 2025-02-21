const project = new Project('Kore');

await project.addProject('v2');

project.addFile('Sources/**');
project.addFile('Shaders/**');

project.addIncludeDir('Sources');

resolve(project);
