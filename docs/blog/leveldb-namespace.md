# 利用leveldb实现文件系统的目录树

目录树维护了整个文件系统的元信息，所有对文件系统中文件的增删查改操作都首先需要经过目录树的操作才能进行。百度开源的分布式文件系统BFS(开源地址：https://github.com/baidu/bfs)利用leveldb，实现了目录树的管理，使得目录树的实现非常简洁，同时对目录树的操作十分高效。本文将为你解析其具体设计及实现思路。

目录树为一个树型结构，而leveldb是一个kv存储引擎，因此，如果想用通过leveldb实现目录树，则需要把树型结构映射成kv的扁平化结构。

单独就每个文件或者目录来讲，其信息只需要一条kv记录即可保存，而这条kv中的value需要保存该目录或文件的属性信息，可变动的空间不大。所以，从树型结构到kv记录的映射 ，关键在于key的选取。

在BFS的目录树中，定义了一个`int64_t`的整型数字作为`EntryID`，每个文件或目录拥有一个`EntryID`，并且全局唯一，根目录的`EntryID`规定为1。假设有如下目录结构：

	/home/dirx/
	          /filex
	      diry/
	          /filey
	/tmp/
	     filez
按照创建顺序，我们依次给`/home`分配的`EntryID`为2，`/tmp`的`EntryID`为3，`/home/dirx`的`EntryId`为4，`/home/diry`的`EntryID`为5，`/home/tmp/filez`的`EntryID`为6，`/home/dirx/filex`的`EntryID`为7，`/home/diry/filey`的`EntryID`为8。

注意到每个目录拥有一个自己的`EntryID`的同时，又肯定拥有一个父目录，其父目录又肯定拥有一个`EntryID`，可以利用这一点，通过父目录的`EntryID`和子目录中的文件名，来确定一条记录的key：对于每个文件或者目录，我们规定：每条kv记录的key为 "父目录的`EntryID`+自身文件名"，同时在value中存储自己的`EntryID`这样编码后，上述目录树便可以表示为如下kv记录：

	1home -> 2 + xxx
	1tmp -> 3 + xxx
	2dirx -> 4 + xxx
	2diry -> 5 + xxx
	3filez -> 6 + xxx
	4filex -> 7 + xxx
	5filey -> 8 + xxx
其中，`xxx`表示该目录或文件的其它信息，如大小，创建时间，实际数据存放位置等。

到此，目录树这个树型结构，便已经平展成为一条条的kv记录，对目录树的操作，便转化成了对某几条kv记录的操作：

- 对于创建文件操作，比如想创建`/home/work/`目录，则首先在`/`目录中查找`home`目录，由于`/`的`EntryID`为1，所以第一次查找时，key为`1home`，然后读出其value，解析后发现`/home`的`EntryID`为2，则将此`EntryID`记下，继续往下走，发现`work`即为所需要创建的文件，则为其申请一个`EntryID`（假设为9），此时，写入一条记录，按照上面的规则，其key为`2work`，value为`work`创建的时间等信息，以及`work`的`EntryID`（9）
- 对于删除操作，比如把刚刚创建的`/home/work`目录删除，只需要将key为`2work`的这条记录删除即可
- 对于读取操作，比如想读取`/home/dirx/filex`文件中的内容，则首先读取`1home`这条key所对应的value，解析发现value中记录的`EntryID`为2，然后再去读取`2dirx`这条key所对应的value，解析发现value中记录的`EntryID`为4，然后再去读取`4filex`这条key所对应的value，从里面解析出`/home/dirx/filex`的实际数据存放位置，进行文件内容的读取
- 对于List目录操作，比如想看看根目录下有哪些文件和目录，由于每个文件和目录在存储时，其key中都包含父目录的`EntryID`，因此，只需进行一次扫描即可。比如`ls /`，则只需扫描leveldb中，以`1\0x0`为前缀的key即可，当遇到2时停止，所得结果即为`/`目录下的所有内容
- 对于Rename操作，只需要改动其key即可。比如想要把`/home/diry/filey`文件移动到`home/dirx`目录中，按照之前的规则，`/home/diry/filey`在leveldb中存储的key为`5filey`，`/home/dirx`的`EntryID`为4，把`5filey`这条记录中的内存读取出来，以`4filey`为key，再次存储到leveldbk ，然后将`5filey`这条记录删除，即完成了Rename操作

这样，一个目录树所需要的基本操作便已经支持，由于leveldb引擎本身写入速度较快，并且在读取时，内部本身已经有cache来缓存住较热的kv数据，并且缓存大小可配置，所以一个非常简洁高效的目录树便实现了~