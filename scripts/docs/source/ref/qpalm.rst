QPALM
=====

Solver
------

.. doxygenclass:: cyqlone::qpalm::Solver
   :no-link:
   :members:
   :undoc-members:

Settings
--------

.. doxygenstruct:: cyqlone::qpalm::Settings
   :no-link:
   :members:
   :undoc-members:

Statistics
----------

.. doxygenstruct:: cyqlone::qpalm::SolverStats
   :no-link:
   :members:
   :undoc-members:
.. doxygenstruct:: cyqlone::qpalm::DetailedStats
   :no-link:
   :members:
   :undoc-members:

Backends
--------

.. doxygenstruct:: cyqlone::qpalm::unique_CyqloneBackend
   :no-link:
   :members:
   :undoc-members:
.. doxygenfunction:: cyqlone::qpalm::make_qpalm_cyqlone_backend
   :no-link:
.. doxygenfunction:: cyqlone::qpalm::update_qpalm_cyqlone_backend(CyqloneBackend<VL, DefaultOrder> &backend, const CyqloneStorage<real_t> &ocp)
   :no-link:
.. doxygenfunction:: cyqlone::qpalm::update_qpalm_cyqlone_backend(CyqloneBackend<VL, DefaultOrder> &backend, const LinearOCPStorage &ocp)
   :no-link:
