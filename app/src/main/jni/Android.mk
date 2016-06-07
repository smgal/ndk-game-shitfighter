LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#####################################################

SRC_LIBZIP_ROOT := libzip
SRC_LIBZIP      := \
	               $(SRC_LIBZIP_ROOT)/zip_add.c \
	               $(SRC_LIBZIP_ROOT)/zip_add_dir.c \
	               $(SRC_LIBZIP_ROOT)/zip_close.c \
	               $(SRC_LIBZIP_ROOT)/zip_delete.c \
	               $(SRC_LIBZIP_ROOT)/zip_dirent.c \
	               $(SRC_LIBZIP_ROOT)/zip_entry_free.c \
	               $(SRC_LIBZIP_ROOT)/zip_entry_new.c \
	               $(SRC_LIBZIP_ROOT)/zip_err_str.c \
	               $(SRC_LIBZIP_ROOT)/zip_error.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_clear.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_get.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_get_sys_type.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_strerror.c \
	               $(SRC_LIBZIP_ROOT)/zip_error_to_str.c \
	               $(SRC_LIBZIP_ROOT)/zip_fclose.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_error_clear.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_error_get.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_get_offset.c \
	               $(SRC_LIBZIP_ROOT)/zip_file_strerror.c \
	               $(SRC_LIBZIP_ROOT)/zip_filerange_crc.c \
	               $(SRC_LIBZIP_ROOT)/zip_fopen.c \
	               $(SRC_LIBZIP_ROOT)/zip_fopen_index.c \
	               $(SRC_LIBZIP_ROOT)/zip_fread.c \
	               $(SRC_LIBZIP_ROOT)/zip_free.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_archive_comment.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_archive_flag.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_file_comment.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_num_files.c \
	               $(SRC_LIBZIP_ROOT)/zip_get_name.c \
		           $(SRC_LIBZIP_ROOT)/zip_memdup.c \
		           $(SRC_LIBZIP_ROOT)/zip_name_locate.c \
		           $(SRC_LIBZIP_ROOT)/zip_new.c \
		           $(SRC_LIBZIP_ROOT)/zip_open.c \
		           $(SRC_LIBZIP_ROOT)/zip_rename.c \
		           $(SRC_LIBZIP_ROOT)/zip_replace.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_archive_comment.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_archive_flag.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_file_comment.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_buffer.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_file.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_filep.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_free.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_function.c \
		           $(SRC_LIBZIP_ROOT)/zip_source_zip.c \
		           $(SRC_LIBZIP_ROOT)/zip_set_name.c \
		           $(SRC_LIBZIP_ROOT)/zip_stat.c \
		           $(SRC_LIBZIP_ROOT)/zip_stat_index.c \
		           $(SRC_LIBZIP_ROOT)/zip_stat_init.c \
		           $(SRC_LIBZIP_ROOT)/zip_strerror.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange_all.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange_archive.c \
		           $(SRC_LIBZIP_ROOT)/zip_unchange_data.c

################################################################################
## App Base

APP_SRC_DIR           := src

APP_AVEJ_SRC_DIR      := $(APP_SRC_DIR)/avej_lite
APP_AVEJ_SRC_FILES    := \
		$(APP_AVEJ_SRC_DIR)/avej_sound_mixer.cpp \
		$(APP_AVEJ_SRC_DIR)/avej_util_smgal_decoder.cpp \
		$(APP_AVEJ_SRC_DIR)/adaptation/avej_app_impl.cpp \
		$(APP_AVEJ_SRC_DIR)/adaptation/avej_gfx_impl.cpp \
		$(APP_AVEJ_SRC_DIR)/adaptation/gfx3d_opengl.cpp \
		$(APP_AVEJ_SRC_DIR)/adaptation/avej_util_impl.cpp

APP_UTIL_SRC_DIR      := $(APP_SRC_DIR)/util
APP_UTIL_SRC_FILES    := \
		$(APP_UTIL_SRC_DIR)/util_convert_to_ucs.cpp \
		$(APP_UTIL_SRC_DIR)/util_convert_to_ucs_table.cpp \
		$(APP_UTIL_SRC_DIR)/util_render_text.cpp

APP_SRC_FILES         := $(APP_UTIL_SRC_FILES) $(APP_AVEJ_SRC_FILES)
APP_INC_FILES         := -I./$(APP_SRC_DIR) -I./$(APP_UTIL_SRC_DIR) -I./$(APP_AVEJ_SRC_DIR) -I./$(APP_AVEJ_SRC_DIR)/adaptation

################################################################################
## App Main

APP_SHITFTR_SRC_DIR   := $(APP_SRC_DIR)/src_ShFgtr_wave
APP_SHITFTR_SRC_FILES := \
		$(APP_SHITFTR_SRC_DIR)/sfwv_gfx.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_main.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_map.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_obj.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_obj_enemy.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_obj_friend.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_res.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_res_map.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_res_shit_img.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_res_story.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_state_ending_bad.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_state_ending_happy.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_state_game_play.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_state_menu_about.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_state_menu_option.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_state_menu_story.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_state_title.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_sys_desc.cpp \
		$(APP_SHITFTR_SRC_DIR)/sfwv_util.cpp

APP_SRC_FILES         += $(APP_SHITFTR_SRC_FILES)

################################################################################

LOCAL_MODULE    := native-lib

LOCAL_SRC_FILES := jni-main.cpp avej-launcher-glue.cpp
LOCAL_SRC_FILES += $(APP_SRC_DIR)/avejapp_register.cpp
LOCAL_SRC_FILES += $(APP_SRC_FILES) $(SRC_LIBZIP)


LOCAL_CFLAGS    := -O2 -fexceptions -fshort-wchar
LOCAL_CFLAGS    += -DTARGET_DEVICE=TARGET_ANDROID -DDEVICE_RES=DEVICE_RES_800x480
LOCAL_CFLAGS    += -I. -I./libzip $(APP_INC_FILES)

LOCAL_DISABLE_FATAL_LINKER_WARNINGS=true

# for OpenGL ES 1.1
LOCAL_LDLIBS    += -lGLESv1_CM
# for logging
LOCAL_LDLIBS    += -llog -lz

include $(BUILD_SHARED_LIBRARY)
