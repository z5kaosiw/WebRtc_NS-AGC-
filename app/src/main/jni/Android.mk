LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
APP_ABI := armeabi x86
LOCAL_MODULE    := webrtc_audio
app_platform:=android-21
LOCAL_SRC_FILES := \
        agc/complex_bit_reverse.c \
        agc/complex_fft.c \
        agc/cross_correlation.c \
        agc/dot_product_with_scale.c \
        agc/downsample_fast.c \
        agc/energy.c \
        agc/fft4g.c \
        agc/get_scaling_square.c \
        agc/min_max_operations.c \
        agc/real_fft.c \
        agc/resample.c \
        agc/resample_48khz.c \
        agc/resample_by_2.c \
        agc/resample_by_2_internal.c \
        agc/resample_by_2_mips.c \
        agc/copy_set_operations.c \
        agc/division_operations.c \
        agc/spl_init.c \
        agc/spl_sqrt.c \
        agc/spl_sqrt_floor.c \
        agc/ring_buffer.c \
        agc/resample_fractional.c \
        agc/splitting_filter.c \
        agc/vector_scaling_operations.c \
        agc/analog_agc.c \
        agc/digital_agc.c \
        agc/ns_core.c \
        agc/nsx_core.c \
        agc/nsx_core_c.c \
        agc/nsx_core_neon_offsets.c \
        agc/noise_suppression.c \
        agc/noise_suppression_x.c \
        ns_jni_wrapper.c \

#undefined reference to 错误问题解决办法
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
APP_SHORT_COMMANDS := true
LOCAL_LDLIBS    := -llog
APP_CPPFLAGS := -frtti -std=c++11
include $(BUILD_SHARED_LIBRARY)

