qol-assist(1) -- Quality Of Life Assistance
===========================================

## SYNOPSIS

`qol-assist [command]`


## DESCRIPTION

`qol-assist` is a system level component to assist in migrations within an
operating system, with a specific focus on combatting changes specific to
so-called "rolling release" systems.

`qol-assist` is designed to be an unobtrusive component of the system update
architecture, running versioned system migrations (such as user group inclusion)
over time to keep existing users at feature parity with new installations.

## SUBCOMMANDS

`list-users [admin all active system]`

    Useful utility to gain insight into how the system views different user
    accounts. This command takes a single argument to control how the user
    list is filtered. This can be useful in debugging to determine which
    accounts the system deems to be administrators, system, or human.

`trigger`

    This subcommand is typically invoked through the update architecture when
    the `qol-assist` package itself is updated. Execution of this subcommand
    will cause a state tracking file to be created to schedule `qol-assist`
    to start up on the next boot during the early init process.

`migrate`

    This subcommand will only execute if a trigger file has been created
    through the aforementioned `trigger` subcommand. During early start up
    the system state and QoL level will be evaluated, so that any pending
    migrations from a higher level than the current system level will be
    executed. Care is taken to not blindly perform actions that are
    unnecessary, and the newly reached system level will be recorded. Future
    migrations will not run earlier versioned migrations.

`help`

    Print the supported command set for the `qol-assist(1)` binary.

`version`

    Print the version and license information, before quitting.

## FILES

`qol-assist` tracks state through some special files, and will recover in their
absence.


`/var/lib/qol-assist/trigger`

    If this file exists, `qol-assist migrate` will be started on the next boot during
    the early init process.

`/var/lib/qol-assist/status`

    This file describes the current migration level of the system, in order to
    prevent unnecessary work between subsequent runs of `qol-assist`. In effect
    this will mandate the current system migration level and any migrations from
    an older level will not be run, preventing unnecessary work and boot time
    regressions.

    The absence of this file is non fatal, all migrations will check the system
    state before applying new changes.
   

## EXIT STATUS

On success, 0 is returned. A non-zero return code signals a failure.


## COPYRIGHT

 * Copyright © 2017 Ikey Doherty, License: CC-BY-SA-3.0


## SEE ALSO

`usysconf(1)`

 * https://github.com/solus-project/qol-assist

## NOTES

Creative Commons Attribution-ShareAlike 3.0 Unported

 * http://creativecommons.org/licenses/by-sa/3.0/
