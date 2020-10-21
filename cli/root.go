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
	"github.com/DataDrake/waterlog/format"
	"github.com/DataDrake/waterlog/level"
	"log"
)

// GlobalFlags contains the flags for the Root command
type GlobalFlags struct {
	Debug bool `short:"d" long:"debug"  desc:"Run in debug mode"`
}

// Root is the main command for this application
var Root = &cmd.RootCMD{
	Name:  "qol-assist",
	Short: "QoL assistance to help Solus roll!",
	Flags: &GlobalFlags{},
}

func init() {
	Root.RegisterCMD(&cmd.Help)
	Root.RegisterCMD(trigger)
	Root.RegisterCMD(version)
	Root.RegisterCMD(migrate)
	Root.RegisterCMD(listUsers)

	waterlog.SetLevel(level.Info)
	waterlog.SetFormat(format.Min)
	waterlog.SetFlags(log.Ltime | log.Ldate | log.LUTC)
}
