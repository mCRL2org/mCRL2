#
# The variable TREE_ROOT should be a relative path to the root of the source tree
#
ifndef TREE_ROOT
  $(error TREE_ROOT must be a relative path to the root of the source tree)
endif

# Add revision number (if not building a source distribution)
ifndef SOURCE_DISTRIBUTION
ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)
MAXIMUM_REVISION := $(TREE_ROOT)/utility/maximum_revision

revision: $(MAXIMUM_REVISION)
	@echo "REVISION := -DREVISION=$$($(MAXIMUM_REVISION))" > revision

$(MAXIMUM_REVISION):
	$(MAKE) -C $(dir $(MAXIMUM_REVISION)) maximum_revision

*.o *.d *.dpp: revision

# This makes make reload, such that REVISION is initialised properly
-include revision

REVISION := -DREVISION=$(shell $(MAXIMUM_REVISION))

endif
endif
