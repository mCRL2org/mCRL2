# The variable SOURCE_ROOT should be a relative path to the root of the source tree
ifndef SOURCE_ROOT
  $(error SOURCE_ROOT must be a relative path to the root of the source tree)
endif

# Add revision number (if not building a source distribution)
ifndef SOURCE_DISTRIBUTION
ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)
MAXIMUM_REVISION := $(SOURCE_ROOT)/utility/maximum_revision

revision: $(MAXIMUM_REVISION)
	@echo "CPPFLAGS += -DREVISION=$$($(MAXIMUM_REVISION))" > revision

$(MAXIMUM_REVISION):
	$(MAKE) -C $(dir $(MAXIMUM_REVISION)) maximum_revision

%.o: revision
endif
endif

-include revision

