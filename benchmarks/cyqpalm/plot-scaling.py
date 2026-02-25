#!/usr/bin/env python3
from pathlib import Path
import re
import matplotlib

matplotlib.use("Agg")  # Use non-interactive backend before importing pyplot
import matplotlib.pyplot as plt
import polars as pl
import json
import click

METRIC_PRETTY = {
    "time": "total solver run time",
    "time_per_iter": "time per iteration",
    "num_iter": "number of iterations",
    "num_outer_iter": "number of outer iterations",
}

METRIC_LABELS = {
    "time": "Time [ms]",
    "time_per_iter": "Time per iteration [ms]",
    "num_iter": "Number of iterations",
    "num_outer_iter": "Number of outer iterations",
}

IGNORE_KEYS = [
    "name",
    "family_index",
    "per_family_instance_index",
    "run_name",
    "run_type",
    "repetitions",
    "threads",
    "aggregate_unit",
    "iterations",
]


def parse_benchmark_name(run_name: str) -> tuple[str, str]:
    parts = run_name.split("@", 1)
    if len(parts) != 2:
        raise ValueError(f"Cannot parse: {run_name}")
    return parts[0], parts[1].split("/")[0]


def load_data(file: str) -> dict[str, pl.DataFrame]:
    with open(file) as f:
        benchmarks = json.load(f)["benchmarks"]
    records = []
    for bench in benchmarks:
        if bench.get("run_type") != "aggregate":
            continue
        problem_id, solver_id = parse_benchmark_name(bench["run_name"])
        record = {"problem_id": problem_id, "solver_id": solver_id} | {
            k: v
            for k, v in bench.items()
            if k not in IGNORE_KEYS and isinstance(v, (int, float, str))
        }
        records.append(record)

    df = pl.DataFrame(records)

    columns_to_add = []
    columns_to_add.append(pl.col("real_time").alias("time"))
    # Handle num_iter column mapping
    iter_cols = [c for c in ["inner_iter", "iter", "num_iter"] if c in df.columns]
    columns_to_add.append(pl.coalesce(iter_cols).alias("num_iter"))
    # Handle num_outer_iter column mapping
    if "outer_iter" in df.columns:
        columns_to_add.append(pl.col("outer_iter").alias("num_outer_iter"))
    df = df.with_columns(columns_to_add)
    # Apply time unit conversions
    df = df.with_columns(
        [
            pl.when(pl.col("time_unit") == "ns")
            .then(pl.col("time") * 1e-6)
            .when(pl.col("time_unit") == "us")
            .then(pl.col("time") * 1e-3)
            .when(pl.col("time_unit") == "s")
            .then(pl.col("time") * 1e3)
            .otherwise(pl.col("time"))
            .alias("time"),
            pl.lit("ms").alias("time_unit"),
        ]
    )
    # Calculate time per iteration
    df = df.with_columns([(pl.col("time") / pl.col("num_iter")).alias("time_per_iter")])

    # Group by solver_id and return dictionary
    return {
        solver[0]: group.sort("problem_id")
        for solver, group in df.group_by("solver_id", maintain_order=True)
    }


def print_stats(all_results: dict[str, pl.DataFrame], aggregate: str):
    for solver, df in all_results.items():
        res = df.filter(pl.col("aggregate_name") == aggregate)
        tot, failed = len(res), res.filter(pl.col("success") != 1).height
        print(f"{solver}: {tot - failed}/{tot} successful runs")


def get_solver_data(all_results: dict[str, pl.DataFrame], solver: str) -> pl.DataFrame:
    if solver not in all_results:
        raise ValueError(f"Solver '{solver}' not found. Available: {list(all_results.keys())}")
    return all_results[solver]


def format_arg(arg: str) -> str | None:
    arg = arg.strip()
    if m := re.match(r"([pv])=(\d+)", arg):
        return f"${m.group(1)}$={m.group(2)}"
    return {"cm": None, "rm": None, "zero": None, "shift": "warm", "upd=0": "no updates"}.get(arg)


def format_solver_name(solver: str) -> str:
    if not (m := re.match(r"(?P<name>\w+)\((?P<args>.*)\)", solver)):
        return solver
    name, args = m.group("name"), m.group("args")
    name = {"cyqlone": "CyQPALM", "hpipm": "HPIPM"}.get(name, name)
    fmt_args = list(filter(None, (format_arg(arg) for arg in args.split(","))))
    if not fmt_args:
        return name
    return name + " (" + ", ".join(fmt_args) + ")"


