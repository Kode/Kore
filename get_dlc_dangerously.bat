@pushd "%~dp0"
@git submodule update --init --remote Kinc
@call Kinc\get_dlc_dangerously.bat
@popd