#pragma once

typedef struct structOnlineUsersTableType OnlineUsersTableType;
typedef OnlineUsersTableType *POnlineUsersTableType;
typedef struct structOnlineUser OnlineUser;
typedef OnlineUser *POnlineUser;
typedef struct structOnlineUserInfo OnlineUserInfo;
typedef OnlineUserInfo *POnlineUserInfo;
typedef struct structUserOperationTable UserOperationTable;
typedef UserOperationTable *PUserOperationTable;
typedef struct structUserOperation UserOperation;
typedef UserOperation *PUserOperation;

typedef int (*OperationEventHandler)(POnlineUser, PUserOperation);

typedef struct structUserOperationFileStore UserOperationFileStore;
typedef UserOperationFileStore *PUserOperationFileStore;
typedef struct structUserOperationFileRequest UserOperationFileRequest;
typedef UserOperationFileRequest *PUserOperationFileRequest;

typedef struct structUserMessageProcessor UserMessageProcessor;
typedef UserMessageProcessor *PUserMessageProcessor;

typedef int(*PacketHandler)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header);

//RESERVE IO池
typedef struct structIOOperation IOOperation;
typedef IOOperation *PIOOperation;