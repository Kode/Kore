@pushd "%~dp0"
@if exist v2\get_dlc_dangerously (
@git submodule update --remote --merge v2
) else (
@git submodule update --init --remote v2
@git -C v2 checkout main
)
@call v2\get_dlc_dangerously.bat
@popd