qol_assist_sources = [
    'main.c',
    'migrate.c',
    'user-manager.c',
    'util.c',
]

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
