project(
    'qol-assist',
    ['c'],
    version: '0.0.2',
    license: [
        'GPL-2.0',
    ],
    default_options: [
        'c_std=c11',
        'prefix=/usr',
        'sysconfdir=/etc',
        'localstatedir=/var',
    ],
)

am_cflags = [
    '-fstack-protector',
    '-Wall',
    '-pedantic',
    '-Wstrict-prototypes',
    '-Wundef',
    '-fno-common',
    '-Werror-implicit-function-declaration',
    '-Wformat',
    '-Wformat-security',
    '-Werror=format-security',
    '-Wconversion',
    '-Wunused-variable',
    '-Wunreachable-code',
    '-W',
    '-D_FORTIFY_SOURCE=2',
]

# Add our main flags
add_global_arguments(am_cflags, language: 'c')

# Get configuration bits together
path_prefix = get_option('prefix')
path_sysconfdir = join_paths(path_prefix, get_option('sysconfdir'))
path_datadir = join_paths(path_prefix, get_option('datadir'))
path_sbindir = join_paths(path_prefix, get_option('sbindir'))
path_vardir = join_paths(path_prefix, get_option('localstatedir'), 'lib', meson.project_name())

# Systemd configuration
with_systemd_system_unit_dir = get_option('with-systemd-system-unit-dir')
if with_systemd_system_unit_dir == ''
    # Automatically discover systemd unit location from pkgconfig
    message('Asking pkg-config for systemd\'s system unit directory')
    dep_systemd = dependency('systemd')
    with_systemd_system_unit_dir = dep_systemd.get_pkgconfig_variable('systemdsystemunitdir')

    if with_systemd_system_unit_dir == ''
        error('Cannot locate the systemd system unit directory')
    endif
endif

with_static_binary = get_option('with-static-binary')

if with_static_binary == true
    message('WARNING: Do not build static binaries with glibc!')
endif

# Sort out config.h now
cdata = configuration_data()

# General options..
cdata.set_quoted('PACKAGE_NAME', meson.project_name())
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())
cdata.set_quoted('PACKAGE_URL', 'https://solus-project.com')
cdata.set_quoted('PACKAGE_BUG_TRACKER', get_option('with-bug-tracker'))

# Tracking files
cdata.set_quoted('QOL_TRACK_DIR', path_vardir)

with_trigger_file = join_paths(path_vardir, 'trigger')
with_status_file = join_paths(path_vardir, 'status')

cdata.set_quoted('QOL_STATUS_FILE', with_status_file)
cdata.set_quoted('QOL_TRIGGER_FILE', with_trigger_file)


# If the uid isn't provided as a number, the build will freak out.
with_minimum_uid = get_option('with-minimum-uid').to_int()
cdata.set('QOL_MIN_UID', with_minimum_uid)

# Grab the name for the sudo group (admins)
with_wheel_group = get_option('with-wheel-group')
cdata.set_quoted('QOL_WHEEL_GROUP', with_wheel_group)

# Write config.h now
config_h = configure_file(
     configuration: cdata,
     output: 'config.h',
)
config_h_dir = include_directories('.')

# Sort out systemd unit
subdir('data')

# Now go build the source
subdir('src')

report = [
    '    Build configuration:',
    '    ====================',
    '',
    '    prefix:                                 @0@'.format(path_prefix),
    '    datadir:                                @0@'.format(path_datadir),
    '    sysconfdir:                             @0@'.format(path_sysconfdir),
    '    sbindir:                                @0@'.format(path_sbindir),
    '    systemd unit directory:                 @0@'.format(with_systemd_system_unit_dir),
    '    build static binary:                    @0@'.format(with_static_binary),
    '',
    '    Migration tracking:',
    '    ===================',
    '',
    '    tracking directory:                     @0@'.format(path_vardir),
    '    trigger file:                           @0@'.format(with_trigger_file),
    '    status file:                            @0@'.format(with_status_file),
    '',
    '    User identification:',
    '    ====================',
    '',
    '    minimum UID for non system user:        @0@'.format(with_minimum_uid),
    '    wheel (sudo) group name:                @0@'.format(with_wheel_group),
]

# Output some stuff to validate the build config
message('\n\n\n' + '\n'.join(report) + '\n\n')
