from tools.build_tools_win import WIN32, build_static

configurations = [
	("vc8", WIN32),
	("vc8_d", WIN32)
	]
build_static(8.0, "src\\lass_vc8.sln", configurations)
