# Examples {#examples}

- [Solving a block tridiagonal system](@ref solve-block-tridiagonal.cpp)

## 🚧 Work in progress 🚧

@example solve-block-tridiagonal.cpp
@par Example demonstrating how to solve a block tridiagonal system in parallel

The system has the form
@f[
\begin{pmatrix}
M_0 & \tp{K_0} & 0 & 0 & \cdots & K_{N-1} \\
K_0 & M_1 & \tp{K_1} & 0 & \cdots & 0 \\
0 & K_1 & M_2 & \tp{K_2} & \cdots & 0 \\
0 & 0 & K_2 & M_3 & \cdots & 0 \\
\vdots & \vdots & \vdots & \vdots & \ddots & \vdots \\
\tp{K_{N-1}} & 0 & 0 & 0 & \cdots & M_{N-1}
\end{pmatrix}
\begin{pmatrix}
x^0 \\ x^1 \\ x^2 \\ x^3 \\ \vdots \\ x^{N-1}
\end{pmatrix}
=
\begin{pmatrix}
b^0 \\ b^1 \\ b^2 \\ b^3 \\ \vdots \\ b^{N-1}
\end{pmatrix}
@f]

We use @ref cyqlone::TricyqleSolver to solve the system in parallel. Since this solver uses
vectorization using batched linear algebra operations and partitions the matrices and vectors
across multiple threads, each thread needs to pack its local portion of the system into batches
of matrices and vectors. In this example, we prepare the system in the batches of matrices
`M` and `K`, but in practice, it is often more efficient to prepare the system in parallel, writing
the local blocks into the solver's workspace directly.

The solution produced by this example is stored in a `.mat` file, which can be loaded and verified
using the Python script @ref solve-block-tridiagonal.py.

@example solve-block-tridiagonal.py
