include prorab.mk
include prorab-test.mk
include prorab-clang-format.mk
include prorab-license.mk

this_name := bedsidemon

$(eval $(call prorab-config, config))

this_srcs += $(call prorab-src-dir, src)

ifeq ($(os),windows)
    this_ldlibs += -lmingw32 # these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs += -lglew32 -lopengl32 -lz -lfreetype -mwindows
else ifeq ($(os),macosx)
    this_ldlibs += -lGLEW -framework OpenGL -framework Cocoa -lfreetype
    this_ldflags += -rdynamic
else ifeq ($(os),linux)
    this_ldlibs += -pthread
    this_ldflags += -rdynamic
endif

this_ldlibs += -lruisapp-opengles -lnitki -lopros -ltreeml -lruis -lutki -lm

this_ldlibs += -lGLESv2

$(eval $(prorab-build-app))

this_run_name := app
this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := ../src/out/$(c)
$(eval $(prorab-run))

this_src_dir := src
$(eval $(prorab-clang-format))
this_license_file := LICENSE
$(eval $(prorab-license))
