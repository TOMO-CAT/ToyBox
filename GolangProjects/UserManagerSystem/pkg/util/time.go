package util

import "time"

func MsSince(start time.Time) float64 {
	return time.Since(start).Seconds() * 1e3
}
