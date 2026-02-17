###############################################################################
#
# Makefile for quasi88/UNIX (FreeBSD, Linux, Mac OS X and so on...)
#
#	必ず、GNU make が必要です。
#
#			    この Makefile の作成にあたっては XMAME の 
#			    makefile.unix、src/unix/unix.mak を参考にしました。
#			    コンパイル関連の設定については、上記のファイルに
#			    記載されているコメントが役立つかもしれません。
###############################################################################

# X11版、SDL1.2版、GTK3版のいずれかを設定します
#	X11版の場合は、   「X11_VERSION」以外の行をコメントアウトしてください。
#	SDL1.2版の場合は、「SDL_VERSION」以外の行をコメントアウトしてください。
#	GTK3版の場合は、  「GTK3_VERSION」以外の行をコメントアウトしてください。
#
#	「MINI_VERSION」はコメントアウトのままとしてください。

X11_VERSION	= 1
# SDL_VERSION	= 1
# GTK3_VERSION	= 1
# MINI_VERSION	= 1


#######################################################################
# 基本設定
#######################################################################


# (X11/SDL/GTK3/MINI)
# QUASI88 で使用する各種ディレクトリを設定します。
#	いくつかは、環境変数や起動時のオプションで指定することができますが、
#	それらの指定がない場合の、デフォルトを設定します。
#	 ~/ は、QUASI88の起動時にホームディレクトリ（$HOME）に展開されます。

# ・ROM用ディレクトリを設定します
#	BASIC の ROMイメージ を検索するディレクトリです。

ROMDIR	= ~/quasi88/rom/


# ・DISK用ディレクトリを設定します
#	起動時に、引数で指定したイメージファイルを開く際に、
#	そのファイルを検索するディレクトリです。

DISKDIR	= ~/quasi88/disk/


# ・TAPE用ディレクトリを設定します
#	TAPE のイメージを置くディレクトリです。

TAPEDIR	= ~/quasi88/tape/


# ・スナップショット保存先ディレクトリを設定します
#	スクリーンスナップショットの画像ファイルが保存されるディレクトリです。
#	コメントアウトされている場合は、カレントディレクトリになります。

# SNAPDIR = ~/


# ・ステートファイル保存先ディレクトリを設定します
#	ステートセーブ時のファイルが保存されるディレクトリです。
#	コメントアウトされている場合は、 ~/.quasi88/state/ になります。

# STATEDIR = ~/.quasi88/state/


# ・個別設定ファイル用ディレクトリを設定します
#	起動時にディスクイメージファイルとともに読み込まれる、個別設定
#	ファイルを置くディレクトリです。
#	コメントアウトされている場合は、 ~/.quasi88/rc/ になります。

# LCFGDIR = ~/.quasi88/rc/


# ・全体設定ファイル用ディレクトリを設定します
#	起動時に読み込まれる、環境設定ファイルを置くディレクトリです。
#	コメントアウトされている場合は、 ~/.quasi88/ になります。

# GCFGDIR = ~/.quasi88/



# (X11)
# リトルエンディアンの場合の指定
#	ビッグエンディアンマシンの場合は、コメントアウトしましょう。
#	    例えば、IRIX, AIX、Mac OS X(PowerPC) などは、コメントアウトします。
#		    Intel 系の CPUを使った OS などは、このままにしておきます。

X11_LSB_FIRST	= 1


# (X11)
# MIT-SHM を使うかどうかの指定
#	MIT-SHM をサポートしてない場合は、コメントアウトしましょう。

X11_MITSHM	= 1



# (X11)
# long long 型をサポートしていないコンパイラを使う場合、以下をコメントアウト
# してください。 (大抵のコンパイラはサポートしている・・・ハズ)

X11_HAVE_LONG_LONG = 1



# (X11)
# ライブラリに clock_gettime() 、gettimeofday() がないシステムの場合、
# 以下を各々コメントアウトしてください。
# (今時、大抵のシステムは実装している・・・ハズ)

X11_HAVE_CLOCK_GETTIME = 1
X11_HAVE_GETTIMEOFDAY = 1



# (X11)
# XFree86-VidModeExtension を使った全画面表示を組み込まない場合、以下の行を
# コメントアウトして下さい。

X11_VIDMODE		= 1



# (X11)
# FreeBSD にて USB joystick を使用したい場合は、
# 以下のコメントアウトを外して下さい。

# X11_BSD_USB_JOYSTICK	= 1



# (X11/SDL/GTK3/MINI)
# 以下は、モニターモード (デバッグ用のモード) の機能設定です。
# 通常はモニターモードは使用しないと思われるので、特に変更の必要はありません。
#
#	  MONITOR の行がコメントアウトされている場合は、モニターモードは
#	使用できません。
#
#	  モニターモードにて、GNU Readline を使用する場合、
#	READLINE 行のコメントアウトを外します。
#
#	  モニターモードでの入力待ち時に Ctrl-D を押すと QUASI88 は強制終了
#	してしまいますが、IRIX/AIX では、IGNORE_C_D の行のコメントアウトを
#	外すと、Ctrl-D を押しても終了しなくなります。
#	( IRIX/AIX 以外では、必ずコメントアウトしておいてください。)
#

# USE_MONITOR		= 1

