@pushd "%~dp0"
@if exist Kinc\get_dlc_dangerously (
@git submodule update --remote --merge Kinc
) else (
@git submodule update --init --remote Kinc
@git -C Kinc checkout main
)
@call Kinc\get_dlc_dangerously.bat
@popd