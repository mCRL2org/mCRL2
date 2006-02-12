#
# The variable TREE_ROOT should be a relative path to the root of the source tree
#
ifndef TREE_ROOT
  $(error TREE_ROOT must be a relative path to the root of the source tree)
endif

# Add revision number (if not building a source distribution)
ifndef SOURCE_DISTRIBUTION
ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)
$(TREE_ROOT)revision: $(MAXIMUM_REVISION)
	@cd $(TREE_ROOT); echo "REVISION := -DREVISION=$$(utility/maximum_revision)" > revision

*.o: $(TREE_ROOT)revision

# Bootstrap bjam
$(BJAM):
	$(MAKE) -C $(TREE_ROOT)src/boost bjam

# Build maximum_revision
$(MAXIMUM_REVISION): $(BJAM)
	@cd $(dir $(MAXIMUM_REVISION)); $(BJAM) --v2 release

# This makes make reload, such that REVISION is initialised properly
-include $(TREE_ROOT)revision

# This is present for viewCVS generated tarballs that do not include revision
ifndef REVISION
REVISION := -DREVISION=0
endif

endif
endif
