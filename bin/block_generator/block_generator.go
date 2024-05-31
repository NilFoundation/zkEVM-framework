package block_generator

import (
	"context"
	"log"
	"strconv"

	"github.com/NilFoundation/nil/common"
	//"github.com/NilFoundation/nil/common/hexutil"
	"github.com/NilFoundation/nil/core/db"
	"github.com/NilFoundation/nil/core/execution"
	"github.com/NilFoundation/nil/core/types"
	"github.com/NilFoundation/nil/msgpool"
	"github.com/NilFoundation/nil/rpc/transport"
	"github.com/NilFoundation/nil/rpc/transport/rpccfg"
	"github.com/NilFoundation/nil/rpc/jsonrpc"
)

/*
func CreateBlock() Block {
	suite.lastBlockHash = common.EmptyHash
	for i := range types.BlockNumber(2) {
		es, err := execution.NewExecutionState(tx, shardId, suite.lastBlockHash, common.NewTestTimer(0))
		suite.Require().NoError(err)

		for j := range int(i) {
			m := &types.Message{Data: types.Code(strconv.Itoa(j))}
			es.AddInMessage(m)

			r := &types.Receipt{MsgHash: m.Hash()}
			es.AddReceipt(r)
		}

		blockHash, err := es.Commit(i)
		suite.Require().NoError(err)
		suite.lastBlockHash = blockHash

		block, err := execution.PostprocessBlock(tx, shardId, blockHash)
		suite.Require().NotNil(block)
		suite.Require().NoError(err)
	}

	err = tx.Commit()
	suite.Require().NoError(err)

	pool := msgpool.New(msgpool.DefaultConfig)
	suite.Require().NotNil(pool)

	suite.api = NewEthAPI(ctx,
		NewBaseApi(rpccfg.DefaultEvmCallTimeout), suite.db, []msgpool.Pool{pool}, common.NewLogger("Test"))
}
*/

func main() {
	//var conig_file_name = "config.jon"
	shardId := types.MasterShardId
	ctx := context.Background()

	var _db db.DB
	var err error
	_db, err = db.NewBadgerDbInMemory()
	if err != nil {
		log.Println(err)
		return
	}

	tx, err := _db.CreateRwTx(ctx)
	if err != nil {
		log.Println(err)
		return
	}

	var lastBlockHash = common.EmptyHash

	for i := range types.BlockNumber(2) {
		es, err := execution.NewExecutionState(tx, shardId, lastBlockHash, common.NewTestTimer(0))
		if err != nil {
			log.Println(err)
			return
		}

		for j := range int(i) {
			m := &types.Message{Data: types.Code(strconv.Itoa(j))}
			es.AddInMessage(m)

			r := &types.Receipt{MsgHash: m.Hash()}
			es.AddReceipt(r)
		}

		blockHash, err := es.Commit(i)
		if err != nil {
			log.Println(err)
			return
		}

		lastBlockHash = blockHash

		block, err := execution.PostprocessBlock(tx, shardId, blockHash)
		if block == nil {
			log.Println("Empty block")
			return
		}
		if err != nil {
			log.Println(err)
			return
		}
	}

	err = tx.Commit()
	if err != nil {
		log.Println(err)
		return
	}

	pool := msgpool.New(msgpool.DefaultConfig)
	if pool == nil {
		log.Println("Empty message pool")
		return
	}

	api := jsonrpc.NewEthAPI(ctx,
		jsonrpc.NewBaseApi(rpccfg.DefaultEvmCallTimeout), _db, []msgpool.Pool{pool}, common.NewLogger("Test"))
	data, err := api.GetBlockByNumber(ctx, shardId, transport.BlockNumber(-5), false)
	if err != nil {
		log.Println(err)
		return
	}
	if data == nil {
		log.Println("can't get block by ID")
		return
	}
	log.Println(data["parentHash"])

	_db.Close()
}
