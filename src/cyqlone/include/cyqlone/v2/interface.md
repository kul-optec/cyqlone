# Interface between QPALM and its backend solvers

## Data representation

- `initalize_var_vec(...)`: allocate the data structure for representing the optimization variables
- `initialize_eq_constr_vec(...)`: allocate the data structure for representing the equality constraints values
- `initialize_active_set(...)`: allocate the data structure for representing the active set
- `initialize_ineq_constr_vec(...)`: allocate the data structure for representing the inequality constraints values

## Initial guess

- `initial_variables(...)`: get the initial guess for the optimization variables
- `initial_multipliers_eq(...)`: get the initial guess for the equality constraint multipliers
- `initial_multipliers_ineq(...)`: get the initial guess for the inequality constraint multipliers

## Linear algebra operations

- `set_constant(...)`: set all elements of a vector to a constant value
- `norm_squared(...)`: compute the squared Euclidean norm of a vector
- `norm_inf(...)`: compute the infinity norm of a vector
- `dot(...)`: compute the dot product between two vectors
- `xcopy(...)`: copy the contents of one vector to another
- `xaxpy(...)`: perform the operation y = αx + y
- `scale(...)`: multiply a vector by a scalar

## Linear solver

- `solve(x, grad, Mᵀλ, Aᵀŷ, Mxb, S, Σ, active_set) ↦ (d, ξ, Ad, Δλ, MᵀΔλ)`: solve the KKT system

## Factorization updates

- `active_set_change(S, Σ, active_set, active_set_old)`: perform a factorization update when the active set changes
- `update_penalty_changed(Σ)`: TODO: perform a factorization update when the penalty parameter changes

## Problem functions

- `eq_constr_resid(x) ↦ Mxb`: compute the residual of the equality constraints Mx + b
- `mat_vec_A(x) ↦ Ax`: compute the value of the inequality constraints Ax
- `f_grad_f(x) ↦ (f, grad_f)`: compute the objective function value and its gradient
- `mat_vec_MT(λ) ↦ Mᵀλ`: compute the product with the transpose of the equality constraint matrix Mᵀ
- `mat_vec_AT(y) ↦ Aᵀy`: compute the product with the transpose of the inequality constraint matrix Aᵀ
- `calc_ŷ_Aᵀŷ(Ax, Σ, y) ↦ (ŷ, Aᵀŷ, active_set)`: evaluate multiple quantities involving the inequality constraints
- `grad_f_remove_regularization(S, x, x_outer, grad_f) ↦ grad_f`: remove the regularization term from the gradient of the objective function
- `recompute_inner(S, x_outer, x, λ) ↦ x`: recompute the matrix-vector products for the inner solver

## Stopping criteria

- `unscaled_lagr_grad_norm(grad_f, Mᵀλ, Aᵀy) ↦ norm`: compute the infinity norm of the unscaled augmented Lagrangian gradient
- `unscaled_eq_constr_viol(Mxb) ↦ violation`: compute the infinity norm of the unscaled violation of the equality constraints

## Line search

- `line_search(...)`: perform an (exact) line search along a given search direction

## QPALM-specific operations

- `boost_regularization(S, S_boosted)`: boost the regularization parameter (when close to conergence)
- `project_multipliers_ineq(y) ↦ y_projected`: project the inequality constraint multipliers onto the proper orthant (for one-sided constraints)
