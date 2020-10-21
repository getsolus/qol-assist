// Copyright © 2020 Solus Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package core

// #include <pwd.h>
// #include <grp.h>
// #include <unistd.h>
import "C"

import (
	"fmt"
	log "github.com/DataDrake/waterlog"
	"os/exec"
	gouser "os/user"
	"strconv"
)

const minimumUID = 1000
const wheelGroup = "sudo"

// Context contains contextual system data, such as groups, users, and active shells, along with paths to certain binaries
type Context struct {
	usermod  string
	groupadd string
	groupmod string
	users    []User
	groups   []gouser.Group
	shells   []string
}

// User is a convenience struct with information on a user entry in /etc/passwd
type User struct {
	Name     string
	Groups   []string
	IsActive bool
	IsRoot   bool
	IsAdmin  bool
}

// NewContext creates an initialized instance of a Context object
func NewContext() (*Context, error) {
	return (&Context{}).init()
}

// FilterUsers returns users that fit into one or more of the given filters
func (c *Context) FilterUsers(filters ...string) []User {
	var filtered = make([]User, 0)

	for _, it := range c.users {
		switch {
		case contains(filters, "all"):
			fallthrough
		case contains(filters, "active") && it.IsActive:
			fallthrough
		case contains(filters, "system") && !it.IsActive:
			fallthrough
		case contains(filters, "admin") && (it.IsRoot || it.IsAdmin):
			filtered = append(filtered, it)
		}
	}

	return filtered
}

// AddToGroup adds a User to a preexisting group
func (c *Context) AddToGroup(user User, group string) (bool, error) {
	if !contains(user.Groups, group) {
		var cmd = &exec.Cmd{
			Path: c.usermod,
			Args: []string{c.usermod, "-aG", group, user.Name},
		}

		if err := cmd.Run(); err != nil {
			return false, err
		}

		user.Groups = append(user.Groups, group)
		return true, nil
	}
	return false, nil
}

// CreateGroup creates a new group with the given name and ID
func (c *Context) CreateGroup(name string, id string) error {
	var cmd = &exec.Cmd{
		Path: c.groupadd,
		Args: []string{c.groupadd, "-g", id, name},
	}

	if err := cmd.Run(); err != nil {
		return err
	}

	c.groups = append(c.groups, gouser.Group{
		Name: name,
		Gid:  id,
	})

	return nil
}

// UpdateGroupID finds a group with the given name and changes its ID to the given ID
func (c *Context) UpdateGroupID(name string, id string) error {
	var cmd = &exec.Cmd{
		Path: c.groupmod,
		Args: []string{c.groupmod, "-g", id, name},
	}

	if err := cmd.Run(); err != nil {
		return err
	}

	for _, it := range c.groups {
		if it.Name == name {
			it.Gid = id
			break
		}
	}

	return nil
}

func (c *Context) init() (*Context, error) {
	var err error

	log.Debugln("Gathering system info...")

	if c.usermod, err = exec.LookPath("usermod"); err != nil {
		return c, fmt.Errorf("usermod command could not be found in PATH")
	}

	if c.groupadd, err = exec.LookPath("groupadd"); err != nil {
		return c, fmt.Errorf("groupadd command could not be found in PATH")
	}

	if c.groupmod, err = exec.LookPath("groupmod"); err != nil {
		return c, fmt.Errorf("groupadd command could not be found in PATH")
	}

	c.shells = activeShells()
	log.Debugln("    Gathered active shells from /etc/shells")

	if err = c.populateGroups(); err != nil {
		return c, fmt.Errorf("failed to obtain groups from /etc/groups: %s", err)
	}
	log.Debugln("    Gathered groups from /etc/groups")

	if err = c.populateUsers(); err != nil {
		return c, fmt.Errorf("failed to obtain users from /etc/passwd: %s", err)
	}
	log.Debugln("    Gathered users from /etc/passwd")

	return c, nil
}

func (c *Context) populateGroups() error {
	c.groups = make([]gouser.Group, 0)

	C.setgrent()
	for {
		var gr = C.getgrent()
		if gr == nil {
			break
		}

		var group, err = gouser.LookupGroup(C.GoString(gr.gr_name))
		if err != nil {
			return err
		}

		c.groups = append(c.groups, *group)
	}
	C.endgrent()

	return nil
}

func (c *Context) populateUsers() error {
	c.users = make([]User, 0)
	var err error

	C.setpwent()
	for {
		var pw = C.getpwent()
		if pw == nil {
			break
		}

		var uid = int(pw.pw_uid)

		var it, err = gouser.LookupId(strconv.Itoa(uid))
		if err != nil {
			break
		}

		var groupIDStrings []string
		if groupIDStrings, err = it.GroupIds(); err != nil {
			break
		}

		var groupNames = c.groupNamesFromGUIDs(groupIDStrings)
		c.users = append(c.users, User{
			Name:     C.GoString(pw.pw_name),
			Groups:   groupNames,
			IsActive: uid >= minimumUID && contains(c.shells, C.GoString(pw.pw_shell)),
			IsRoot:   uid == 0 && int(pw.pw_gid) == 0,
			IsAdmin:  contains(groupNames, wheelGroup),
		})
	}
	C.endpwent()

	return err
}

func (c *Context) groupNamesFromGUIDs(guidStrings []string) []string {
	var groupNames = make([]string, 0, len(guidStrings))
	for _, group := range c.groups {
		if contains(guidStrings, group.Gid) {
			groupNames = append(groupNames, group.Name)
		}
	}
	return groupNames
}

func activeShells() []string {
	var shells = make([]string, 0)

	C.setusershell()
	for {
		var cShell = C.getusershell()
		if cShell == nil {
			break
		}
		shells = append(shells, C.GoString(cShell))
	}
	C.endusershell()

	return shells
}

func contains(list []string, item string) bool {
	for _, it := range list {
		if it == item {
			return true
		}
	}
	return false
}
