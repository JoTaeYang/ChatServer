package rdb

type IRdbPK[T any] interface {
	IModel

	GetPK() string
	SetPK(string)
}

type RdbPK[T any] struct {
	PK       string
	CreateAt int64
	UpdateAt int64
	ExpireAt int64
}

func (r *RdbPK[T]) GetPK() string {
	return r.PK
}

func (r *RdbPK[T]) SetPK(pk string) {
	r.PK = pk
}
