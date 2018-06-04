LOCAL_PATH:= $(call my-dir)

#libiconv.so
include $(CLEAR_VARS)

LOCAL_MODULE := iconv_static
LOCAL_MODULE_FILENAME := libiconv

LOCAL_CFLAGS := \
  -Wno-multichar \
  -DAndroid \
  -DLIBDIR="c" \
  -DBUILDING_LIBICONV \
  -DIN_LIBRARY

LOCAL_SRC_FILES := \
  src/libcharset/lib/localcharset.c \
  src/lib/iconv.c \
  src/lib/relocatable.c

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/src/include \
  $(LOCAL_PATH)/src/libcharset \
  $(LOCAL_PATH)/src/lib \
  $(LOCAL_PATH)/src/libcharset/include \
  $(LOCAL_PATH)/src/srclib
include $(BUILD_STATIC_LIBRARY)