project('catch2-ex', 'cpp', version: '1.0', default_options: ['cpp_std=c++17'])

catch2_dep = subproject('catch2').get_variable('catch2_dep')

executable('catch-ex-test', ['catch_ex_tests.cpp', 'catch_ex.cpp'], dependencies: catch2_dep)

libcatch2_ex = static_library('catch2_ex', ['catch_ex.cpp'],
    dependencies: [ catch2_dep ]
)

catch2_ex_dep = declare_dependency(
    include_directories: '.', link_with: libcatch2_ex
)