# MONITOR_READLINE	= 1
# MONITOR_IGNORE_C_D	= 1
# MONITOR_USE_LOCALE	= 1



# (X11/SDL/GTK3/MINI)
# PC-8801のキーボードバグをエミュレートしたい場合は、
# 以下のコメントアウトを外して下さい。

# USE_KEYBOARD_BUG	= 1



#######################################################################
# サウンド関連の設定
#######################################################################

# (X11/SDL/MINI)
# MAME/XMAME ベースのサウンド出力を組み込まない場合、以下の行を
# コメントアウトして下さい。

USE_SOUND		= 1



# (X11)
# OS の指定
#	どれか一つを指定してください。
#	generic を選ぶと、サウンドなしになります。
#	そ の 他を選ぶと、サウンドありになる場合もあります。
#	動作確認が取れているのは、FreeBSD と Linux のみです

# FreeBSD
ARCH = freebsd

# Linux
# ARCH = linux

# NetBSD
# ARCH = netbsd

# OpenBSD
# ARCH = openbsd

# Solaris / SunOS
# ARCH = solaris

# QNX Neutrino (QNX4/QNX6)
# ARCH = nto

# OpenStep on NeXT systems
# ARCH = next

# OpenStep on Apple systems (Cocoa)
# ARCH = macosx

# IRIX (with sound using the old AL (version 1) package)
# ARCH = irix

# IRIX (with sound using the al (IRIX 6.x) package)
# ARCH = irix_al

# AIX (with sound, you'll need the UMS and SOM lpp's installed (under AIX4))
# ARCH = aix

# BeOS on Intel
# ARCH = beos

# generic UNIX, no sound
# ARCH = generic



# (X11)
# サウンドデバイスの指定
#	追加したいデバイスがあれば、コメントアウトを外します。
#	いずれも動作確認されていません。

# SOUND_ESOUND		= 1
# SOUND_ALSA		= 1
# SOUND_ARTS_TEIRA	= 1
# SOUND_ARTS_SMOTEK	= 1
# SOUND_SDL		= 1
# SOUND_WAVEOUT		= 1



# (X11/SDL/MINI)
# QUASI88 ver 0.6.3 以前にて使用していた、古いバージョンの MAME/XMAME の
# サウンド出力を使用したい場合は、以下のコメントアウトを外して下さい。
#	動作に必要なリソースが少なめとなります。

# USE_OLD_MAME_SOUND	= 1



#######################################################################
# fmgen (FM Sound Generator) の設定
#######################################################################

# (X11/SDL/MINI)
# cisc氏作の、fmgen (FM Sound Generator) を組み込まない場合、以下の行を
# コメントアウトして下さい。

USE_FMGEN	= 1


# 注意！
#	FM Sound Generator は C++ により実装されています。
#	C++ のコンパイラの設定を以下で行なって下さい。
# 
# 	設定すべき項目は、CXX、CXXFLAGS、CXXLIBS および LD の定義です。
# 



#######################################################################
# SDLライブラリの設定
#######################################################################

# (X11/SDL)
# 以下の場合、SDLライブラリが必要です。
#	・X11_VERSION を選択して、かつ、
#	      サウンドの設定で、「SOUND_SDL = 1」を指定した場合
#	・SDL_VERSION を選択した場合
# ここでは、sdl-config を実行するコマンドを指定してください。
#	通常のOS の場合、sdl-config   のままで大丈夫なはずです。
#	FreeBSD  の場合、sdl12-config などにリネームされていることがあります

SDL_CONFIG	= sdl-config



#######################################################################
# GTK3ライブラリの設定
#######################################################################

# (GTK3)
# GTK3_VERSION を選択した場合のみ、以下の設定が必要です。
# なお、GTK3_VERSION を選択した場合は、サウンド出力は使用できません。
# (「USE_SOUND」の行をコメントアウトしてください。)

GTK3_CONFIG	= pkg-config gtk+-3.0



#######################################################################
# コンパイル関連の設定
#######################################################################

# (X11/SDL/GTK3/MINI)
# C コンパイラの設定
#	gcc 、 clang などを指定してください。

CC	= cc


# (X11/SDL/GTK3/MINI)
# 必要に応じて、コンパイルオプション (最適化など) を指定してください
#	gcc なら、 -fomit-frame-pointer 、 -fstrength-reduce 、 -ffast-math 、
#	-funroll-loops 、 -fstrict-aliasing 等が定番の最適化オプションです。
#
#	コンパイラによっては、char を signed char とみなす指定が必要な場合が
#	あります。PowerPC 系の gcc などがそうですが、この場合、-fsigned-char 
#	を指定します。

CFLAGS = -O2

# 例えば gcc & PowerPC の場合、以下のコメントアウトを外します。
# CFLAGS += -fsigned-char

# 例えば gcc で最適化をしたい場合、以下のコメントアウトを外します。
# CFLAGS += -fomit-frame-pointer -fstrength-reduce -funroll-loops -ffast-math



# (X11/SDL/GTK3/MINI)
# コンパイラによっては、インライン関数を使うことが出来ます。
#	以下から、適切なものを一つだけ指定してください。

# どんなコンパイラでもOK
# USEINLINE	= '-DINLINE=static'

# GCC の場合
USEINLINE	= '-DINLINE=static __inline__'

