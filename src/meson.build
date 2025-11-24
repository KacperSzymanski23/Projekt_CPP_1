src = files(
  'main.cpp',
  'mainwindow.cpp',
  'playercontrols.cpp',
  'track.cpp',
  'treeitem.cpp',
  'treemodel.cpp',
)

moc_headers = files(
  'mainwindow.hpp',
  'playercontrols.hpp',
  'treemodel.hpp',
)

qt_moc = qt6.compile_moc(headers: moc_headers, dependencies: qt_dep)

app_exe = executable(
  meson.project_name(),
  sources: [src, qt_moc, resources],
  dependencies: [qt_dep, taglib_dep, qlementine_dep, qlementine_icons_dep],
  include_directories : include_dir,
)

test('general-test', app_exe, workdir: meson.project_build_root() + '/src/')
