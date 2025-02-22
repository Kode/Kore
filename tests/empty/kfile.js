let project = new Project('Empty');

await project.addProject('../../');

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
