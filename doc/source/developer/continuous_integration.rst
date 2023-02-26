.. _developer_continuous_integration:

**********************
Continuous Integration
**********************

A |CI| pipeline is setup in the private GitLab repository. It is composed of
4 stages:

#. Static analysis: for now there is only one job in the stage that compiles the
   documentation.
#. Build: this stage compiles |FMDT| on various compilers and with various
   compiler definitions.
#. Test: regression tests and memory leaks tests are performed.
#. Coverage: the code coverage of the regression tests is computed.

The |CI| pipeline is triggered after each ``push`` on the GitLab repository.
The jobs are executed on runners hosted by the LIP6 laboratory.
The jobs can easily be deployed thanks to the use of Docker images.
The public |AFF3CT| container registry is used
(https://gitlab.com/aff3ct/aff3ct/container_registry).
