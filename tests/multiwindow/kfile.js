const project = new Project('MultiWindow');

await project.addProject(findKore());

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
