qol_assist_sources = [
    'cli/main.c',
    'cli/migrate.c',
    'cli/trigger.c',
    'cli/users.c',
    'migrations/shared.c',
    'migrate.c',
    'user-manager.c',
    'util.c',
]

# Set static if requested (defaults to being enabled)
qol_link_args = []
if with_static_binary == true
    qol_link_args += '-static'
endif

# Allow clean definition of migrations

migrations = [
    '01_scanner_group',
    '02_plugdev_group',
    '03_users_group_gid',
    '04_users_group_join',
    '05_fuse_group',
]

# Insert into sources
foreach migration : migrations
    qol_assist_sources += 'migrations/@0@.c'.format(migration)
endforeach

# Build main qol-assist binary
qol_assist = executable(
    'qol-assist',
    sources: qol_assist_sources,
    link_args: qol_link_args,
    install: true,
    install_dir: path_sbindir,
    include_directories: [
        config_h_dir,
    ],
)
