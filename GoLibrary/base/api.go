package base

import (
	"io"

	"google.golang.org/protobuf/encoding/protojson"
	"google.golang.org/protobuf/proto"
)

type Api struct {
	Uid        string
	SessionKey string
	Req        proto.Message
}

func UnMarshal(body io.ReadCloser, req proto.Message) error {
	data, err := io.ReadAll(body)
	if err != nil {
		return err
	}

	err = protojson.Unmarshal(data, req)
	if err != nil {
		return err
	}

	return nil
}
