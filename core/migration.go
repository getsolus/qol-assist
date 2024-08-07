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

import (
	"errors"
	"fmt"
	"io/ioutil"
	"os"
	gouser "os/user"
	"path/filepath"
	"strconv"

	"github.com/BurntSushi/toml"
	"github.com/DataDrake/waterlog"
)

// Migration contains the information about a migration, including where it is on the system and its requested system modifications
type Migration struct {
	Name string
	Path string

	Description string         `toml:"description"`
	AddSubIds   []*AddSubIds   `toml:"add-subids"`
	UpdateUsers []*UpdateUsers `toml:"users-update"`
	UpdateGroup []*UpdateGroup `toml:"group-update"`
	RemoveUsers []*RemoveUsers `toml:"users-remove"`
	RemoveGroup []*RemoveGroup `toml:"group-delete"`
	DeleteUsers []*DeleteUsers `toml:"users-delete"`
}

// AddSubIds is a type of modification that adds subuids and subgids to a specific set of users
type AddSubIds struct {
	GroupName  string `toml:"group"`
	RangeStart int    `toml:"range-start"`
	RangeEnd   int    `toml:"range-end"`
}

// UpdateUsers is a type of modification that adds a group to a specific set of users
type UpdateUsers struct {
	UserFilters []string `toml:"only"`
	GroupName   string   `toml:"group"`
}

// UpdateGroup is a type of modification that attempts to create or update a group to the given name and ID
type UpdateGroup struct {
	GroupName  string `toml:"name"`
	NewGroupID int    `toml:"id"`
}

// RemoveUsers is a type of modification that removes a group from a specific set of users
type RemoveUsers struct {
	UserFilters []string `toml:"only"`
	GroupName   string   `toml:"group"`
}

// RemoveGroup is a type of modification that attempts to delete a preexisting group from the given name
type RemoveGroup struct {
	GroupName string `toml:"name"`
}

// DeleteUser is a type of modification that attempts to delete a preexisting user from the system
type DeleteUsers struct {
	UserName string `toml:"name"`
}

// LoadMigrations finds migration files in SysDir and UsrDir and attempts to load them
func LoadMigrations() []Migration {
	var allMigrations []Migration

	if sysFiles, err := ioutil.ReadDir(SysDir); err != nil {
		waterlog.Warnf("System directory for migrations at %s is unreadable, skipping\n", SysDir)
	} else {
		waterlog.Debugf("Loading migrations from system directory %s...\n", SysDir)
		for _, it := range sysFiles {
			allMigrations = appendMigrationFrom(allMigrations, SysDir, it.Name())
		}
	}

	if usrFiles, err := ioutil.ReadDir(UsrDir); err != nil {
		waterlog.Warnf("User directory for migrations at %s is unreadable, skipping\n", UsrDir)
	} else {
		waterlog.Debugf("Loading migrations from user directory %s...\n", UsrDir)
		for _, it := range usrFiles {
			allMigrations = appendMigrationFrom(allMigrations, UsrDir, it.Name())
		}
	}

	return allMigrations
}

func appendMigrationFrom(migrations []Migration, dir string, name string) []Migration {
	if migration, err := parseMigration(dir, name); err != nil {
		waterlog.Warnf("\tFailed to parse migration %s: %s\n", name, err)
	} else {
		migrations = append(migrations, *migration)
		waterlog.Debugf("\tLoaded migration %s\n", name)
	}
	return migrations
}

func parseMigration(dir string, name string) (migration *Migration, err error) {
	// Read the configuration into the program
	path := filepath.Join(dir, name)
	var cfg []byte
	if cfg, err = readFile(path); err != nil {
		return migration, err
	}

	// Save the configuration into the content structure
	if err = toml.Unmarshal(cfg, &migration); err != nil {
		return migration, err
	}

	// Validate the migration so we don't load any invalid data
	if err := migration.Validate(); err == nil {
		migration.Name = name
		migration.Path = path
	}

	return migration, err
}

// Validate checks if a migration contains at least one modification
func (m *Migration) Validate() error {
	if len(m.UpdateUsers) == 0 && len(m.UpdateGroup) == 0 {
		return fmt.Errorf("migrations must contain at least one modification")
	}
	return nil
}

// Run applies the modifications contained in a migration
func (m *Migration) Run(context *Context) {
	waterlog.Debugf("Running migration %s...\n", m.Name)
	for _, task := range m.AddSubIds {
		m.addSubIds(context, task)
	}
	for _, task := range m.UpdateUsers {
		m.updateUsers(context, task)
	}
	for _, task := range m.UpdateGroup {
		m.updateGroup(context, task)
	}
	for _, task := range m.RemoveUsers {
		m.removeUsers(context, task)
	}
	for _, task := range m.RemoveGroup {
		m.removeGroup(context, task)
	}
	for _, task := range m.DeleteUsers {
		m.deleteUsers(context, task)
	}
}

func (m *Migration) addSubIds(context *Context, task *AddSubIds) {
	users := context.GetUsersInGroup(task.GroupName)
	users = append(users, context.GetRootUser())

	m.createSubUidFile(context, task, users)
	m.createSubGidFile(context, task, users)
}

func createFileIfNotExists(path string) (bool, error) {
	fh, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_EXCL, 0644)

	if err != nil {
		if errors.Is(err, os.ErrExist) {
			waterlog.Debugf("\t%s already exists, skipping", path)
			// The file already existing isn't an error mode for us,
			// so don't propagate the error.
			return false, nil
		}

		return false, err
	}

	fh.Close()

	return true, nil
}

