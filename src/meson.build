qol_assist_sources = [
    'cli/main.c',
    'cli/migrate.c',
    'cli/users.c',
    'migrate.c',
    'user-manager.c',
    'util.c',
]

# Allow clean definition of migrations

migrations = [
    '01_scanner_group',
]

# Insert into sources
foreach migration : migrations
    qol_assist_sources += 'migrations/@0@.c'.format(migration)
endforeach

# Build main qol-assist binary
qol_assist = executable(
    'qol-assist',
    sources: qol_assist_sources,
    install: true,
    install_dir: path_sbindir,
    include_directories: [
        config_h_dir,
    ],
)
