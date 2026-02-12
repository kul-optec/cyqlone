.. image:: https://img.shields.io/badge/arXiv-Preprint-b31b1b
   :target: https://arxiv.org/abs/2512.09058
   :alt: arXiv Preprint

.. image:: https://github.com/kul-optec/cyqlone/actions/workflows/linux.yml/badge.svg
   :target: https://github.com/kul-optec/cyqlone/actions/workflows/linux.yml
   :alt: CI: Linux

.. image:: https://img.shields.io/pypi/dm/cyqlone?label=PyPI&logo=python
   :target: https://pypi.org/project/cyqlone
   :alt: PyPI Downloads


cyqlone
=======

Fast, **parallel and vectorized solver** for linear systems with **optimal control structure**.

Also includes an implementation of the **CyQPALM** solver that uses the Cyqlone linear solver as a
backend, with parallel factorization update routines to handle active set changes and a parallel
exact line search.

Preprint
--------

The paper describing the algorithms in this repository can be found on arXiv:  
`https://arxiv.org/abs/2512.09058 <https://arxiv.org/abs/2512.09058>`_

.. code-block:: bibtex

   @misc{pas_cyqlone_2025,
      title = {{Cyqlone}: {A} {Parallel}, {High}-{Performance} {Linear} {Solver} for {Optimal} {Control}},
      url = {http://arxiv.org/abs/2512.09058},
      doi = {10.48550/arXiv.2512.09058},
      publisher = {arXiv},
      author = {Pas, Pieter and Patrinos, Panagiotis},
      month = dec,
      year = {2025}
   }

Content
-------

.. toctree::
   :maxdepth: 1

   python.rst
   cpp.rst
