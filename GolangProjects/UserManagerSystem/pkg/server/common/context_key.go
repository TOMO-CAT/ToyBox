package common

import "github.com/google/uuid"

type ContextKey int

// https://stackoverflow.com/questions/40891345/fix-should-not-use-basic-type-string-as-key-in-context-withvalue-golint
const (
	ContextKeyTraceID ContextKey = iota
	ContextKeyCaller
)

func NewTraceId() string {
	return uuid.New().String()
}
