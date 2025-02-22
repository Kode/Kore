const project = new Project('ShaderTest');

await project.addProject(findKore());

project.addFile('sources/**');
project.addFile('shaders/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
