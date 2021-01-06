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
	"github.com/DataDrake/cli-ng/cmd"
	log "github.com/DataDrake/waterlog"
	"github.com/DataDrake/waterlog/level"
	"github.com/getsolus/qol-assist/core"
	"os"
)

var migrate = &cmd.CMD{
	Name:  "migrate",
	Short: "Applies migrations that are available on the system",
	Alias: "m",
	Args:  &struct{}{},
	Run: func(root *cmd.RootCMD, _ *cmd.CMD) {
		if gFlags := root.Flags.(*GlobalFlags); gFlags.Debug {
			log.SetLevel(level.Debug)
		}

		if os.Geteuid() != 0 || os.Getegid() != 0 {
			log.Fatalln("This command must be run with root privileges.")
		}

		if !core.TriggerFileExists() {
			log.Fatalln("Refusing to run migration without trigger file.")
		}

		context, err := core.NewContext()
		if err != nil {
			log.Fatalf("Unable to gather system info: %s\n", err)
		}

		migrations := core.LoadMigrations()
		for _, it := range migrations {
			it.Run(context)
		}

		log.Println("Migrations complete.")

		if err := core.RemoveTriggerFile(); err != nil {
			log.Warnf("Failed to remove trigger file %s due to error: %s\n", core.TriggerFile, err)
		}
	},
}
