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
)

// VersionNumber is an externally defined string that contains qol-assist's current version
var VersionNumber string

var version = &cmd.Sub{
	Name:  "version",
	Short: "Print the qol-assist version and exit",
	Alias: "v",
	Run: func(_ *cmd.Root, _ *cmd.Sub) {
		waterlog.Printf("qol-assist version %v\n\nCopyright © 2017-2021 Solus Project\n", VersionNumber)
		waterlog.Println("Licensed under the Apache License, Version 2.0")
	},
}
