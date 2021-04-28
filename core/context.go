// Copyright 2021 Solus Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
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
	"github.com/DataDrake/waterlog"
	"os/exec"
	gouser "os/user"
	"strconv"
)

const minimumUID = 1000
const wheelGroup = "sudo"

// Context contains contextual system data, such as groups, users, and active shells, along with paths to certain binaries
type Context struct {
	users  []User
	groups []gouser.Group
	shells []string
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
func NewContext() (ctx *Context, err error) {
	ctx = &Context{}
	waterlog.Debugln("Gathering system info...")

	ctx.shells = activeShells()
	waterlog.Debugln("\tGathered active shells from /etc/shells")

	if err = ctx.populateGroups(); err != nil {
		return ctx, fmt.Errorf("failed to obtain groups from /etc/groups: %s", err)
	}
	waterlog.Debugln("\tGathered groups from /etc/groups")

	if err = ctx.populateUsers(); err != nil {
		return ctx, fmt.Errorf("failed to obtain users from /etc/passwd: %s", err)
	}
	waterlog.Debugln("\tGathered users from /etc/passwd")

	return ctx, err
}

// FilterUsers returns users that fit into one or more of the given filters
func (c *Context) FilterUsers(filters ...string) (filtered []User) {
	for _, it := range c.users {
		switch {
		case contains(filters, "all"):
			fallthrough
		case it.IsActive && contains(filters, "active"):
			fallthrough
		case !it.IsActive && contains(filters, "system"):
			fallthrough
		case (it.IsRoot || it.IsAdmin) && contains(filters, "admin"):
			filtered = append(filtered, it)
		}
	}

	return filtered
}

// AddToGroup adds a User to a preexisting group. Returns whether or not the modification ran, along with an error in
// case something went wrong
func (c *Context) AddToGroup(user *User, group string) (ran bool, err error) {
	if !contains(user.Groups, group) {
		cmd := exec.Command("usermod", "-aG", group, user.Name)
		if err = cmd.Run(); err != nil {
			return false, err
		}

		user.Groups = append(user.Groups, group)
		ran = true
	}
	return ran, err
}

// CreateGroup creates a new group with the given name and ID
func (c *Context) CreateGroup(name string, id string) error {
	cmd := exec.Command("groupadd", "-g", id, name)
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
	cmd := exec.Command("groupmod", "-g", id, name)
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

func (c *Context) populateGroups() (err error) {
	c.groups = make([]gouser.Group, 0)

	C.setgrent()
	for {
		gr := C.getgrent()
		if gr == nil {
			break
		}

		group, err := gouser.LookupGroup(C.GoString(gr.gr_name))
		if err != nil {
			return err
		}

		c.groups = append(c.groups, *group)
	}
	C.endgrent()

	return err
}

func (c *Context) populateUsers() error {
	C.setpwent()
	for {
		pw := C.getpwent()
		if pw == nil {
			break
		}

		uid := int(pw.pw_uid)
		it, err := gouser.LookupId(strconv.Itoa(uid))
		if err != nil {
			C.endpwent()
			return err
		}

		var groupIDStrings []string
		if groupIDStrings, err = it.GroupIds(); err != nil {
			C.endpwent()
			return err
		}

		groupNames := c.groupNamesFromGUIDs(groupIDStrings)
		c.users = append(c.users, User{
			Name:     C.GoString(pw.pw_name),
			Groups:   groupNames,
			IsActive: uid >= minimumUID && contains(c.shells, C.GoString(pw.pw_shell)),
			IsRoot:   uid == 0 && int(pw.pw_gid) == 0,
			IsAdmin:  contains(groupNames, wheelGroup),
		})
	}
	C.endpwent()

	return nil
}

func (c *Context) groupNamesFromGUIDs(guidStrings []string) (groupNames []string) {
	for _, group := range c.groups {
		if contains(guidStrings, group.Gid) {
			groupNames = append(groupNames, group.Name)
		}
	}
	return groupNames
}

func activeShells() (shells []string) {
	C.setusershell()
	for {
		cShell := C.getusershell()
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
