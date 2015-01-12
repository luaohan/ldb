ldb
===

一个以leveldb 为引擎的分布式kv

说明:本份说明目前仅包含使用方法及协议

第一部分:
本服务器的使用方法:

1，本服务器是以 leveldb 作为引擎的，所以首先你的机器上要安装有 leveldb。如果你
的机器上没有leveldb, 自己必须先下载安装。

    leveldb 的安装方式:
        下载到leveldb 的源码压缩包后，解压后进入leveldb 的目录，make即可。
        然后将leveldb 的头文件拷贝到/usr/local/include目录(需要root权限):
        "sudo cp -r include/leveldb /usr/local/include"
    
    本服务器的源码在编译时必须要有 leveldb 生成的 libleveldb.a 。你成功安装
leveldb 后就会找到 libleveldb.a，这是leveldb 的库文件，这个文件要放到ldb 目
录下，ldb 的源代码才能编译成功。

2,  安装服务器:
    
    目前，本服务器只提供使用源码安装的方法，且源代码只在 github 上是提供，
你可以在你的机器上这样下载源码:"git clone https://github.com/luaohan/ldb.git"，
如果你的机器上没有git 程序，请自己安装。

    如果执行命令"git clone https://github.com/luaohan/ldb.git"成功，会在当前
目录下生成一个ldb 目录，进入这个目录，执行 make 命令，如果一切顺利，即可生
成 server 可执行程序。然后运行命令"./server"，即可运行程序。


3，客户端的使用:
    在 ldb/dbclient 目录下有一个简单的客户端的例子，你可以编译运行。
    目前，客户端只提供 get，set，del 三个基本的方法, 方法的格式,参数及返回值
参见dbclient目录下的 ldbc.h 文件。
    在使用客户端库的时候，必须要包含 ldbc.h 头文件。
    目前，客户端还没有以库文件的方式提供，所以，只能以dbclient 目录下的方式
使用。



4，服务器的配置:
    ldb.conf 是ldb数据库的配置文件，配置文件可以不进行配置，但这个文件必须存在，
否则服务器无法启动。

    配置文件只支持5 个配置项，这 5 个配置项分别是 daemonize，server_port，
logfile，db_directory，level。每个配置项后边只能跟一个参数,配置项与配置参数
之间用空格隔开，配置项的先后顺序无所谓，配置文件的注释采用行注释的方式，
符号"#" 后边的内容将视为注释。

下面对每个配置项进行说明：

    1，daemonize，这个配置项后边只能跟随两种参数，分别是 yes 和 no，
跟其它的参数，则视为错误。yes 代表服务器将以守护进程的方式运行，no 则代表
服务器不以守护进程的方式运行。

    2，server_port，这个配置项后边跟一个数字，这个数字代表服务器的监听的端口号，
这个数字最好大于2000，小于10000，避免与其它的服务冲突。

    3，logfile，这个配置项后边跟一个文件名，表示日志文件，每个日志文件的大小
最大是10M，大于10M 后将有新的日志文件生成，新的日志文件的名称是原来的文件名
后跟这个新文件的生成时间，这个时间精确到毫秒。

    4，db_directory，这个配置项后边跟一个目录名，这个目录下将存储数据库的持
久化文件，这个目录下的文件不能随便删除，否则将会丢失数据。

    5，level，这个配置项表示日志文件的级别， 数字越大，日志越详细, 数字的范
围0~4。虽然这个数字可以取5 个值，但目前服务器只有两个日志级别，分别是
ERROR 和 INFO。

    若配置文件不进行任何配置，那么，它们的默认值分别是：
    1，daemonize    no
    2，server_port  8899
    3，logfile      ./log/loginfo
    4，db_directory ./db_directory
    5，level            4

    如果，在配置logfile 配置项时，若想要在文件名前跟一个目录，首先一定要自己手动
建立这个目录，否则服务器无法启动。
   
第二部分:

ldb 协议: server 与 client 的交互都将采用本协议格式

==================================================================
|unsigned int    |unsigned short |unsigned short|
| packet_len     | type          | key_len      | key | value |
==================================================================


Set: 命令
Client: 发送    | packet_len | type | key_len | key | value  |
Server: 返回    | packet_len | type |     无                 |
Server 一般只返回成功

Get：命令
Client：发送    | packet_len | type | key_len | key | 无     |
Server：有这个key
Server：返回    | packet_len | type | value_len | value | 无 |
Server：没有这个key
Server：返回    |packet_len  | type |     无                 |
Server 一般只返回成功

Del:命令
Client: 发送    | packet_len | type | key_len | key | 无     | 
Server: 返回    | packet_len | type |     无                 |
Server 一般只返回成功
=============================================================================

目前最大的value 值只允许 1M !!!

本服务器还有很多不完善的地方，有待改进。
