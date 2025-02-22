const project = new Project('DisplayTest');

await project.addProject('../../');

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
