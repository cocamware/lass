from tools.build_tools_win import WIN32, build_static

configurations = [
	("win32_intel700", WIN32),
	("win32_intel700_d", WIN32)
	]
build_static(7.1, "src\\lass.sln", configurations)
