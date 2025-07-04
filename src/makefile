include prorab.mk
include prorab-test.mk
include prorab-clang-format.mk
include prorab-license.mk

# =====================================
# ============== library ==============

this_name := bedsidemon

$(eval $(call prorab-config, ../config))

this__root_dir_rel := ../

this__version_cpp := $(this_out_dir)version.cpp

this_srcs += $(this__version_cpp)
this_srcs += $(call prorab-src-dir, $(this_name))

ifneq ($(config), emsc)
    this_srcs += $(call prorab-src-dir, sensors)
endif

this_srcs := $(filter-out %main.cpp,$(this_srcs))

this_no_install := true
this_static_lib_only := true

$(eval $(prorab-build-lib))

define this__rules
$(abspath $(d)$(this__version_cpp)): $(abspath $(d)$(this__root_dir_rel)build/debian/changelog)
$(.RECIPEPREFIX)$(a)mkdir -p $$(dir $$@)
$(.RECIPEPREFIX)$(a)(cd $(d)$(this__root_dir_rel); echo "extern const char* const program_version = \"$$$$(myci-deb-version.sh)\";" > $$@)
endef
$(eval $(this__rules))

# ================================================
# ============== format and license ==============
this_src_dir := .
$(eval $(prorab-clang-format))
this_license_file := ../LICENSE
$(eval $(prorab-license))

# ===============================================
# ============== install resources ==============
define this_rules
install::
$(.RECIPEPREFIX)$(a)for i in $(patsubst $(d)../res/%,/%,$(shell find $(d)../res -type f -name "*")); do \
		install -d $(DESTDIR)$(PREFIX)/share/$(this_name)$$$${i%/*}; \
		install -m 644 $(d)../res$$$$i $(DESTDIR)$(PREFIX)/share/$(this_name)$$$$i; \
	done
endef
$(eval $(this_rules))

# =========================================
# ============== application ==============
define bedsidemon_rules
    $$(eval $$(prorab-clear-this-vars))

    this_name := bedsidemon-$1-$2

    $$(eval $$(call prorab-config, ../config))

    this_srcs += bedsidemon/main.cpp

    this_ldlibs += -pthread
    this_ldflags += -rdynamic

    this_ldlibs += $$(this_out_dir)libbedsidemon.a

    ifeq ($(config), emsc)
        this_name := $$(this_name).html

        this_ldflags += -sWASM=1
        this_ldflags += -sSINGLE_FILE
        this_ldflags += --embed-file $(d)../res@res
    else
        this_ldlibs += -l clargs
        this_ldlibs += -l nitki
        this_ldlibs += -l opros
    endif

    this_ldlibs += -l papki
    this_ldlibs += -l tml
    this_ldlibs += -l ruis
    this_ldlibs += -l utki
    this_ldlibs += -l m

    this_ldlibs += -l ruisapp-$1-$2

    $$(eval $$(prorab-build-app))

    this_run_name := app-$1-$2
    this_test_cmd := $$(prorab_this_name) --window --res-path=../res
    this_test_deps := $$(prorab_this_name)
    this_test_ld_path := $$(prorab_space)
    $$(eval $$(prorab-run))
endef

ifneq ($(config), emsc)
    # non-Emscripten

    # $(eval $(call bedsidemon_rules,opengl,xorg))
    $(eval $(call bedsidemon_rules,opengles,xorg))
    # $(eval $(call bedsidemon_rules,opengl,wayland))
    $(eval $(call bedsidemon_rules,opengles,wayland))
else
    # Emscripten

    $(eval $(call bedsidemon_rules,opengles,sdl))
endif

# clean up
bedsidemon_rules :=


# ==================================================
# = targets for building with conan and emscripten =

define this__rules
conan_install:
$(.RECIPEPREFIX)@echo install conan dependencies
$(.RECIPEPREFIX)$(a)conan install $(d)../build/emscripten --update --profile:host $(d)../build/emscripten/conan.profile --build=missing --output-folder=$(d)out/emsc --deployer-package="*"
endef
$(eval $(this__rules))
