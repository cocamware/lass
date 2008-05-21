from tools.build_tools_win import build_dll, WIN32

configurations = [
	("win32_intel700", WIN32),
	("win32_intel700_d", WIN32)
	]
build_dll(7.1, "src\\dll.sln", "src\\dll\\dll.vcproj", configurations)
