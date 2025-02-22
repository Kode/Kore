let project = new Project('Input');

await project.addProject('../../');

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
