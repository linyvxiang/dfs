// Copyright (c) 2016, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef  BFS_BLOCK_MAPPING_MANAGER_IMPL_H_
#define  BFS_BLOCK_MAPPING_MANAGER_IMPL_H_

#include <vector>

#include <common/thread_pool.h>
#include <common/mutex.h>
#include "proto/blockmapping.pb.h"
#include "rpc/rpc_client.h"


namespace baidu {
namespace bfs {

class BlockMapping;

class BlockMappingManagerImpl : public BlockMappingManager {
public:
    BlockMappingManagerImpl();
    ~BlockMappingManagerImpl();
    void GetNewBlockId(::google::protobuf::RpcController* controller,
                       const BlockMappingGetNewBlockIdRequest* request,
                       BlockMappingGetNewBlockIdResponse* response,
                       ::google::protobuf::Closure* done);
    void GetLocatedBlock(::google::protobuf::RpcController* controller,
                       const BlockMappingGetLocatedBlockRequest* request,
                       BlockMappingGetLocatedBlockResponse* response,
                       ::google::protobuf::Closure* done);
    void ChangeReplicaNum(::google::protobuf::RpcController* controller,
                       const BlockMappingChangeReplicaNumRequest* request,
                       BlockMappingChangeReplicaNumResponse* response,
                       ::google::protobuf::Closure* done);
    void AddNewBlock(::google::protobuf::RpcController* controller,
                       const BlockMappingAddNewBlockRequest* request,
                       BlockMappingAddNewBlockResponse* response,
                       ::google::protobuf::Closure* done);
    void UpdateBlockInfo(::google::protobuf::RpcController* controller,
                       const BlockMappingUpdateBlockInfoRequest* request,
                       BlockMappingUpdateBlockInfoResponse* response,
                       ::google::protobuf::Closure* done);
    void RemoveBlocksForFile(::google::protobuf::RpcController* controller,
                       const BlockMappingRemoveBlocksForFileRequest* request,
                       BlockMappingRemoveBlocksForFileResponse* response,
                       ::google::protobuf::Closure* done);
    void RemoveBlock(::google::protobuf::RpcController* controller,
                       const BlockMappingRemoveBlockRequest* request,
                       BlockMappingRemoveBlockResponse* response,
                       ::google::protobuf::Closure* done);
    void DealWithDeadNode(::google::protobuf::RpcController* controller,
                       const BlockMappingDealWithDeadNodeRequest* request,
                       BlockMappingDealWithDeadNodeResponse* response,
                       ::google::protobuf::Closure* done);
    void CheckBlockVersion(::google::protobuf::RpcController* controller,
                       const BlockMappingCheckBlockVersionRequest* request,
                       BlockMappingCheckBlockVersionResponse* response,
                       ::google::protobuf::Closure* done);
    void PickRecoverBlocks(::google::protobuf::RpcController* controller,
                       const BlockMappingPickRecoverBlocksRequest* request,
                       BlockMappingPickRecoverBlocksResponse* response,
                       ::google::protobuf::Closure* done);
    void ProcessRecoveredBlocks(::google::protobuf::RpcController* controller,
                       const BlockMappingProcessRecoveredBlocksRequest* request,
                       BlockMappingProcessRecoveredBlocksResponse* response,
                       ::google::protobuf::Closure* done);
    void GetCloseBlocks(::google::protobuf::RpcController* controller,
                       const BlockMappingGetCloseBlocksRequest* request,
                       BlockMappingGetCloseBlocksResponse* response,
                       ::google::protobuf::Closure* done);
    void GetStat(::google::protobuf::RpcController* controller,
                       const BlockMappingGetStatRequest* request,
                       BlockMappingGetStatResponse* response,
                       ::google::protobuf::Closure* done);
    void ListRecover(::google::protobuf::RpcController* controller,
                       const BlockMappingListRecoverRequest* request,
                       BlockMappingListRecoverResponse* response,
                       ::google::protobuf::Closure* done);
private:
    ThreadPool* thread_pool_;
    std::vector<BlockMapping*> block_mapping_;
    Mutex mu_;
    int64_t max_block_id_;
};

}
}

#endif
