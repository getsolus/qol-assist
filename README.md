# qol-assist

[![License](https://img.shields.io/badge/License-Apache%202.0-lightgrey.svg)](https://www.apache.org/licenses/LICENSE-2.0.html)
![#solus-dev on Freenode](https://img.shields.io/badge/freenode-%23solus--dev-28C)

qol-assist is a quality of life assistant for rolling release Linux distributions, such as Solus.

During the lifetime of a rolling release Linux distribution, new problems occur that are often
complex to deal with. An example would be udev rules requiring the addition of new UNIX user groups
by udev rules (`setfacl`), and the requirement to automatically migrate active users to those
groups.

Traditionally, post-install packaging scripts have no knowledge of users, so `qol-assist`
bridges that gap by being a central location for rolling QoL operations to continue
providing a solid user experience while still being able to make deep changes to the OS.

`qol-assist` is a [Solus project.](https://getsol.us/)

![Solus logo](https://build.getsol.us/logo.png)

## Authors

Copyright © 2020-2021 Solus Project

`qol-assist` is available under the terms of the `Apache-2.0` license.
