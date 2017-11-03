# Write systemd unit
data_conf = configuration_data()
data_conf.set('QOL_TRIGGER_FILE', with_trigger_file)
data_conf.set('SBINDIR', path_sbindir)

configure_file(
    input: 'qol-assist-migration.service.in',
    output: 'qol-assist-migration.service',
    configuration: data_conf,
    install_dir: with_systemd_system_unit_dir,
)
