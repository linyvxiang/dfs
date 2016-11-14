# 简介


百度的核心业务和数据库系统依赖分布式文件系统作为底层存储，文件系统的可用性和性能对上层搜索业务的稳定性与效果有着至关重要的影响。现有的分布式文件系统（如HDFS等）是为离线批处理设计的，无法在保证高吞吐的情况下做到低延迟和持续可用，所以我们从搜索的业务特点出发，设计了百度文件系统。

开源地址：https://github.com/baidu/bfs

#设计目标

- 持续可用
- 高吞吐
- 低延时
- 水平扩展

#系统架构：
![BFS整体架构](https://static.oschina.net/uploads/img/201610/26152136_Ew2E.png "BFS整体架构")

系统整体分为NameServer、MetaServer、ChunkServer、SDK、bfs_client、bfs_mount 6个模块

- NameServer负责目录树的管理
- MetaServer负责对ChunkServer以及当前block状态的管理
- ChunkServer负责数据的存储和读写
- SDK是以静态库的形式发布的系统API
- bfs_client作为管理集群的二进制工具
- bfs_mount用于将BFS挂载到本地

#特点

- NameServer高可用

   多台NameServer之间自动选主，并通过Raft协议进行同步，当主宕机后，剩余NameServer会自动选出新主，不影响集群的可用性

- 高吞吐，低延时

   ChunkServer写流程异步化，最大化存储介质IO吞吐，全局负载均衡，支持链式写与扇出写模式，遇到慢节点时自动规避

- 高可靠

   ChunkServer宕机后快速进行副本恢复，根据存货副本数区别恢复优先级，支持自动将不同副本进行多机房部署，保证数据不丢失

- 水平扩展

   支持两地三机房，1万+台机器管理

#快速使用

使用单机搭建BFS模拟集群，只需要三步：`

1. 下载BFS源码

   `git clone https://github.com/baidu/bfs`
2. 一键编译

  ` cd bfs; ./build.sh`

1. 一键启动

  ` cd sandbox; ./deploy.sh; start_bfs.sh`

到这里，一个用来在单机上测试的BFS集群便已经搭建完成了～   可以通过`bfs_client`上传一个文件进行测试：`./bfs_client put XXX YYY`，其中`XXX`为本地任意文件，`YYY`为希望放到BFS上后的文件名。

#SDK使用

执行`make install`，会将使用SDK所需的头文件及静态库放到`output`目录下，只需包含对应的头文件，链接`libbfs.a`静态库，便可以在程序中使用BFS存储数据

示例代码如下：



    #include <iostream>
    #include "bfs.h"
    int main(int argc, char* argv[])
    {
        if (argc < 3) {
        std::cerr << "Usage ./main <source_file> <target_file>" << std::endl;
        exit(EXIT_FAILURE);
        }
        baidu::bfs::FS* fs;
            if (!baidu::bfs::FS::OpenFileSystem("bfs.flag", &fs_, FSOptions())) {
            std::cerr << "Open filesytem failed " << FLAGS_nameserver_nodes << std::endl;
            exit(EXIT_FAILURE);
        }
        FILE* fp = fopen(source.c_str(), "rb");
        if (fp == NULL) {
            std::cerr << "Open local file failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        baidu::bfs::File* file;
        if (fs_->OpenFile(filename.c_str(), O_WRONLY | O_TRUNC, 664, &file, WriteOptions()) != OK) {
            std::cerr << "Open BFS file failed " << filename << std::endl;
            exit(EXIT_FAILURE);
        }
    
        char buf[10240];
        int64_t len = 0;
        int32_t bytes = 0;
        while ( (bytes = fread(buf, 1, sizeof(buf), fp)) > 0) {
            int32_t write_bytes = file->Write(buf, bytes);
            if (write_bytes < bytes) {
                std::cerr << "Write to BFS failed" << std::endl;
                exit(EXIT_FAILURE);
            }
            len += bytes;
        }
    
        fclose(fp);
        if (file->Close() != 0) {
            std::cerr << "Close BFS file failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        delete file;
        return 0;
    }



# 挂载到本地使用

BFS支持将某个目录挂载到本地进行使用，方法如下：

1. 编译挂载工具

    `make bfs_mount`

2. 进行挂载

   `nohup ./bfs_mount  -d /home/xxx/ -c localhost:8828 -p /yyy > fuse_log 2>&1 &`

   其中，`/home/xxx`为需要挂载到的本地目录，`localhost:8828`为BFS集群的NameServer地址，`/yyy`为需要挂载的BFS目录

3. 使用

   可以`cd`到`/home/xxx`目录下，像使用本地文件系统一样使用BFS

#支持团队

百度网页搜索部开源团队 opensearch@baidu.com