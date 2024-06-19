package main

import (
	"context"
	"strconv"

	"github.com/NilFoundation/nil/common"
	"github.com/NilFoundation/nil/core/db"
	"github.com/NilFoundation/nil/core/execution"
	"github.com/NilFoundation/nil/core/types"
)

func CreateBlock(n types.BlockNumber) *types.Block {
	badger, err := db.NewBadgerDb("test.db")
	if err != nil {
		return nil
	}
	tx, err := badger.CreateRwTx(context.Background())
	if err != nil {
		return nil
	}
	es, err := execution.NewExecutionState(tx, types.MasterShardId, common.EmptyHash, common.NewTestTimer(0))
	if err != nil {
		return nil
	}

	m := &types.Message{Data: types.Code(strconv.Itoa(9))}
	es.AddInMessage(m)

	r := &types.Receipt{MsgHash: m.Hash()}
	es.AddReceipt(r)

	blockHash, err := es.Commit(n)
	if err != nil {
		return nil
	}

	execution.PostprocessBlock(tx, types.MasterShardId, blockHash)
	tx.Commit()
	return db.ReadBlock(tx, types.MasterShardId, blockHash)
	// badger.Close()
}

func main() {
	b := CreateBlock(1)
	if b == nil {
		println("Generation failed")
		return
	}
	println("Generation succeeded")
}