# Intel C++ コンパイラの場合 (?)
# USEINLINE	= '-DINLINE=static inline'



# (X11)
# X11 関連のディレクトリとライブラリを指定

# 一部のシステム (?)
# X11INC  	= -I/usr/include/X11
# X11LIB  	= -L/usr/lib/X11

# XFree86 の場合 (?)
# X11INC  	= -I/usr/X11R6/include
# X11LIB  	= -L/usr/X11R6/lib

# X.Org の場合
X11INC  	= -I/usr/local/include
X11LIB  	= -L/usr/local/lib



# (X11/SDL/GTK3/MINI)
# C++ コンパイラ関連の設定
#	fmgen を組み込まない場合、これらの設定は不要です。
#
#	g++ 、 clang++ などを指定してください。
#	環境によっては、$(CC) と同じ （ gcc 、 clang など） にします。

CXX	 = c++
CXXFLAGS = -O2
CXXLIBS	 = -lstdc++



# (X11/SDL/GTK3/MINI)
# リンカの設定
#	C++ コンパイラを使う場合、環境によっては $(CXX) とする必要が
#	あるかもしれません。

LD	= $(CC) -Wl,-s
# LD	= $(CXX) -Wl,-s



#######################################################################
# インストールの設定
#######################################################################

# (X11/SDL/GTK3/MINI)
# インストール先ディレクトリの設定
#

BINDIR = /usr/local/bin



###############################################################################
#
# 編集、おつかれさまでした。
# これ以降は、変更不要のはずです。多分・・・
#
###############################################################################

######## 古いコンパイラ(C89とか)では、コメントアウトしないといけないかも・・・

HAVE_INTPTR_T		= 1




######## 実験あれこれ

# コメントアウトすると、その bpp の X環境において動作不能になる実験

X11_SUPPORT_8BPP	= 1
X11_SUPPORT_16BPP	= 1
X11_SUPPORT_32BPP	= 1

# コメントアウトすると、倍サイズでの表示ができなくなる実験

SUPPORT_DOUBLE		= 1



#######################################################################
#
#######################################################################

#
# バージョン毎の設定
#

ifdef	X11_VERSION

# X11 バージョンでの設定

CFLAGS   += $(X11INC) 
#CXXFLAGS += 
LIBS     += $(X11LIB) -lX11 -lXext

# X11バージョンの表示まわり

ifdef	X11_MITSHM
CFLAGS += -DMITSHM 
endif

ifdef	X11_VIDMODE
CFLAGS += -DUSE_VIDMODE
LIBS   += -lXxf86vm
endif

ifdef	X11_SUPPORT_8BPP
CFLAGS += -DSUPPORT_8BPP
endif
ifdef	X11_SUPPORT_16BPP
CFLAGS += -DSUPPORT_16BPP
endif
ifdef	X11_SUPPORT_32BPP
CFLAGS += -DSUPPORT_32BPP
endif

# X11バージョンでのジョイスティック設定

ifdef	X11_BSD_USB_JOYSTICK

CFLAGS += -DUSE_BSD_USB_JOYSTICK

ifeq ($(shell test -f /usr/include/usbhid.h && echo have_usbhid), have_usbhid)
CFLAGS += -DHAVE_USBHID_H
LIBS   += -lusbhid
else
ifeq ($(shell test -f /usr/include/libusbhid.h && echo have_libusbhid), have_libusbhid)
CFLAGS += -DHAVE_LIBUSBHID_H
LIBS   += -lusbhid
else
LIBS   += -lusb
endif
endif

endif

# X11バージョンでの雑多な設定

ifdef	X11_LSB_FIRST
CFLAGS += -DLSB_FIRST
endif

ifdef	X11_HAVE_LONG_LONG
CFLAGS += -DHAVE_LONG_LONG
endif

ifdef	X11_HAVE_CLOCK_GETTIME
CFLAGS += -DHAVE_CLOCK_GETTIME
endif

ifdef	X11_HAVE_GETTIMEOFDAY
CFLAGS += -DHAVE_GETTIMEOFDAY
endif

ifdef	HAVE_SELECT
CFLAGS += -DHAVE_SELECT
endif


CFLAGS += -DQUASI88_X11



else
ifdef	SDL_VERSION

# SDLバージョンでの設定

CFLAGS   += `$(SDL_CONFIG) --cflags`
CXXFLAGS += `$(SDL_CONFIG) --cflags`
LIBS     += `$(SDL_CONFIG) --libs`

CFLAGS += -DQUASI88_SDL

else
ifdef	GTK3_VERSION

# GTK3バージョンでの設定

CFLAGS   += `$(GTK3_CONFIG) --cflags`
CXXFLAGS += `$(GTK3_CONFIG) --cflags`
LIBS     += `$(GTK3_CONFIG) --libs`

CFLAGS += -DQUASI88_GTK3

else
ifdef	MINI_VERSION

# MINIバージョンでの設定

#CFLAGS   +=
#CXXFLAGS +=

CFLAGS += -DQUASI88_MINI

endif
endif
endif
endif



# モニターモード有効時の設定


ifdef	USE_MONITOR
CFLAGS += -DUSE_MONITOR

ifdef	MONITOR_READLINE
CFLAGS += -DUSE_GNU_READLINE
LIBS   += -lreadline -lncurses
endif

