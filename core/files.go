// Copyright 2021 Solus Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package core

import (
	"io/ioutil"
	"os"
	"path/filepath"
)

var (
	// TrackDir is an externally defined location used to store the TriggerFile
	TrackDir string
	// SysDir is an externally defined location wherein system migration files are stored
	SysDir string
	// UsrDir is an externally defined location wherein user migration files are stored
	UsrDir string
	// TriggerFile is the location of the trigger file, which must be present for migration to occur
	TriggerFile = TrackDir + "/trigger"
)

func createDir(path string) error {
	if _, err := os.Stat(path); os.IsNotExist(err) {
		if err := os.Mkdir(path, 0644); err != nil {
			return err
		}
	} else if err != nil {
		return err
	}
	return nil
}

func createFile(path string) error {
	if _, err := os.Stat(path); os.IsNotExist(err) {
		if _, err := os.Create(path); err != nil {
			return err
		}
	} else if err != nil {
		return err
	}
	return nil
}

func readFile(path string) ([]byte, error) {
	// Check if this is a valid file path
	if _, err := os.Stat(path); os.IsNotExist(err) {
		return []byte{}, err
	}

	// Read the file as bytes, returning both bytes and a possible error
	return ioutil.ReadFile(filepath.Clean(path))
}

// CreateTriggerFile creates a trigger file at TriggerFile, along with its parent directories if necessary
func CreateTriggerFile() error {
	if err := createDir(TrackDir); err != nil {
		return err
	}
	return createFile(TriggerFile)
}

// RemoveTriggerFile facilitates the removal of the file at TriggerFile
func RemoveTriggerFile() (err error) {
	if _, err = os.Stat(TriggerFile); err == nil {
		err = os.Remove(TriggerFile)
	}
	return err
}

// TriggerFileExists is a convenience function to determine if a file exists at TriggerFile
func TriggerFileExists() bool {
	_, err := os.Stat(TriggerFile)
	return err == nil
}