def create_scaling_plot(
    all_results: dict[str, pl.DataFrame],
    metric: str,
    solvers: list[str],
    figsize: tuple[float, float],
    aggregate: str,
    ylim: tuple[float, float] | None,
    xvar: str = "N",
    log: bool = False,
):
    plot_data = []
    if not solvers:
        solvers = list(all_results.keys())
    for solver in solvers:
        if solver.startswith("cyqlone(p=1,v=1"):
            continue  # Cyqlone without parallelization or vectorization is not very relevant
        df = get_solver_data(all_results, solver)
        df = df.filter(pl.col("aggregate_name") == aggregate)
        df = df.with_columns(
            [
                pl.col("problem_id").str.extract(r"N=(\d+)", 1).cast(pl.Int64).alias("N"),
                pl.col("problem_id").str.extract(r"M=(\d+)", 1).cast(pl.Int64).alias("M"),
            ]
        )
        by_n = df.group_by(xvar).agg(pl.col(metric).mean()).sort(xvar)
        plot_data.append((solver, by_n))

    all_values = [v for _, df_result in plot_data for v in df_result[metric].to_list()]
    max_y = ylim[1] if ylim else max(all_values, default=0)

    ylabel = METRIC_LABELS.get(metric, metric.replace("_", " ").capitalize())
    scale = 1.0
    if "[ms]" in ylabel and max_y <= 1.0:
        ylabel = ylabel.replace("[ms]", r"[$\mu$s]")
        scale = 1e3

    fig, ax = plt.subplots(figsize=figsize)
    legend_data = []
    min_x, max_x = 999999, 0
    for solver, df_result in plot_data:
        single_threaded = re.search(r"p=1\b", solver) is not None or "hpipm" in solver
        four_threaded = re.search(r"p=4\b", solver) is not None
        no_vectorization = re.search(r"v=1\b", solver) is not None
        line = ax.plot(
            df_result[xvar].to_numpy(),
            df_result[metric].to_numpy() * scale,
            marker="|" if four_threaded else "o",
            linewidth=1.25,
            linestyle=":" if no_vectorization else "-",
            markersize=5 if four_threaded else 3 if single_threaded else 2.5,
            mfc="white" if single_threaded else None,
        )
        legend_data.append((line[0], format_solver_name(solver)))
        min_x, max_x = min(min_x, df_result[xvar].min()), max(max_x, df_result[xvar].max())

    if log:
        ax.set_yscale("log")
    xlabel = {"N": "Horizon length $N$", "M": "Number of masses $M$"}.get(xvar, xvar)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    title = METRIC_PRETTY.get(metric, metric.replace("_", " "))
    label = {"N": "horizon lengths $N$", "M": "numbers of masses $M$"}.get(xvar, xvar)
    ax.set_title(f"{title.capitalize()}\nfor increasing {label}")
    ax.grid(True, alpha=0.3)
    legend_data.sort(key=lambda x: x[0].get_ydata()[-1], reverse=True)
    ax.legend(*zip(*legend_data), loc="best", fontsize=9, framealpha=0.7)
    if ylim:
        ax.set_ylim(ylim[0] * scale, ylim[1] * scale)
    elif not log:
        ax.set_ylim(bottom=0)
    ax.set_xlim(left=0, right=max_x + min_x)

    plt.tight_layout()
    return fig


@click.command()
@click.argument("file", type=click.Path(exists=True))
@click.option("--output", required=True, help="Output PDF filename")
@click.option("--solver", "solvers", multiple=True, help="Solver IDs")
@click.option("--metric", default="time_per_iter", help="Metric to plot")
@click.option("--figsize", default="6,5", help="Figure size as 'width,height'")
@click.option("--aggregate", default="min", help="Aggregate: min, mean, median")
@click.option("--ylim", help="Y-axis limits as 'min,max'")
@click.option("--xvar", default="N", help="X-axis variable (N or M)")
@click.option("--log", is_flag=True, help="Use logarithmic scale for Y-axis")
@click.option("--tex", is_flag=True, help="Render text using LaTeX")
def main(
    file: str,
    output: str,
    solvers: tuple[str, ...],
    metric: str,
    figsize: str,
    aggregate: str,
    ylim: str | None,
    xvar: str,
    log: bool,
    tex: bool,
):
    plt.rcParams.update({"text.usetex": tex, "font.size": 11, "font.family": "serif"})
    ylim_tuple = None
    if ylim:
        try:
            ylim_tuple = tuple(map(float, ylim.split(",")))
            assert len(ylim_tuple) == 2
        except:
            raise click.BadParameter(f"Invalid ylim: {ylim}")

    try:
        fig_size = tuple(map(float, figsize.split(",")))
        assert len(fig_size) == 2
    except:
        raise click.BadParameter(f"Invalid figsize: {figsize}")

    all_results = load_data(file)
    print_stats(all_results, aggregate)
    fig = create_scaling_plot(
        all_results, metric, list(solvers), fig_size, aggregate, ylim_tuple, xvar, log
    )
    fig_path = Path(output).absolute()
    fig.savefig(fig_path, dpi=300)
    plt.close(fig)
    print(fig_path)


if __name__ == "__main__":
    main()
