package util

import "encoding/json"

func ToString(src interface{}) string {
	if srcStr, ok := src.(string); ok {
		return srcStr
	}

	bytes, err := json.Marshal(src)
	if err != nil {
		return ""
	}
	return string(bytes)
}
