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
	@echo "CPPFLAGS += -DREVISION=$$($(MAXIMUM_REVISION))" > revision

$(MAXIMUM_REVISION):
	$(MAKE) -C $(dir $(MAXIMUM_REVISION)) maximum_revision

*.o: revision

-include revision

ifdef REVISION
REVISION := -DREVISION=$(shell $(MAXIMUM_REVISION))

CPPFLAGS += $(REVISION)
endif
endif
endif
