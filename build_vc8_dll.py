from tools.build_dll import build_dll

solution_file = 'src/dll_vc8.sln'
project_file = 'src/dll/dll_vc8.vcproj'
configurations = ['"vc8|Win32"','"vc8_d|Win32"', '"vc8_sse2|Win32"']
comntools = 'vs80comntools'
build_dll(solution_file, project_file, configurations, comntools)
