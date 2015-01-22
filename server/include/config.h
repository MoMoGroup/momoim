//最大客户端数
#define CONFIG_MAX_CLIENTS 2000
//监听器最大等待队列数
#define CONFIG_LISTENER_BACKLOG 128
//epoll最大事件个数
#define CONFIG_EPOLL_QUEUE 32
//事务处理线程数
#define CONFIG_WORKER_COUNT 1
//事务池大小
#define CONFIG_JOB_QUEUE_SIZE (CONFIG_MAX_CLIENTS / 4)
//IM功能监听端口号
#define CONFIG_LISTEN_PORT 8014
//主机发现功能端口号
#define CONFIG_HOST_DISCOVER_PORT 8015
//垃圾收集执行概率
#define CONFIG_GC_RADIO (0.002)