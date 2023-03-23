.. _developer_contributing:

******************
Contributing Guide
******************

|FMDT| code versioning is achieved thanks to Git. This section details how new
contributions are integrated to the repository. There are two possible way to
contribute depending on if your are a external contributor or if your are an
inner contributor, see the next sections.

.. important:: The |FMDT| project exposes two mains protected branches:
               ``master`` and ``develop``. The merge/pull requests are only
               accepted in the ``develop`` branch. In other words, all
               merge/pull requests targeting the ``master`` branch will be
               rejected.

.. danger:: **Please read the coding conventions first** in
            :numref:`developer_conventions`. Contributions that do not
            follow the coding and naming conventions will not be accepted!

Inner Contributions on GitLab
"""""""""""""""""""""""""""""

This is the inner workflow for people that have access to the private GitLab
repository. In this repository, the ``master`` and ``develop`` branches are
public because they are automatically mirrored on the public GitHub repository.
By definitions, the other branches are private.

The way to contribute is to create a new branch from the ``develop`` to develop
a new feature (lets call this a feature branch). When the feature branch is
mature enough (and when it passes the |CI| pipeline). The developer should send
a **merge request** (|MR|) from the feature branch into the ``develop`` branch.
To send a |MR| in GitLab, you need to do it from the GitLab web
interface. If you don't know how to do that, you can refer to the official
documentation here: https://docs.gitlab.com/ee/user/project/merge_requests/.

Once your |MR| is submitted, your code will be reviewed and accepted later if
it matches the requirements.

External Contributions on GitHub
""""""""""""""""""""""""""""""""

External contributions are also more than welcome. Everyone can access and clone
the public |FMDT| repository from GitHub (https://github.com/alsoc/fmdt).

The way to contribute is to submit |PR| to the ``develop`` branch. This can be
done from the GitHub web interface. If you don't know how to do that, you can
refer to the official documentation here:
https://docs.github.com/en/pull-requests/.

Once your |PR| is submitted, your code will be reviewed and accepted later if
it matches the requirements.

Workflow Git
""""""""""""

Every contributions are firstly merged in the ``develop`` branch. When we
consider that the current state of the ``develop`` branch is stable enough, a
versioning tag (for instance ``v1.0.0``) is added to a specific commit in the
``develop`` branch, then the ``develop`` branch is merge in the ``master``
branch.
