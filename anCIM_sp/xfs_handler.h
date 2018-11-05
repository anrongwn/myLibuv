#pragma once

//消息处理总泵
int xfs_result_handler(void * arg);

//
#define AN_XFSMESSAGE_MAP(XX)							\
XX(WFS_OPEN_COMPLETE, 0)								\
XX(WFS_CLOSE_COMPLETE, 0)								\
XX(WFS_LOCK_COMPLETE, 0)								\
XX(WFS_UNLOCK_COMPLETE, 0)								\
XX(WFS_REGISTER_COMPLETE, 0)							\
XX(WFS_DEREGISTER_COMPLETE, 0)							\
XX(WFS_GETINFO_COMPLETE, 0)								\
XX(WFS_EXECUTE_COMPLETE, 0)								\
XX(WFS_EXECUTE_EVENT, EXECUTE_EVENTS)					\
XX(WFS_SERVICE_EVENT, SERVICE_EVENTS)					\
XX(WFS_USER_EVENT, USER_EVENTS)							\
XX(WFS_SYSTEM_EVENT, SYSTEM_EVENTS)						\

#define AN_XFSMESSAGE_CLASS_GEN(msg, eventclass)	case msg : return eventclass;
inline int an_get_xfsmessage_class(int msgid) {
	switch (msgid) {
		AN_XFSMESSAGE_MAP(AN_XFSMESSAGE_CLASS_GEN);
	default:
		return 0;
	}
}
#undef AN_XFSMESSAGE_CLASS_GEN

#define AN_XFSMESSAGE_NAME_GEN(msg, eventclass)		case msg : return #msg;
inline const char * an_get_xfsmessage_name(int msgid) {
	switch (msgid) {
		AN_XFSMESSAGE_MAP(AN_XFSMESSAGE_NAME_GEN);
	default:
		return "completed";
	}
}
#undef AN_XFSMESSAGE_NAME_GEN