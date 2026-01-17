src = files(
  'library.cpp',
  'logs.cpp',
  'main.cpp',
  'mainwindow.cpp',
  'middletreemodel.cpp',
  'playbackqueue.cpp',
  'playercontrols.cpp',
  'playertreemodel.cpp',
  'settings.cpp',
  'settingsdialog.cpp',
  'sidebar.cpp',
  'treeitem.cpp',
  'treemodel.cpp',
)

moc_headers = files(
  'mainwindow.hpp',
  'middletreemodel.hpp',
  'playbackqueue.hpp',
  'playercontrols.hpp',
  'playertreemodel.hpp',
  'settingsdialog.hpp',
  'sidebar.h',
  'treemodel.hpp',
)

qt_moc = qt6.compile_moc(headers: moc_headers, dependencies: qt_dep)

app_exe = executable(
  meson.project_name(),
  sources: [src, qt_moc, resources],
  dependencies: [
    qt_dep,
    taglib_dep,
    qlementine_dep,
    qlementine_icons_dep,
    tracy_dep,
    win_libs,
  ],
  include_directories: include_dir,
  cpp_args: common_args,
)

test(
  'general-test',
  app_exe,
  workdir: meson.project_build_root() + '/src/',
  timeout: 0,
)
