@pushd "%~dp0"
@if exist tools\windows_x64\icon.png (
@git submodule update --remote --merge tools/windows_x64
) else (
@git submodule update --init --remote tools/windows_x64
@git -C tools/windows_x64 checkout main
)
@popd