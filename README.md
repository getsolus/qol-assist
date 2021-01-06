# qol-assist

[![License](https://img.shields.io/badge/License-Apache%202.0-lightgrey.svg)](https://www.apache.org/licenses/LICENSE-2.0.html)
![#solus-dev on Freenode](https://img.shields.io/badge/freenode-%23solus--dev-28C)

`qol-assist` is a quality of life assistant for rolling release Linux distributions.

During the lifetime of a rolling release Linux distribution, new problems occur that are often complex to deal with. An
example would be udev rules requiring the addition of new UNIX user groups by udev rules (`setfacl`), and the
requirement to automatically migrate active users to those groups.

Traditionally, post-install packaging scripts have no knowledge of users, so `qol-assist`
bridges that gap by being a central location for rolling QoL operations to continue providing a solid user experience
while still being able to make deep changes to the OS.

`qol-assist` is a [Solus project.](https://getsol.us/)

![Solus logo](https://build.getsol.us/logo.png)

## Building

Install the dependencies:

- go (>=1.15)

Then compile:

```
$ make
```

If desired, you may set the configuration and tracking directories at compile time:

```
$ make PREFIX=/usr BINDIR=/usr/bin MANDIR=/usr/man SYSDIR=/etc/dir USRDIR=/usr/dir TRACKDIR=/var/lib/dir SYSTEMDUNITDIR=/etc/systemd/system
```

## Installation

```
# make install PREFIX=/usr
```

## Running

```
$ qol-assist list-users all
# qol-assist migrate
# qol-assist trigger
```

## License

Copyright 2020-2021 Solus Project <copyright@getsol.us>

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the
License. You may obtain a copy of the License at

<https://www.apache.org/licenses/LICENSE-2.0>

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "
AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
language governing permissions and limitations under the License.
