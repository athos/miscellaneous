# ターゲット実行形式ファイル名
TARGET = shisoukai_0727

# nxtOSEKルートディレクトリ
NXTOSEK_ROOT = ../../../..

# インクルードパス
USER_INC_PATH= $(NXTOSEK_ROOT)/ecrobot/nxtway_gs_balancer

# ライブラリ
USER_LIB = nxtway_gs_balancer

# Cソースファイル
TARGET_SOURCES :=          \
	../src/start.c         \
	../src/model_control.c \
	../src/command/bluetooth.c \
	../src/command/command.c \
	../src/command/command_factory.c \
	../src/command/command_c.c \
	../src/command/decoder.c \
	../src/command/encoder.c \
	../src/command/byte_buffer.c \
	../src/robot/robot.c \
	../src/robot/device/sensor.c \
	../src/robot/device/motor.c \
	../src/robot/device/internal.c \
	../src/strategy/soukou.c         \
	../src/strategy/controlval.c         \
	../src/strategy/recv.c         \
	../src/strategy/seesaw.c         \
	../src/strategy/performancepart2.c         \
	../src/strategy/bridge.c         \
	../src/strategy/obstacle.c         \
	../src/strategy/dispatch.c         \
	../src/strategy/soukou_touritsu.c         \
	../src/control/control.c         \

# TOPPERS/ATK(OSEK)設定ファイル
TOPPERS_OSEK_OIL_SOURCE := ../osek/teamkgk.oil

# 下記のマクロは変更しないでください
O_PATH ?= build

include $(NXTOSEK_ROOT)/ecrobot/ecrobot.mak
