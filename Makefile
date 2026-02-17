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

#######################################################################
# 基本設定
#######################################################################


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



# PC-8801のキーボードバグをエミュレートしたい場合は、
# 以下のコメントアウトを外して下さい。

# USE_KEYBOARD_BUG	= 1



#######################################################################
# サウンド関連の設定
#######################################################################

# MAME/XMAME ベースのサウンド出力を組み込まない場合、以下の行を
# コメントアウトして下さい。

USE_SOUND		= 1



# QUASI88 ver 0.6.3 以前にて使用していた、古いバージョンの MAME/XMAME の
# サウンド出力を使用したい場合は、以下のコメントアウトを外して下さい。
#	動作に必要なリソースが少なめとなります。

# USE_OLD_MAME_SOUND	= 1



#######################################################################
# fmgen (FM Sound Generator) の設定
#######################################################################

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
# SDL2ライブラリの設定
#######################################################################

# sdl2-config を実行するコマンドを指定してください。

SDL2_CONFIG	= sdl2-config



#######################################################################
# コンパイル関連の設定
#######################################################################

# C コンパイラの設定
#	gcc 、 clang などを指定してください。

CC	= cc


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



# コンパイラによっては、インライン関数を使うことが出来ます。
#	以下から、適切なものを一つだけ指定してください。

# どんなコンパイラでもOK
# USEINLINE	= '-DINLINE=static'

# GCC の場合
USEINLINE	= '-DINLINE=static __inline__'

# Intel C++ コンパイラの場合 (?)
# USEINLINE	= '-DINLINE=static inline'



# C++ コンパイラ関連の設定
#	fmgen を組み込まない場合、これらの設定は不要です。
#
#	g++ 、 clang++ などを指定してください。
#	環境によっては、$(CC) と同じ （ gcc 、 clang など） にします。

CXX	 = c++
CXXFLAGS = -O2
CXXLIBS	 = -lstdc++



# リンカの設定
#	C++ コンパイラを使う場合、環境によっては $(CXX) とする必要が
#	あるかもしれません。

LD	= $(CC) -Wl,-s
# LD	= $(CXX) -Wl,-s



#######################################################################
# インストールの設定
#######################################################################

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

# コメントアウトすると、倍サイズでの表示ができなくなる実験

SUPPORT_DOUBLE		= 1



#######################################################################
#
#######################################################################

#
# SDL2バージョンでの設定
#

CFLAGS   += `$(SDL2_CONFIG) --cflags`
CXXFLAGS += `$(SDL2_CONFIG) --cflags`
LIBS     += `$(SDL2_CONFIG) --libs`

CFLAGS += -DQUASI88_SDL2



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
SD_SDL_DIR	= $(XMAME_DIR)/quasi88/SDL

XM_SRC_DIR	= $(XMAME_DIR)/src
XM_SOUND_DIR	= $(XMAME_DIR)/src/sound


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


#### Cフラグ

CFLAGS   += -DUSE_SOUND
CXXFLAGS += -DUSE_SOUND

SOUND_INCDIR      = $(XMAME_DIR) $(SD_Q88_DIR) $(XM_SRC_DIR) $(XM_SOUND_DIR)
SOUND_CFLAGS      = -DPI=M_PI
#SOUND_CXXFLAGS    =


#
# SDL2 バージョンでのサウンド設定
#

SOUND_OBJS	= $(SOUND_OBJS_BASE)		\
		  $(SD_SDL_DIR)/audio.o		\
		  $(SD_SDL_DIR)/sdl.o

SOUND_INCDIR    += $(SD_SDL_DIR)
SOUND_CFLAGS	+= -DSYSDEP_DSP_SDL
#SOUND_CXXFLAGS	+=
SOUND_LIBS	= -lm




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


PROGRAM = quasi88

OSDEP_DIR =	osdepend
SYSDEP_DIR =	sysdepend
OSDEPEND_DIR  = $(OSDEP_DIR)/unix
SYSDEPEND_DIR = $(SYSDEP_DIR)/sdl2
OBJECT =	$(SYSDEPEND_DIR)/graph.o	\
		$(SYSDEPEND_DIR)/touchkey.o	\
		$(SYSDEPEND_DIR)/wait.o		\
		$(SYSDEPEND_DIR)/event.o	\
		$(SYSDEPEND_DIR)/main.o		\
		$(OSDEPEND_DIR)/file-op.o

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

SRCDIR	= src


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

OBJDIR		= obj

OBJECTS		= $(addprefix $(OBJDIR)/, $(OBJECT))
DEPENDS		= $(patsubst %.o, %.d, $(OBJECTS))


###

all:		$(OBJDIR) $(PROGRAM)

$(OBJDIR):
		-mkdir -p $@

$(PROGRAM):	$(OBJECTS)
		$(LD) $(OBJECTS) $(LIBS) -o $(PROGRAM) 


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


$(OBJDIR)/$(XMAME_DIR)/%.d: $(SRCDIR)/$(XMAME_DIR)/%.c
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
		@$(CC) -MM $(CXXFLAGS) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; [ -s $@ ] || rm -f $@


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
