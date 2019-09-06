#APP_ABI := armeabi armeabi-v7a x86
APP_ABI := armeabi x86
APP_STL := stlport_static
APP_CPPFLAGS := -frtti -std=c++11
APP_SHORT_COMMANDS := true
LOCAL_LDFLAGS += -fuse-ld=bfd
