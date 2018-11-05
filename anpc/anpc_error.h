#pragma once

#define AN_ERRORNO_MAP(XX)														\
XX(0,	AN_SUCCESS,		"success")												\
XX(1,	AN_FAILED,		"failed")												\



#define AN_ERRORNO_GEN(value, name, desc)	ANPC_##name = value,
typedef enum {
	AN_ERRORNO_MAP(AN_ERRORNO_GEN)
}anpc_result_code;
#undef AN_ERRORNO_GEN


#define ANPC_STRERROR_GEN(value, name, desc)	case ANPC_##name : return desc;
inline const char * anpc_error_desc(anpc_result_code err) {
	switch (err)
	{
	AN_ERRORNO_MAP(ANPC_STRERROR_GEN);
	default:
		return "libuv_error";
	}
}
#undef ANPC_STRERROR_GEN

#define ANPC_ERROR_NAME_GEN(value, name, desc)	case ANPC_##name : return #name;
inline const char * anpc_error_name(anpc_result_code err) {
	switch (err)
	{
		AN_ERRORNO_MAP(ANPC_ERROR_NAME_GEN);
	default:
		return "libuv_name";
	}
}
#undef ANPC_ERROR_NAME_GEN
