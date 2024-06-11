include prorab.mk
include prorab-test.mk
include prorab-clang-format.mk
include prorab-license.mk

# =====================================
# ============== library ==============

this_name := bedsidemon

$(eval $(call prorab-config, ../config))

this_srcs += $(call prorab-src-dir, bedsidemon)

this_no_install := true
this_static_lib_only := true

$(eval $(prorab-build-lib))

# =========================================
# ============== application ==============

define bedsidemon_rules
    $$(eval $$(prorab-clear-this-vars))

    this_name := bedsidemon-$1-$2

    $$(eval $$(call prorab-config, ../config))

    this_srcs += main.cpp

    this_ldlibs += -pthread
    this_ldflags += -rdynamic

    this_ldlibs += out/$$(c)/libbedsidemon.a

    this_ldlibs += -l papki
    this_ldlibs += -l clargs
    this_ldlibs += -l nitki
    this_ldlibs += -l opros
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

# $(eval $(call bedsidemon_rules,opengl,xorg))
$(eval $(call bedsidemon_rules,opengles,xorg))
# $(eval $(call bedsidemon_rules,opengl,wayland))
$(eval $(call bedsidemon_rules,opengles,wayland))

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
