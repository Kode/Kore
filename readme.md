## Kore 3

Kore is a low level toolkit for cross-platform game engine development and similar
endeavors. It is comparable to SDL but bigger in scope as it also takes care of
cross-platform GPU programming with multiple portable APIs to choose from and its
own shader-language [Kongruent](https://github.com/Kode/Kongruent).

Kore projects are built using kmake, a meta-build-tool that resides in
a git-submodule of Kore. In your project's directory call `path/to/Kore/make`,
this will create a project file for your IDE in a subdirectory called build.
kmake by default creates a project for the system you are currently using,
but you can also put one of windows, linux, android, windowsapp, osx, ios
and freebsd in the arguments list to create something else.
Depending on the capabilities of the target system you can also choose
your graphics api (-g direct3d11/direct3d12/opengl/vulkan/metal).
Console support is implemented via separate plugins because the code can not
be provided publicly - contact Robert for details if you are interested in it.

This in particular is Kore 3, the very latest and still slightly experimental
version of Kore that uses a new Graphics-API (essentially an expanded version
of WebGPU) and a new shader-language (very custom).
To get some insight into the current state of Kore 3, have a look at
https://github.com/Kode/Kore-Samples/actions - we have a growing list
of graphics samples which are checked on each revision across the supported APIs.

If you are looking for a more stable experience, visit the [v2-branch](https://github.com/Kode/Kore/tree/v2).

If you are looking for Kore's original C++-API, go to the [v1-branch](https://github.com/Kode/Kore/tree/v1).
