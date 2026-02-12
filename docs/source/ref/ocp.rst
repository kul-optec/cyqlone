Optimal Control Problems
========================

Problems are usually formulated using :cpp:struct:`cyqlone::LinearOCPStorage`.
To pass them to the Cyqlone solver, they need to be converted to :cpp:struct:`cyqlone::CyqloneStorage`
format, where the initial state is eliminated. For testing, it can be helpful to convert the
problem to a sparse QP format, which is supported by :cpp:struct:`cyqlone::qpalm::LinearOCPSparseQP`.

.. doxygenstruct:: cyqlone::LinearOCPStorage
   :no-link:
   :members:
   :undoc-members:
.. doxygenstruct:: cyqlone::CyqloneStorage
   :no-link:
   :members:
   :undoc-members:
.. doxygenstruct:: cyqlone::qpalm::LinearOCPSparseQP
   :no-link:
   :members:
   :undoc-members:
