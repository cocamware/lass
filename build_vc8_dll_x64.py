from tools.build_tools_win import build_dll, X64

configurations = [
	("vc8", X64),
	("vc8_d", X64)
	]
build_dll(8.0, "src\\dll_vc8.sln", "src\\dll\\dll_vc8.vcproj", configurations)
