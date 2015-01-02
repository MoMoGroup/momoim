#pragma once

typedef struct structOnlineUsersTableType OnlineUsersTableType;
typedef OnlineUsersTableType *POnlineUsersTableType;
typedef struct structOnlineUser OnlineUser;
typedef OnlineUser *POnlineUser;
typedef struct structOnlineUserInfo OnlineUserInfo;
typedef OnlineUserInfo *POnlineUserInfo;
typedef struct structUserCancelableOperationTable UserCancelableOperationTable;
typedef UserCancelableOperationTable *PUserCancelableOperationTable;
typedef struct structUserCancelableOperation UserCancelableOperation;
typedef UserCancelableOperation *PUserCancelableOperation;

typedef int (*OperationCancelHandler)(POnlineUser, PUserCancelableOperation);

typedef struct structUserFileStoreOperation UserFileStoreOperation;
typedef UserFileStoreOperation *PUserFileStoreOperation;
typedef struct structUserMessageProcessor UserMessageProcessor;
typedef UserMessageProcessor *PUserMessageProcessor;

typedef int(*PacketHandler)(POnlineUser user, uint32_t session, void *packet, CRPBaseHeader *header);
