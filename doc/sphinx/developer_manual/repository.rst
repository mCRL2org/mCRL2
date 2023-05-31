.. _repository:

Git workflow
###################################
This page describes the workflow for people with write-access to our 
`repository <https://github.com/mCRL2org/mCRL2>`__.

A workflow describes the procedures to follow when publishing the changes made to
the files in the repository to other developers. In Git there is a difference between
recording changes and pushing them to the public. A single record is called a commit
which consists of a message and the author (you). The commit message describes the reason
for a change, it should consist of a summary on the first line followed by a 
detailed description starting on the third line if necessary. For a detailed
description on committing changes see the following `documentation <https://git-scm.com/docs/git-commit>`__.

The next step is to publish the changes so that they are accessible to other
developers. For this step the `Centralized Workflow` as described in the following 
`tutorial <https://www.atlassian.com/git/tutorials/comparing-workflows>`_ was 
chosen as our workflow. In this workflow the `master` branch contains all the 
latest changes and preserves a linear history similar to SVN. Every commit 
should first be rebased on top of the latest `master` branch before it
can be pushed. The documentation for rebasing can be found `here <https://git-scm.com/book/en/v2/Git-Branching-Rebasing>`__.

To contribute changes without write-access the contributors are required to make 
a pull request. This is a request to `pull` in the changes from their mirror repository, 
which is referred to as a `fork` by Github. These pull requests must be reviewed
and approved by one of the developers with write-access. For information on merging 
pull requests see the corresponding Github `documentation <https://help.github.com/articles/merging-a-pull-request/>`__.
To preserve a linear history every pull request should be merged by means of a rebase.

Finally, the repository can also be conveniently used to work on release candidates
in a separate branch. Whenever a release is desired a new branch named `release-<version>` is created 
based of the `master` branch. This branch is then exclusively used to record changes 
relevant to that release. When this branch is deemed stable enough for an actual 
release a tag is added to it. After this step a binary release is made from this 
branch and published online.
