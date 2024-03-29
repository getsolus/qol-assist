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

package cli

import (
	"github.com/DataDrake/cli-ng/v2/cmd"
	"github.com/DataDrake/waterlog"
	"github.com/DataDrake/waterlog/level"
	"github.com/getsolus/qol-assist/core"
	"os"
)

var trigger = &cmd.Sub{
	Name:  "trigger",
	Short: "Schedule migration on next boot",
	Alias: "t",
	Run: func(root *cmd.Root, _ *cmd.Sub) {
		if gFlags := root.Flags.(*GlobalFlags); gFlags.Debug {
			waterlog.SetLevel(level.Debug)
		}

		if os.Geteuid() != 0 || os.Getegid() != 0 {
			waterlog.Fatalln("This command must be run with root privileges.")
		}

		if err := core.CreateTriggerFile(); err != nil {
			waterlog.Fatalf("Failed to create trigger file %s: %s\n", core.TriggerFile, err)
		}

		waterlog.Debugf("Created trigger file at %s\n", core.TriggerFile)
		waterlog.Println("Migration will run on next boot.")
	},
}
