// Copyright (c) 2016, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include <boost/bind.hpp>
#include <gflags/gflags.h>

#include "block_mapping_manager_impl.h"

#include "nameserver/block_mapping.h"
#include "proto/status_code.pb.h"

DECLARE_int32(block_mapping_bucket_num);

namespace baidu {
namespace bfs {

BlockMappingManagerImpl::BlockMappingManagerImpl() {
    thread_pool_ = new ThreadPool(FLAGS_block_mapping_bucket_num);
    block_mapping_.resize(FLAGS_block_mapping_bucket_num);
    for (size_t i = 0; i < block_mapping_.size(); i++) {
        block_mapping_[i] = new BlockMapping();
    }
}

BlockMappingManagerImpl::~BlockMappingManagerImpl() {
    delete thread_pool_;
    for (size_t i = 0; i < block_mapping_.size(); i++) {
        delete block_mapping_[i];
        block_mapping_[i] = NULL;
    }
}

void BlockMappingManagerImpl::GetLocatedBlock(::google::protobuf::RpcController* controller,
                       const BlockMappingGetLocatedBlockRequest* request,
                       BlockMappingGetLocatedBlockResponse* response,
                       ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::GetLocatedBlock,
                                            this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int64_t block_id = request->block_id();
    std::vector<int32_t> replica;
    int64_t block_size = 0;
    int bucket_offset = block_id % block_mapping_.size();
    if (!block_mapping_[bucket_offset]->GetLocatedBlock(block_id, &replica, &block_size)) {
        response->set_status(kNotOK);
        done->Run();
        return;
    }
    response->set_block_size(block_size);
    for (size_t i = 0; i < replica.size(); i++) {
        response->add_replica(replica[i]);
    }
    response->set_status(kOK);
    done->Run();
}
void BlockMappingManagerImpl::ChangeReplicaNum(::google::protobuf::RpcController* controller,
                   const BlockMappingChangeReplicaNumRequest* request,
                   BlockMappingChangeReplicaNumResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::ChangeReplicaNum,
                                        this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int64_t block_id = request->block_id();
    int32_t replica_num = request->replica_num();
    int bucket_offset = block_id % block_mapping_.size();
    if (!block_mapping_[bucket_offset]->ChangeReplicaNum(block_id, replica_num)) {
        response->set_status(kNotOK);
        done->Run();
        return;
    }
    response->set_status(kOK);
    done->Run();
}
void BlockMappingManagerImpl::AddNewBlock(::google::protobuf::RpcController* controller,
                   const BlockMappingAddNewBlockRequest* request,
                   BlockMappingAddNewBlockResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::AddNewBlock,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int64_t block_id = request->block_id();
    int32_t replica = request->replica();
    int64_t version = request->block_version();
    int64_t block_size = request->block_size();
    int bucket_offset = block_id % block_mapping_.size();
    if (!request->init_replicas_size()) {
        block_mapping_[bucket_offset]->AddNewBlock(block_id, replica, version, block_size, NULL);
    } else {
        std::vector<int32_t> init_replica;
        for (int i = 0; i < request->init_replicas_size(); i++) {
            init_replica.push_back(request->init_replicas(i));
        }
        block_mapping_[bucket_offset]->AddNewBlock(block_id, replica, version, block_size, &init_replica);
    }
    response->set_status(kOK);
    done->Run();
}
void BlockMappingManagerImpl::UpdateBlockInfo(::google::protobuf::RpcController* controller,
                   const BlockMappingUpdateBlockInfoRequest* request,
                   BlockMappingUpdateBlockInfoResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::UpdateBlockInfo,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int64_t block_id = request->block_id();
    int32_t cs_id = request->cs_id();
    int64_t block_size = request->block_size();
    int64_t block_version = request->block_version();
    int bucket_offset = block_id % block_mapping_.size();
    if (!block_mapping_[bucket_offset]->UpdateBlockInfo(block_id, cs_id, block_size, block_version)) {
        response->set_status(kNotOK);
    } else {
        response->set_status(kOK);
    }
    done->Run();
}
void BlockMappingManagerImpl::RemoveBlocksForFile(::google::protobuf::RpcController* controller,
                   const BlockMappingRemoveBlocksForFileRequest* request,
                   BlockMappingRemoveBlocksForFileResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::RemoveBlocksForFile,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    FileInfo file_info = request->fileinfo();
    for (int i = 0; i < file_info.blocks_size(); i++) {
        int bucket_offset = file_info.blocks(i) % block_mapping_.size();
        block_mapping_[bucket_offset]->RemoveBlocksForFile(file_info);
    }
    response->set_status(kOK);
    done->Run();
}
void BlockMappingManagerImpl::RemoveBlock(::google::protobuf::RpcController* controller,
                   const BlockMappingRemoveBlockRequest* request,
                   BlockMappingRemoveBlockResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::RemoveBlock,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int64_t block_id = request->block_id();
    int bucket_offset = block_id % block_mapping_.size();
    block_mapping_[bucket_offset]->RemoveBlock(block_id);
    response->set_status(kOK);
    done->Run();
}
void BlockMappingManagerImpl::DealWithDeadNode(::google::protobuf::RpcController* controller,
                   const BlockMappingDealWithDeadNodeRequest* request,
                   BlockMappingDealWithDeadNodeResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::DealWithDeadNode,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    std::vector<std::set<int64_t> > blocks_array;
    blocks_array.resize(block_mapping_.size());
    for (int i = 0; i < request->blocks_size(); i++) {
        int bucket_offset = request->blocks(i) % block_mapping_.size();
        blocks_array[bucket_offset].insert(request->blocks(i));
    }
    int32_t cs_id = request->cs_id();
    for (size_t i = 0; i < blocks_array.size(); i++) {
        block_mapping_[i]->DealWithDeadNode(cs_id, blocks_array[i]);
    }
    response->set_status(kOK);
    done->Run();
}
void BlockMappingManagerImpl::CheckBlockVersion(::google::protobuf::RpcController* controller,
                   const BlockMappingCheckBlockVersionRequest* request,
                   BlockMappingCheckBlockVersionResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::CheckBlockVersion,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int64_t block_id = request->block_id();
    int64_t block_version = request->block_version();
    int bucket_offset = block_id % block_mapping_.size();
    StatusCode status = block_mapping_[bucket_offset]->CheckBlockVersion(block_id, block_version);
    response->set_status(status);
    done->Run();
}

void BlockMappingManagerImpl::PickRecoverBlocks(::google::protobuf::RpcController* controller,
                   const BlockMappingPickRecoverBlocksRequest* request,
                   BlockMappingPickRecoverBlocksResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::PickRecoverBlocks,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int32_t cs_id = request->cs_id();
    int num = request->num();
    int32_t hi_num = 0;
    std::map<int64_t, std::set<int32_t> > recover_blocks;
    for (size_t i = 0; i < block_mapping_.size(); i++) {
        int tmp_hi = 0;
        block_mapping_[i]->PickRecoverBlocks(cs_id, num / block_mapping_.size() + 1, &recover_blocks, &hi_num);
        hi_num += tmp_hi;
    }
    response->set_hi_num(hi_num);
    for (std::map<int64_t, std::set<int32_t> >::iterator it = recover_blocks.begin();
            it != recover_blocks.end(); ++it) {
        RecoverBlockInfo* block = response->add_block_info();
        block->set_block_id(it->first);
        for (std::set<int32_t>::iterator cs_it = it->second.begin();
                cs_it != it->second.end(); ++cs_it) {
           block->add_cs_id(*cs_it);
       }
    }
    response->set_status(kOK);
    done->Run();
}

void BlockMappingManagerImpl::GetCloseBlocks(::google::protobuf::RpcController* controller,
                   const BlockMappingGetCloseBlocksRequest* request,
                   BlockMappingGetCloseBlocksResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::GetCloseBlocks,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int32_t cs_id = request->cs_id();
    for (size_t i = 0; i < block_mapping_.size(); i++) {
        block_mapping_[i]->GetCloseBlocks(cs_id, response->mutable_blocks());
    }
    response->set_status(kOK);
    done->Run();
}

void BlockMappingManagerImpl::GetStat(::google::protobuf::RpcController* controller,
                   const BlockMappingGetStatRequest* request,
                   BlockMappingGetStatResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::GetStat,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    int64_t lo_recover_num = 0, hi_recover_num = 0, lo_pending = 0, hi_pending = 0,
            lost_num = 0, incomplete_num = 0;
    for (size_t i = 0; i < block_mapping_.size(); i++) {
        int64_t l_r, h_r, l_p, h_p, l_n, i_n;
        block_mapping_[i]->GetStat(&l_r, &h_r, &l_p, &h_p, &l_n, &i_n);
        lo_recover_num += l_r;
        hi_recover_num += h_r;
        lo_pending += l_p;
        hi_pending += h_p;
        lost_num += l_n;
        incomplete_num += i_n;
    }
    response->set_lo_recover_num(lo_recover_num);
    response->set_hi_recover_num(hi_recover_num);
    response->set_lo_pending(lo_pending);
    response->set_hi_pending(hi_pending);
    response->set_lost_num(lost_num);
    response->set_incomplete_num(incomplete_num);
    response->set_status(kOK);
    done->Run();
}

void BlockMappingManagerImpl::ListRecover(::google::protobuf::RpcController* controller,
                   const BlockMappingListRecoverRequest* request,
                   BlockMappingListRecoverResponse* response,
                   ::google::protobuf::Closure* done) {
    if (!response->has_sequence_id()) {
        response->set_sequence_id(request->sequence_id());
        boost::function<void ()> task = boost::bind(&BlockMappingManagerImpl::ListRecover,
                this, controller, request, response, done);
        thread_pool_->AddTask(task);
        return;
    }
    std::string hi_recover, lo_recover, lost, hi_check, lo_check, incomplete;
    //TODO check whether need to continue
    for (size_t i = 0; i < block_mapping_.size(); i++) {
        block_mapping_[i]->ListRecover(&hi_recover, &lo_recover, &lost, &hi_check, &lo_check, &incomplete);
    }
    response->set_hi_recover(hi_recover);
    response->set_lo_recover(lo_recover);
    response->set_lost(lost);
    response->set_hi_check(hi_check);
    response->set_lo_check(lo_check);
    response->set_incomplete(incomplete);
    response->set_status(kOK);
    done->Run();
}

}
}
