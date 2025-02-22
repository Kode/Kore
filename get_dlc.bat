@pushd "%~dp0"
@git submodule update --depth 1 --init tools/windows_x64
@popd