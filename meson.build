project(
  'Odtwarzacz',
  'cpp',
  version: '0.0.1',
  meson_version: '>=1.1',
  default_options: 'cpp_std=c++20',
)

qt6 = import('qt6')
qt_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Multimedia', 'Concurrent'])

taglib_dep = dependency('taglib')

qlementine_dep = dependency('qlementine')
qlementine_icons_dep = dependency('qlementine-icons')
tracy_dep = dependency('tracy', static: true)

include_dir = include_directories('include')

common_args = []

if get_option('tracy_enable')
  common_args += ['-DTRACY_ENABLE', '-DTRACY_NO_EXIT']
endif

cc = meson.get_compiler('cpp')

win_libs = []

if host_machine.system() == 'windows'
  win_libs += cc.find_library('ws2_32')
  win_libs += cc.find_library('dbghelp')
endif

subdir('resources')
subdir('src')
