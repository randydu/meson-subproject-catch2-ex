project('catch2-ex', 'cpp', version: '1.0.4', default_options: ['cpp_std=c++17'])

if host_machine.system() == 'windows'
    add_project_arguments(['-DDO_NOT_USE_WMAIN'], language: 'cpp') 
endif

catch2_dep = dependency('catch2')

libcatch2_ex = static_library('catch2_ex', ['catch_ex.cpp'], dependencies: [catch2_dep])

catch2_ex_dep = declare_dependency(include_directories: '.', link_with: libcatch2_ex, dependencies: [catch2_dep])

# tests
catch_ex_test = executable('catch-ex-test', ['catch_ex_tests.cpp', ], dependencies: [catch2_dep, catch2_ex_dep])
test('catch_ex_test', catch_ex_test)
