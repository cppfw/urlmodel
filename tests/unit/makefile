include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs := $(call prorab-src-dir, src)

this_u_urlmodel_lib := ../../src/out/$(c)/liburlmodel.so

this_cxxflags += -I ../../src

this_ldlibs += -ltst -lutki $(this_u_urlmodel_lib)

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_u_urlmodel_lib)))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(dir $(this_u_urlmodel_lib))
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))