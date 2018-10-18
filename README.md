# qol-assist

[![License](https://img.shields.io/badge/License-GPL%202.0-blue.svg)](https://opensource.org/licenses/GPL-2.0)

Quality of Life assistant for rolling release Linux distributions (i.e. Solus).

During the life time of a rolling release Linux distribution, new problems occur that are often
complex to deal with. An example would be the addition of new UNIX user groups being required
by udev rules (`setfacl`) - and the requirement to automatically migrate active users to those
groups.

Traditionally post-install packaging scripts have no knowledge of users, so `qol-assist`
will bridge that gap by being a central location for rolling QoL operations to continue
providing a solid user experience whilst still being able to make deep changes to the OS.

**Note**: This is a brand new project and is currently being built for a very specific Solus
need, but it will grow in time to be an agnostic project suitable outside of Solus.

`qol-assist` is a [Solus project](https://getsol.us/)

![logo](https://build.getsol.us/logo.png)

## Authors

Copyright © 2017-2018 Solus Project

`qol-assist` is available under the terms of the `GPL-2.0` license.
