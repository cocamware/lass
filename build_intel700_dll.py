from tools.build_dll import build_dll

solution_file = 'src/dll.sln'
project_file = 'src/dll/dll.vcproj'
configurations = ['win32_intel700', 'win32_intel700_d']
comntools = 'vs71comntools'
build_dll(solution_file, project_file, configurations, comntools)
