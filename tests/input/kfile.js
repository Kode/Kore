let project = new Project('Input');

await project.addProject(findKore());

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
