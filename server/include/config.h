//最大客户端数
#define MAX_CLIENTS 2000
//监听器最大等待队列数
#define LISTENER_BACKLOG 128
//epoll最大事件个数
#define EPOLL_BACKLOG 32
//事务处理线程数
#define WORKER_COUNT 2
//事务池大小
#define CONFIG_JOB_QUEUE_SIZE (MAX_CLIENTS / 4)
//监听端口号
#define LISTEN_PORT 8014

