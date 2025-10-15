src = files(
  'main.cpp',
)

app_exe = executable(
  meson.project_name(),
  sources: [src],
)

test('general-test', app_exe, workdir: meson.project_build_root() + '/src/')
