@if exist "%~dp0v2\Tools\windows_x64\kmake.exe" (
	@call "%~dp0v2\Tools\windows_x64\kmake.exe" %*
) else (
	echo kmake was not found, please run the get_dlc script.
)
