include prorab.mk
include prorab-test.mk

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

this_ldlibs += -lruisapp-opengl -lnitki -lruis -lutki -lm

$(eval $(prorab-build-app))

this_run_name := app
this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := ../src/out/$(c)
$(eval $(prorab-run))
