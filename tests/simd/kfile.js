let project = new Project('SIMD');

await project.addProject(findKore());

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
