from tools.build_dll import build_dll

solution_file = 'src/dll_vc8.sln'
project_file = 'src/dll/dll_vc8.vcproj'
configurations = ['win32_vc8','win32_vc8_d', 'win32_vc8_sse2']
comntools = 'vs80comntools'
build_dll(solution_file, project_file, configurations, comntools)