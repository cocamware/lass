from tools.build_tools_win import build_dll, WIN32

configurations = [
	("vc8", WIN32),
	("vc8_d", WIN32),
	("vc8_sse2", WIN32)
	]
build_dll(8.0, "src\\dll_vc8.sln", "src\\dll\\dll_vc8.vcproj", configurations)
