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

package cli

import (
	"github.com/DataDrake/cli-ng/cmd"
	"github.com/DataDrake/waterlog"
	"github.com/DataDrake/waterlog/level"
	"github.com/getsolus/qol-assist/core"
	"strings"
)

var listUsers = &cmd.CMD{
	Name:  "list-users",
	Short: "List users on the system and their associated groups",
	Alias: "l",
	Args:  &ListArgs{},
	Run: func(root *cmd.RootCMD, command *cmd.CMD) {
		if gFlags := root.Flags.(*GlobalFlags); gFlags.Debug {
			waterlog.SetLevel(level.Debug)
		}

		var context, err = core.NewContext()
		if err != nil {
			waterlog.Errorf("Unable to gather system info: %s\n", err)
		}

		for _, it := range context.FilterUsers(command.Args.(*ListArgs).Filter) {
			waterlog.Printf("User: %s (%s)\n", it.Name, strings.Join(it.Groups, ":"))
		}
	},
}

// ListArgs contains the arguments for the "list-users" subcommand
type ListArgs struct {
	Filter string `desc:"[system|all|admin|active]"`
}