ifdef	MONITOR_IGNORE_C_D
CFLAGS += -DIGNORE_CTRL_D
endif

ifdef	MONITOR_USE_LOCALE
CFLAGS += -DUSE_LOCALE
endif

endif


# その他

ifdef	SUPPORT_DOUBLE
CFLAGS += -DSUPPORT_DOUBLE
endif

ifdef	USE_KEYBOARD_BUG
CFLAGS += -DUSE_KEYBOARD_BUG
endif



ifdef	HAVE_INTPTR_T
CFLAGS += -DHAVE_INTPTR_T
endif





#######################################################################
# サウンドが有効になっている場合の各種定義
#	ここでは、
#		SOUND_OBJS
#		SOUND_LIBS
#		SOUND_CFLAGS
#	が定義される。
#######################################################################
ifdef	USE_SOUND

# GTK3_VERSION は、 USE_SOUND の指定禁止

ifdef	GTK3_VERSION
$(error GTK3 can't build with USE_SOUND option)
endif


#
# サウンド有効時の、追加オブジェクト ( OS依存部 + 共用部 )
#

#### ディレクトリ

ifdef	USE_OLD_MAME_SOUND
XMAME_DIR	= $(SNDDRV_DIR)/xmame-old
else
XMAME_DIR	= $(SNDDRV_DIR)/xmame
endif

SD_Q88_DIR	= $(XMAME_DIR)/quasi88
SD_X11_DIR	= $(XMAME_DIR)/quasi88/X11
SD_SDL_DIR	= $(XMAME_DIR)/quasi88/SDL

XM_SRC_DIR	= $(XMAME_DIR)/src
XM_SOUND_DIR	= $(XMAME_DIR)/src/sound
XM_UNIX_DIR	= $(XMAME_DIR)/src/unix
XM_SYSDEP_DIR	= $(XMAME_DIR)/src/unix/sysdep
XM_DSP_DIR	= $(XMAME_DIR)/src/unix/sysdep/dsp-drivers
XM_MIXER_DIR	= $(XMAME_DIR)/src/unix/sysdep/mixer-drivers


#### オブジェクト

SOUND_OBJS_BASE	= $(SD_Q88_DIR)/mame-quasi88.o	\
		  $(SD_Q88_DIR)/beep.o		\
		  $(SD_Q88_DIR)/beepintf.o	\
		  $(SD_Q88_DIR)/pcg.o		\
		  $(SD_Q88_DIR)/pcgintf.o	\
		  $(XM_SRC_DIR)/driver.o	\
		  $(XM_SRC_DIR)/restrack.o	\
		  $(XM_SRC_DIR)/sound.o		\
		  $(XM_SRC_DIR)/sndintrf.o	\
		  $(XM_SRC_DIR)/streams.o	\
		  $(XM_SOUND_DIR)/flt_vol.o	\
		  $(XM_SOUND_DIR)/flt_rc.o	\
		  $(XM_SOUND_DIR)/wavwrite.o	\
		  $(XM_SOUND_DIR)/2203intf.o	\
		  $(XM_SOUND_DIR)/2608intf.o	\
		  $(XM_SOUND_DIR)/ay8910.o	\
		  $(XM_SOUND_DIR)/fm.o		\
		  $(XM_SOUND_DIR)/ymdeltat.o	\
		  $(XM_SOUND_DIR)/samples.o

SOUND_OBJS_UNIX	= $(XM_UNIX_DIR)/sound.o		\
		  $(XM_SYSDEP_DIR)/rc.o			\
		  $(XM_SYSDEP_DIR)/misc.o		\
		  $(XM_SYSDEP_DIR)/plugin_manager.o	\
		  $(XM_SYSDEP_DIR)/sysdep_dsp.o		\
		  $(XM_SYSDEP_DIR)/sysdep_mixer.o	\
		  $(XM_SYSDEP_DIR)/sysdep_sound_stream.o

SOUND_OBJS.linux   = $(XM_DSP_DIR)/oss.o $(XM_MIXER_DIR)/oss.o
SOUND_OBJS.freebsd = $(XM_DSP_DIR)/oss.o $(XM_MIXER_DIR)/oss.o
SOUND_OBJS.netbsd  = $(XM_DSP_DIR)/netbsd.o
#SOUND_OBJS.openbsd = $(XM_DSP_DIR)/oss.o $(XM_MIXER_DIR)/oss.o
SOUND_OBJS.openbsd = $(XM_DSP_DIR)/netbsd.o 
SOUND_OBJS.solaris = $(XM_DSP_DIR)/solaris.o $(XM_MIXER_DIR)/solaris.o
SOUND_OBJS.next    = $(XM_DSP_DIR)/soundkit.o
SOUND_OBJS.macosx  = $(XM_DSP_DIR)/coreaudio.o
SOUND_OBJS.nto     = $(XM_DSP_DIR)/io-audio.o
SOUND_OBJS.irix    = $(XM_DSP_DIR)/irix.o
SOUND_OBJS.irix_al = $(XM_DSP_DIR)/irix_al.o
SOUND_OBJS.beos    =
SOUND_OBJS.generic =
#these need to be converted to plugins first
#SOUND_OBJS.aix     = $(XM_DSP_DIR)/aix.o


#### Cフラグ

CFLAGS   += -DUSE_SOUND
CXXFLAGS += -DUSE_SOUND

SOUND_INCDIR      = $(XMAME_DIR) $(SD_Q88_DIR) $(XM_SRC_DIR) $(XM_SOUND_DIR)
SOUND_CFLAGS      = -DPI=M_PI
#SOUND_CXXFLAGS    =

SOUND_INCDIR_UNIX = $(SD_X11_DIR) $(XM_UNIX_DIR) $(XM_SYSDEP_DIR) $(XM_DSP_DIR) $(XM_MIXER_DIR)


#### バージョン毎の設定

ifdef	X11_VERSION

#
# X11 バージョンでのサウンド設定
#

SOUND_OBJS	= $(SOUND_OBJS_BASE)		\
		  $(SD_X11_DIR)/audio.o		\
		  $(SOUND_OBJS_UNIX)		\
		  $(SOUND_OBJS.$(ARCH))


#
# サウンド有効時のコンパイルオプション
#	-DHAVE_SNPRINTF, -DHAVE_VSNPRINTF は不要のはず…

CFLAGS.linux      = -DSYSDEP_DSP_OSS -DSYSDEP_MIXER_OSS -DHAVE_SNPRINTF -DHAVE_VSNPRINTF
CFLAGS.freebsd    = -DSYSDEP_DSP_OSS -DSYSDEP_MIXER_OSS -DHAVE_SNPRINTF -DHAVE_VSNPRINTF
CFLAGS.netbsd     = -DSYSDEP_DSP_NETBSD -DHAVE_SNPRINTF -DHAVE_VSNPRINTF
CFLAGS.openbsd    = -DSYSDEP_DSP_NETBSD -DHAVE_SNPRINTF -DHAVE_VSNPRINTF
CFLAGS.solaris    = -DSYSDEP_DSP_SOLARIS -DSYSDEP_MIXER_SOLARIS
CFLAGS.next       = -DSYSDEP_DSP_SOUNDKIT -DBSD43
CFLAGS.macosx     = -DSYSDEP_DSP_COREAUDIO
CFLAGS.nto        = -DSYSDEP_DSP_ALSA -DSYSDEP_MIXER_ALSA
CFLAGS.irix       = -DSYSDEP_DSP_IRIX -DHAVE_SNPRINTF
CFLAGS.irix_al    = -DSYSDEP_DSP_IRIX -DHAVE_SNPRINTF
CFLAGS.beos       = `$(SDL_CONFIG) --cflags` -DSYSDEP_DSP_SDL
CFLAGS.generic    =
#these need to be converted to plugins first
#CFLAGS.aix        = -DSYSDEP_DSP_AIX -I/usr/include/UMS -I/usr/lpp/som/include

SOUND_INCDIR    += $(SOUND_INCDIR_UNIX)
SOUND_CFLAGS	+= -D__ARCH_$(ARCH) $(CFLAGS.$(ARCH))


#
# サウンド有効時のライブラリ指定
#	関係無いのも含まれていそう………

LIBS.solaris       = -lnsl -lsocket
LIBS.irix          = -laudio
LIBS.irix_al       = -laudio
LIBS.aix           = -lUMSobj
LIBS.next	   = -framework SoundKit
LIBS.macosx	   = -framework CoreAudio
#LIBS.macosx	   = -framework AudioUnit -framework CoreServices
#LIBS.openbsd       = -lossaudio
LIBS.nto	   = -lsocket -lasound
LIBS.beos          = `$(SDL_CONFIG) --libs`

SOUND_LIBS	= -lm $(LIBS.$(ARCH))


#
# 追加サウンドデバイス指定時の、追加設定
#

ifdef SOUND_ESOUND
SOUND_CFLAGS += -DSYSDEP_DSP_ESOUND `esd-config --cflags`
SOUND_LIBS   += `esd-config --libs`
SOUND_OBJS   += $(XM_DSP_DIR)/esound.o
endif

ifdef SOUND_ALSA
SOUND_CFLAGS += -DSYSDEP_DSP_ALSA -DSYSDEP_MIXER_ALSA
SOUND_LIBS   += -lasound
SOUND_OBJS   += $(XM_DSP_DIR)/alsa.o $(XM_MIXER_DIR)/alsa.o
endif

ifdef SOUND_ARTS_TEIRA
SOUND_CFLAGS += -DSYSDEP_DSP_ARTS_TEIRA `artsc-config --cflags`
SOUND_LIBS   += `artsc-config --libs`
SOUND_OBJS   += $(XM_DSP_DIR)/artssound.o
endif

ifdef SOUND_ARTS_SMOTEK
SOUND_CFLAGS += -DSYSDEP_DSP_ARTS_SMOTEK `artsc-config --cflags`
SOUND_LIBS   += `artsc-config --libs`
SOUND_OBJS   += $(XM_DSP_DIR)/arts.o
endif

ifdef SOUND_SDL
SOUND_CFLAGS += -DSYSDEP_DSP_SDL `$(SDL_CONFIG) --cflags`
SOUND_LIBS   += `$(SDL_CONFIG) --libs`
SOUND_OBJS   += $(XM_DSP_DIR)/sdl.o
endif

ifdef SOUND_WAVEOUT
SOUND_CFLAGS += -DSYSDEP_DSP_WAVEOUT
SOUND_OBJS   += $(XM_DSP_DIR)/waveout.o
endif



else
ifdef	SDL_VERSION

#
# SDL バージョンでのサウンド設定
#

SOUND_OBJS	= $(SOUND_OBJS_BASE)		\
		  $(SD_SDL_DIR)/audio.o		\
		  $(SD_SDL_DIR)/sdl.o

SOUND_INCDIR    += $(SD_SDL_DIR)
SOUND_CFLAGS	+= -DSYSDEP_DSP_SDL
#SOUND_CXXFLAGS	+=
SOUND_LIBS	= -lm

else
ifdef	MINI_VERSION

#
# MINI バージョンでのサウンド設定
#	コンパイル検証のみ。音は出ない

SOUND_OBJS	= $(SOUND_OBJS_BASE)	\
		  $(SYSDEPEND_DIR)/audio.o
#SOUND_INCDIR	+=
#SOUND_CFLAGS	+=
#SOUND_CXXFLAGS	+=
SOUND_LIBS	= -lm

endif
endif
endif




#### fmgen 指定時の設定

ifdef	USE_FMGEN

FMGEN_DIR	= $(SNDDRV_DIR)/fmgen
FMGEN_OBJ	= $(SD_Q88_DIR)/2203fmgen.o	\
		  $(SD_Q88_DIR)/2608fmgen.o	\
		  $(FMGEN_DIR)/fmgen.o		\
		  $(FMGEN_DIR)/fmtimer.o	\
		  $(FMGEN_DIR)/opna.o		\
		  $(FMGEN_DIR)/psg.o

CFLAGS		+= -DUSE_FMGEN
CXXFLAGS	+= -DUSE_FMGEN

SOUND_INCDIR    += $(FMGEN_DIR)
#SOUND_CFLAGS	+=
#SOUND_CXXFLAGS	+=

SOUND_OBJS	+= $(FMGEN_OBJ)

SOUND_LIBS	+= $(CXXLIBS)

endif

endif



#######################################################################
#
#######################################################################


ifdef	X11_VERSION
PROGRAM = quasi88-x11
else
ifdef	SDL_VERSION
PROGRAM = quasi88-sdl
else
ifdef	GTK3_VERSION
PROGRAM = quasi88-gtk3
else
ifdef	MINI_VERSION
PROGRAM = quasi88-mini
endif
endif
endif
endif



OSDEP_DIR =	osdepend
SYSDEP_DIR =	sysdepend

ifdef	X11_VERSION
OSDEPEND_DIR  = $(OSDEP_DIR)/unix
SYSDEPEND_DIR = $(SYSDEP_DIR)/x11
OBJECT =	$(SYSDEPEND_DIR)/graph.o	\
		$(SYSDEPEND_DIR)/wait.o		\
		$(SYSDEPEND_DIR)/event.o	\
		$(SYSDEPEND_DIR)/joystick.o	\
		$(SYSDEPEND_DIR)/main.o		\
		$(OSDEPEND_DIR)/file-op.o
else
ifdef	SDL_VERSION
OSDEPEND_DIR  = $(OSDEP_DIR)/unix
SYSDEPEND_DIR = $(SYSDEP_DIR)/sdl
OBJECT =	$(SYSDEPEND_DIR)/graph.o	\
		$(SYSDEPEND_DIR)/wait.o		\
		$(SYSDEPEND_DIR)/event.o	\
		$(SYSDEPEND_DIR)/main.o		\
		$(OSDEPEND_DIR)/file-op.o
else
ifdef	GTK3_VERSION
OSDEPEND_DIR  = $(OSDEP_DIR)/unix
SYSDEPEND_DIR = $(SYSDEP_DIR)/gtk3
OBJECT =	$(SYSDEPEND_DIR)/graph.o	\
		$(SYSDEPEND_DIR)/wait.o		\
		$(SYSDEPEND_DIR)/event.o	\
		$(SYSDEPEND_DIR)/main.o		\
		$(SYSDEPEND_DIR)/menubar.o	\
		$(OSDEPEND_DIR)/file-op.o
else
ifdef	MINI_VERSION
OSDEPEND_DIR  = $(OSDEP_DIR)/dummy
SYSDEPEND_DIR = $(SYSDEP_DIR)/mini
OBJECT =	$(SYSDEPEND_DIR)/graph.o	\
		$(SYSDEPEND_DIR)/wait.o		\
		$(SYSDEPEND_DIR)/event.o	\
		$(SYSDEPEND_DIR)/main.o		\
		$(OSDEPEND_DIR)/file-op.o
endif
endif
endif
endif



PC88_DIR	= pc88
SCREEN_DIR	= screen
TK_DIR		= tk
UI_DIR		= ui
SNDDRV_DIR	= snddrv
OBJECT +=	\
		alarm.o					\
		control.o				\
		debug.o					\
		emu.o					\
		fname.o					\
		getconf.o				\
		icon.o					\
		menu.o					\
		monitor.o				\
		pause.o					\
		quasi88.o				\
		utility.o				\
		$(PC88_DIR)/basic.o			\
		$(PC88_DIR)/crtcdmac.o			\
		$(PC88_DIR)/fdc.o			\
		$(PC88_DIR)/image.o			\
		$(PC88_DIR)/intr.o			\
		$(PC88_DIR)/keyboard.o			\
		$(PC88_DIR)/memory.o			\
		$(PC88_DIR)/pc88main.o			\
		$(PC88_DIR)/pc88sub.o			\
		$(PC88_DIR)/pio.o			\
		$(PC88_DIR)/romaji.o			\
		$(PC88_DIR)/soundbd.o			\
		$(PC88_DIR)/suspend.o			\
		$(PC88_DIR)/z80.o			\
		$(PC88_DIR)/z80-debug.o			\
		$(SCREEN_DIR)/color.o			\
		$(SCREEN_DIR)/menu-screen.o		\
		$(SCREEN_DIR)/putimage.o		\
		$(SCREEN_DIR)/screen.o			\
		$(SCREEN_DIR)/screen-8bpp.o		\
		$(SCREEN_DIR)/screen-16bpp.o		\
		$(SCREEN_DIR)/screen-32bpp.o		\
		$(SCREEN_DIR)/screen-snapshot.o		\
		$(SCREEN_DIR)/snapshot.o		\
		$(TK_DIR)/q8tk-draw.o			\
		$(TK_DIR)/q8tk-event.o			\
		$(TK_DIR)/q8tk-glib.o			\
		$(TK_DIR)/q8tk.o			\
		$(TK_DIR)/q8tk/q8tk-accel.o		\
		$(TK_DIR)/q8tk/q8tk-adjustment.o	\
		$(TK_DIR)/q8tk/q8tk-button.o		\
		$(TK_DIR)/q8tk/q8tk-combo.o		\
		$(TK_DIR)/q8tk/q8tk-common.o		\
		$(TK_DIR)/q8tk/q8tk-container.o		\
		$(TK_DIR)/q8tk/q8tk-entry.o		\
		$(TK_DIR)/q8tk/q8tk-frame.o		\
		$(TK_DIR)/q8tk/q8tk-fselection.o	\
		$(TK_DIR)/q8tk/q8tk-fviewer.o		\
		$(TK_DIR)/q8tk/q8tk-grab.o		\
		$(TK_DIR)/q8tk/q8tk-item.o		\
		$(TK_DIR)/q8tk/q8tk-label.o		\
		$(TK_DIR)/q8tk/q8tk-listbox.o		\
		$(TK_DIR)/q8tk/q8tk-misc.o		\
		$(TK_DIR)/q8tk/q8tk-notebook.o		\
		$(TK_DIR)/q8tk/q8tk-signal.o		\
		$(TK_DIR)/q8tk/q8tk-widget.o		\
		$(TK_DIR)/q8tk/q8tk-window.o		\
		$(UI_DIR)/diskchange-top.o		\
		$(UI_DIR)/menu-top.o			\
		$(UI_DIR)/pause-top.o			\
		$(UI_DIR)/quit-top.o			\
		$(UI_DIR)/reset-top.o			\
		$(UI_DIR)/speedup-top.o			\
		$(UI_DIR)/statusbar.o			\
		$(UI_DIR)/toolbar.o			\
		$(UI_DIR)/menu/menu-about.o		\
		$(UI_DIR)/menu/menu-common.o		\
		$(UI_DIR)/menu/menu-cpu.o		\
		$(UI_DIR)/menu/menu-disk.o		\
		$(UI_DIR)/menu/menu-graph.o		\
		$(UI_DIR)/menu/menu-key.o		\
		$(UI_DIR)/menu/menu-misc.o		\
		$(UI_DIR)/menu/menu-mouse.o		\
		$(UI_DIR)/menu/menu-reset.o		\
		$(UI_DIR)/menu/menu-tape.o		\
		$(UI_DIR)/menu/menu-volume.o		\
		$(SOUND_OBJS)

# ソースコードディレクトリ

SRCDIR	= ../src


# インクルードディレクトリ

INCDIR =	$(OSDEP_DIR)			\
		$(SYSDEP_DIR)			\
		$(OSDEPEND_DIR)			\
		$(SYSDEPEND_DIR)		\
		$(PC88_DIR)			\
		$(SCREEN_DIR)			\
		$(SCREEN_DIR)/func		\
		$(SCREEN_DIR)/func/macro	\
		$(TK_DIR)			\
		$(TK_DIR)/q8tk			\
		$(UI_DIR)			\
		$(UI_DIR)/menu			\
		$(SNDDRV_DIR)

INCLUDES =	-I$(SRCDIR) $(addprefix -I$(SRCDIR)/, $(INCDIR))
SOUND_INCLUDES =            $(addprefix -I$(SRCDIR)/, $(SOUND_INCDIR))


# 最終的な C/C++フラグ

CFLAGS +=	-DROM_DIR='"$(ROMDIR)"'		\
		-DDISK_DIR='"$(DISKDIR)"'	\
		-DTAPE_DIR='"$(TAPEDIR)"'	\
		-DSNAP_DIR='"$(SNAPDIR)"'	\
		-DSTATE_DIR='"$(STATEDIR)"'	\
		-DL_CFG_DIR='"$(LCFGDIR)"'	\
		-DG_CFG_DIR='"$(GCFGDIR)"'	\
		$(USEINLINE)			\
		-DCLIB_DECL=
CFLAGS :=	$(INCLUDES) $(CFLAGS)
CXXFLAGS +=	-DCLIB_DECL=
CXXFLAGS :=	$(INCLUDES) $(CXXFLAGS)

SOUND_CFLAGS +=		$(SOUND_INCLUDES)
SOUND_CXXFLAGS +=	$(SOUND_INCLUDES)

LIBS   += $(SOUND_LIBS)


# 中間ファイルディレクトリ

ifdef	X11_VERSION
OBJDIR		= obj-x11
else
ifdef	SDL_VERSION
OBJDIR		= obj-sdl
else
ifdef	GTK3_VERSION
OBJDIR		= obj-gtk3
else
ifdef	MINI_VERSION
OBJDIR		= obj-mini
endif
endif
endif
endif

OBJECTS		= $(addprefix $(OBJDIR)/, $(OBJECT))
DEPENDS		= $(patsubst %.o, %.d, $(OBJECTS))


###

all:		$(OBJDIR) $(PROGRAM)

$(OBJDIR):
		-mkdir -p $@

$(PROGRAM):	$(OBJECTS)
		$(LD) $(OBJECTS) $(LIBS) -o $(PROGRAM) 


$(OBJDIR)/sysdepend/mini/audio.o: $(SRCDIR)/sysdepend/mini/audio.c
		@-mkdir -p $(dir $@)
		$(CC) $(CFLAGS) $(SOUND_CFLAGS) $(DEBUG_CFLAGS) -o $@ -c $<


$(OBJDIR)/$(XMAME_DIR)/%.o: $(SRCDIR)/$(XMAME_DIR)/%.c
		@-mkdir -p $(dir $@)
		$(CC) $(CFLAGS) $(SOUND_CFLAGS) $(DEBUG_CFLAGS) -o $@ -c $<

$(OBJDIR)/$(XMAME_DIR)/%.o: $(SRCDIR)/$(XMAME_DIR)/%.m
		@-mkdir -p $(dir $@)
		$(CC) $(CFLAGS) $(SOUND_CFLAGS) $(DEBUG_CFLAGS) -o $@ -c $<

$(OBJDIR)/$(XMAME_DIR)/%.o: $(SRCDIR)/$(XMAME_DIR)/%.cpp
		@-mkdir -p $(dir $@)
		$(CXX) $(CXXFLAGS) $(SOUND_CXXFLAGS) $(DEBUG_CXXFLAGS) -o $@ -c $<

$(OBJDIR)/$(FMGEN_DIR)/%.o: $(SRCDIR)/$(FMGEN_DIR)/%.cpp
		@-mkdir -p $(dir $@)
		$(CXX) $(CXXFLAGS) $(SOUND_CXXFLAGS) $(DEBUG_CXXFLAGS) -o $@ -c $<


$(OBJDIR)/%.o: $(SRCDIR)/%.c
		@-mkdir -p $(dir $@)
		$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
		@-mkdir -p $(dir $@)
		$(CXX) $(CXXFLAGS) $(DEBUG_CXXFLAGS) -o $@ -c $<


$(OBJDIR)/sysdepend/mini/audio.d: $(SRCDIR)/sysdepend/mini/audio.c
		@echo create dependency-file $@
		@-mkdir -p $(dir $@)
		@$(CC) -MM $(CFLAGS) $(SOUND_CFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@


$(OBJDIR)/$(XMAME_DIR)/%.d: $(SRCDIR)/$(XMAME_DIR)/%.c
		@echo create dependency-file $@
		@-mkdir -p $(dir $@)
		@$(CC) -MM $(CFLAGS) $(SOUND_CFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@

$(OBJDIR)/$(XMAME_DIR)/%.d: $(SRCDIR)/$(XMAME_DIR)/%.m
		@echo create dependency-file $@
		@-mkdir -p $(dir $@)
		@$(CC) -MM $(CFLAGS) $(SOUND_CFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@

$(OBJDIR)/$(XMAME_DIR)/%.d: $(SRCDIR)/$(XMAME_DIR)/%.cpp
		@echo create dependency-file $@
		@-mkdir -p $(dir $@)
		@$(CXX) -MM $(CXXFLAGS) $(SOUND_CXXFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@

$(OBJDIR)/$(FMGEN_DIR)/%.d: $(SRCDIR)/$(FMGEN_DIR)/%.cpp
		@echo create dependency-file $@
		@-mkdir -p $(dir $@)
		@$(CXX) -MM $(CXXFLAGS) $(SOUND_CXXFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@


$(OBJDIR)/%.d: $(SRCDIR)/%.c
		@echo create dependency-file $@
		@-mkdir -p $(dir $@)
		@$(CC) -MM $(CFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
		@echo create dependency-file $@
		@-mkdir -p $(dir $@)
		@$(CXX) -MM $(CXXFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@


$(OBJDIR)/%.s: $(SRCDIR)/%.c
		@-mkdir -p $(dir $@)
		$(CC) $(CFLAGS) $(SOUND_CFLAGS) $(DEBUG_CFLAGS) -o $@ -S $<


clean:
		rm -rf $(OBJDIR) $(PROGRAM) $(PROGRAM).core

debug:
		@echo Makefile Debug Target is here.


#
# インストールに関する設定
#

install:
		@echo installing binaries under $(BINDIR)...
		@cp $(PROGRAM) $(BINDIR)/


#
#
#
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDS)
endif

.PHONY: all clean debug install
