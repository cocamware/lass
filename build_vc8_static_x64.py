from tools.build_tools_win import X64, build_static

configurations = [
	("vc8", X64),
	("vc8_d", X64)
	]
build_static(8.0, "src\\lass_vc8.sln", configurations)
