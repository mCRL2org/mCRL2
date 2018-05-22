Git repository
###################################
This page describes the workflow for people with write-access to our 
`repository <https://github.com/mCRL2org/mCRL2>`_.

The `Centralized Workflow` as described in the following 
`tutorial <https://www.atlassian.com/git/tutorials/comparing-workflows>`_ was 
chosen as our workflow. This means that the `master` branch contains all the 
latest changes and preserves a linear history similar to SVN. Every commit 
should first be rebased on top of the latest `master` branch before it
can be pushed. The documentation for rebasing can be found `here <https://git-scm.com/book/en/v2/Git-Branching-Rebasing>`_.

For information on merging pull requests see the corresponding
Github `documentation <https://help.github.com/articles/merging-a-pull-request/>`_.
To preserve a linear history every pull request should be merged by means of a rebase.

When a release is desired a new branch named `release-<version>` is created 
based of the master branch. This branch is then exclusively used for changes 
relevant to that release. When this branch is deemed stable enough for an actual 
release a tag is added to it after which a binary release is made from this 
version and published online.
