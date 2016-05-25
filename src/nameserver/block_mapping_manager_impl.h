// Copyright (c) 2016, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef  BFS_BLOCK_MAPPING_MANAGER_IMPL_H_
#define  BFS_BLOCK_MAPPING_MANAGER_IMPL_H_

#include <common/thread_pool.h>
#include "proto/blockmapping.pb.h"

#include "nameserver/block_mapping.h"

namespace baidu {
namespace bfs {

class BlockMappingManagerImpl : public BlockMappingManager {
public:
    BlockMappingManagerImpl();
    ~BlockMappingManagerImpl();
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
private:
};

}
}

#endif
