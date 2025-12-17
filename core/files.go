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
	"bufio"
	"io/ioutil"
	"os"
	"path/filepath"
	"strconv"
	"strings"
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
		if err := os.Mkdir(path, 0750); err != nil {
			return err
		}
	} else if err != nil {
		return err
	}
	return nil
}

func createFile(path string) error {
	if _, err := os.Stat(path); os.IsNotExist(err) {
		if _, err := os.Create(filepath.Clean(path)); err != nil {
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

// ReadLoginDefs reads the login.defs file and returns the values of the specified keys.
func ReadLoginDefs(keys []string) (map[string]int, error) {
	path := "/etc/login.defs"
	if _, err := os.Stat(path); os.IsNotExist(err) {
		path = "/usr/share/defaults/etc/login.defs"
		if _, err := os.Stat(path); os.IsNotExist(err) {
			return nil, err
		}
	}

	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	vals := make(map[string]int)
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if strings.HasPrefix(line, "#") {
			continue
		}
		parts := strings.Fields(line)
		if len(parts) != 2 {
			continue
		}
		for _, key := range keys {
			if parts[0] == key {
				val, err := strconv.Atoi(parts[1])
				if err != nil {
					return nil, err
				}
				vals[key] = val
			}
		}
	}

	if err := scanner.Err(); err != nil {
		return nil, err
	}

	return vals, nil
}
