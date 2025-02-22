const project = new Project('MultiWindow');

await project.addProject('../../');

project.addFile('sources/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
