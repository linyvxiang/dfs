// Copyright (c) 2016, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "block_mapping_manager_impl.h"

namespace baidu {
namespace bfs {

BlockMappingManagerImpl::BlockMappingManagerImpl() {
}

BlockMappingManagerImpl::~BlockMappingManagerImpl() {
}

void BlockMappingManagerImpl::GetLocatedBlock(::google::protobuf::RpcController* controller,
                       const BlockMappingGetLocatedBlockRequest* request,
                       BlockMappingGetLocatedBlockResponse* response,
                       ::google::protobuf::Closure* done) {
}
void BlockMappingManagerImpl::ChangeReplicaNum(::google::protobuf::RpcController* controller,
                   const BlockMappingChangeReplicaNumRequest* request,
                   BlockMappingChangeReplicaNumResponse* response,
                   ::google::protobuf::Closure* done) {
}
void BlockMappingManagerImpl::AddNewBlock(::google::protobuf::RpcController* controller,
                   const BlockMappingAddNewBlockRequest* request,
                   BlockMappingAddNewBlockResponse* response,
                   ::google::protobuf::Closure* done) {
}
void BlockMappingManagerImpl::UpdateBlockInfo(::google::protobuf::RpcController* controller,
                   const BlockMappingUpdateBlockInfoRequest* request,
                   BlockMappingUpdateBlockInfoResponse* response,
                   ::google::protobuf::Closure* done) {
}
void BlockMappingManagerImpl::RemoveBlocksForFile(::google::protobuf::RpcController* controller,
                   const BlockMappingRemoveBlocksForFileRequest* request,
                   BlockMappingRemoveBlocksForFileResponse* response,
                   ::google::protobuf::Closure* done) {
}
void BlockMappingManagerImpl::RemoveBlock(::google::protobuf::RpcController* controller,
                   const BlockMappingRemoveBlockRequest* request,
                   BlockMappingRemoveBlockResponse* response,
                   ::google::protobuf::Closure* done) {
}
void BlockMappingManagerImpl::DealWithDeadNode(::google::protobuf::RpcController* controller,
                   const BlockMappingDealWithDeadNodeRequest* request,
                   BlockMappingDealWithDeadNodeResponse* response,
                   ::google::protobuf::Closure* done) {
}
void BlockMappingManagerImpl::CheckBlockVersion(::google::protobuf::RpcController* controller,
                   const BlockMappingCheckBlockVersionRequest* request,
                   BlockMappingCheckBlockVersionResponse* response,
                   ::google::protobuf::Closure* done) {
}

}
}
