const project = new Project('Shader-G5');

await project.addProject('../../');

project.addFile('sources/**');
project.addFile('shaders/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