func (m *Migration) createSubUidFile(context *Context, task *AddSubIds, users []User) {
	created, err := createFileIfNotExists("/etc/subuid")

	if !created {
		if err != nil {
			waterlog.Warnf("\tUnable to create /etc/subuid due to error: %s\n", err)
		}

		return
	}

	for _, user := range users {
		if err := context.AddSubUids(&user, task.RangeStart, task.RangeEnd); err != nil {
			waterlog.Warnf("\tFailed to add subuids to user %s due to error: %s\n", user.Name, err)
			return
		}

		waterlog.Debugf("\tSuccessfully added subuids to user %s\n", user.Name)
	}
}

func (m *Migration) createSubGidFile(context *Context, task *AddSubIds, users []User) {
	created, err := createFileIfNotExists("/etc/subgid")

	if !created {
		if err != nil {
			waterlog.Warnf("\tUnable to create /etc/subgid due to error: %s\n", err)
		}

		return
	}

	for _, user := range users {
		if err := context.AddSubGids(&user, task.RangeStart, task.RangeEnd); err != nil {
			waterlog.Warnf("\tFailed to add subgids to user %s due to error: %s\n", user.Name, err)
			return
		}

		waterlog.Debugf("\tSuccessfully added subgids to user %s\n", user.Name)
	}
}

func (m *Migration) updateUsers(context *Context, task *UpdateUsers) {
	filtered := context.FilterUsers(task.UserFilters...)

	for _, user := range filtered {
		user := user
		if ran, err := context.AddToGroup(&user, task.GroupName); err != nil {
			waterlog.Warnf("\tFailed to add group %s to user %s due to error: %s\n", task.GroupName, user.Name, err)
		} else if ran {
			waterlog.Debugf("\tSuccessfully added group %s to user %s\n", task.GroupName, user.Name)
		} else {
			waterlog.Debugf("\tUser %s already has group %s, skipping\n", user.Name, task.GroupName)
		}
	}
}

func (m *Migration) updateGroup(context *Context, task *UpdateGroup) {
	var byName *gouser.Group
	var byID *gouser.Group

	gid := strconv.Itoa(task.NewGroupID)
	for _, group := range context.groups {
		switch {
		case group.Name == task.GroupName:
			group := group
			byName = &group
		case group.Gid == strconv.Itoa(task.NewGroupID):
			group := group
			byID = &group
		}
	}

	if byName == nil && byID == nil {
		// group doesn't exist, create it
		if err := context.CreateGroup(task.GroupName, gid); err != nil {
			waterlog.Warnf("\tFailed to create group with name %s and GID %s due to error %s\n", task.GroupName, gid, err)
		} else {
			waterlog.Debugf("\tSuccessfully created group %s with GID %s\n", task.GroupName, gid)
		}
	} else if byName != nil && byID == nil {
		// group has wrong ID, fix it
		if err := context.UpdateGroupID(task.GroupName, gid); err != nil {
			waterlog.Warnf("\tFailed to update group with name %s to new GID %s due to error %s\n", task.GroupName, gid, err)
		} else {
			waterlog.Debugf("\tSuccessfully updated group with name %s to new GID %s\n", task.GroupName, gid)
		}
	} else if byName != byID {
		// there's a group with our desired ID, and it isn't supposed to have it. Fail.
		waterlog.Warnf("\tAnother group already exists with desired GID %s, skipping update for group %s\n", gid, task.GroupName)
	}
}

func (m *Migration) removeUsers(context *Context, task *RemoveUsers) {
	filtered := context.FilterUsers(task.UserFilters...)

	for _, user := range filtered {
		user := user
		if ran, err := context.RemoveFromGroup(&user, task.GroupName); err != nil {
			waterlog.Warnf("\tFailed to remove group %s to user %s due to error: %s\n", task.GroupName, user.Name, err)
		} else if ran {
			waterlog.Debugf("\tSuccessfully removed group %s from user %s\n", task.GroupName, user.Name)
		} else {
			waterlog.Debugf("\tUser %s is already not a part of group %s, skipping\n", user.Name, task.GroupName)
		}
	}
}

func (m *Migration) removeGroup(context *Context, task *RemoveGroup) {
	var byName *gouser.Group

	for _, group := range context.groups {
		if group.Name == task.GroupName {
			group := group
			byName = &group
			break
		}
	}

	// Group name matches
	if byName != nil {
		if err := context.DeleteGroup(task.GroupName); err != nil {
			waterlog.Warnf("\tFailed to remove group %s from system due to error %s\n", task.GroupName, err)
		} else {
			waterlog.Debugf("\tSuccessfully removed group %s from the system\n", task.GroupName)
		}
	} else if byName == nil {
		waterlog.Debugf("\tGroup %s doesn't exist, skipping\n", task.GroupName)
	}
}

func (m *Migration) deleteUsers(context *Context, task *DeleteUsers) {

	var byName *User

	for _, user := range context.users {
		if user.Name == task.UserName {
			user := user
			byName = &user
			if ran, err := context.DeleteUser(&user); err != nil {
				waterlog.Warnf("\tFailed to delete user %s from system due to error: %s\n", task.UserName, err)
			} else if ran {
				waterlog.Debugf("\tSuccessfully deleted user %s from the system\n", task.UserName)
			} else {
				waterlog.Warnf("\tUnknown error occured when deleting user %s from system, err %s\n", task.UserName, err)
			}
		}
	}

	if byName == nil {
		waterlog.Debugf("\tUser %s doesn't exist on the system, skipping\n", task.UserName)
	}
}
