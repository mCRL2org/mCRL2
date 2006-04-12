#
# The variable TREE_ROOT should be a relative path to the root of the source tree
#
ifndef TREE_ROOT
 TREE_ROOT                = ./
 MAXIMUM_REVISION         = utility/maximum_revision
 BJAM                     = boost/tools/jam/bin/bjam
endif

REVISION_HEADER = $(TREE_ROOT)src/mcrl2_revision.h

# Add revision number
ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)

revision: $(REVISION_HEADER)

*.d *.dpp: $(REVISION_HEADER)

# Bootstrap bjam
$(BJAM):
	$(MAKE) -C $(TREE_ROOT)/boost bjam

# Build maximum_revision
$(REVISION_HEADER): $(BJAM)
	@cd $(TREE_ROOT); $(BJAM)

endif
