package app

import (
	"fmt"
)

var (
	Version   string
	Commit    string
	Branch    string
	BuildTime string
	Builder   string
	GoVersion string
	ExtraInfo string
)

var _buildInfoTemplate = `
Version:     %s
Commit:      %s
Branch:      %s
BuildTime:   %s
Builder:     %s
GoVersion:   %s
ExtraInfo:   %s
`

func printBuildInfo() {
	fmt.Printf(_buildInfoTemplate, Version, Commit, Branch, BuildTime, Builder, GoVersion, ExtraInfo)
}
