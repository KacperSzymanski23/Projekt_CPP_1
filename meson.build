project(
  'Odtwarzacz',
  'cpp',
  version: '0.0.1',
)

qt6 = import('qt6')
qt_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Multimedia'])

taglib_dep = dependency('taglib')

qlementine_dep = dependency('qlementine')
qlementine_icons_dep = dependency('qlementine-icons')

include_dir = include_directories('include')

subdir('resources')
subdir('src')
