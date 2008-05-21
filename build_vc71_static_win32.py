from tools.build_tools_win import WIN32, build_static

configurations = [
	("win32_vc71", WIN32),
	("win32_vc71_d", WIN32)
	]
build_static(7.1, "src\\lass.sln", configurations)
