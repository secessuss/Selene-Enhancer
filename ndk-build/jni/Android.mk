LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := enhancer_binary

LOCAL_SRC_FILES := enhancer_logic.cpp

LOCAL_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)