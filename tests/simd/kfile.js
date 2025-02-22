let project = new Project('SIMD');

await project.addProject('../../');

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
