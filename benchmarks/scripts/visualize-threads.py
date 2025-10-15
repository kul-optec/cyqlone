from contextlib import suppress
from pathlib import Path
import time
from typing import DefaultDict
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import numpy as np
import pandas as pd
import matplotlib.colors as mcolors
from plotly.subplots import make_subplots
import plotly.io as pio

pio.renderers.default = "browser"

plt.rcParams.update(
    {
        "text.usetex": True,
        "text.latex.preamble": r"\renewcommand{\sfdefault}{phv}\renewcommand{\rmdefault}{ptm}",
        "font.family": "ptm",
        "font.size": 15,
        "legend.fontsize": 13,
        "xtick.labelsize": 12,
        "ytick.labelsize": 12,
        "lines.linewidth": 1.2,
        "lines.markersize": 5,
        "legend.borderpad": 0.25,
        "legend.labelspacing": 0.25,
    }
)

colors = {
    "factor prep": "red",
    "factor psi": "blue",
    "build Ψ": "tomato",
    "factor Ψ": "blue",
    "factor Ψ YY": "navy",
    "unpack Ψ": "tan",
    "factor Ψ scalar": "teal",
    "factor Ψ YY scalar": "dodgerblue",
    "updowndate": "navy",
    "updowndate H": "cyan",
    "updowndate Ψ": "yellow",
    "solve Hd=-g-MᵀΔλ": "cornflowerblue",
    "solve ψ fwd": "darkgreen",
    "solve ψ fwd 2": "darkgreen",
    "solve ψ fwd scalar": "seagreen",
    "solve ψ fwd scalar 2": "seagreen",
    "pack Δλ": "tan",
    "unpack Δλ": "tan",
    "solve ψ rev": "purple",
    "solve ψ rev 2": "purple",
    "solve ψ rev scalar": "mediumpurple",
    "solve ψ rev scalar 2": "mediumpurple",
    "solve Hv=g": "orange",
    "eval rhs Ψ": "goldenrod",
    "solve store-notify": "crimson",
    "updowndate wait": "black",
    "updowndate body": "gray",
    "riccati factor": "slateblue",
    "riccati solve bwd": "greenyellow",
    "riccati solve fwd": "hotpink",
    "linesearch breakpoints": "pink",
    "linesearch breakpoints cyqlone": "pink",
    "linesearch sort": "green",
    "linesearch partition": "green",
    "linesearch find": "tan",
    "prop_diag_rev": "navy",
    "prop_diag_fwd": "blue",
    "Subtract UUᵀ": "navy",
    "Subtract YYᵀ": "blue",
    "prop_subdiag": "cyan",
    "Invert Q": "teal",
    "Compute first U": "lawngreen",
    "Compute first Y": "orange",
    "Compute U": "lawngreen",
    "Trsm U": "darkgreen",
    "Compute Y": "orange",
    "Trsm Y": "brown",
    "Compute L⁻ᵀL⁻¹": "mediumpurple",
    "Compute (BA)(BA)ᵀ": "purple",
    "Wait D": "red",
    "Riccati init": "tomato",
    "Riccati QRS": "tan",
    "Riccati update AB": "goldenrod",
    "Riccati last": "darksalmon",
    "wait": "crimson",
    "wait D": "crimson",
    "wait UY": "crimson",
    "wait D(U)": "crimson",
    "wait D(Y)": "crimson",
    "Factor D": "deepskyblue",
    "notify": "yellow",
    "notify U": "yellow",
    "notify Y": "yellow",
    "solve Ψ pcg": "lawngreen",
}

factor_step_names = {
    "factor prep",
    "factor psi",
    "factor Ψ",
    "factor Ψ YY",
    "factor Ψ scalar",
    "factor Ψ YY scalar",
    "riccati factor",
}
solve_step_names = {
    "solve Hd=-g-MᵀΔλ",
    "solve ψ fwd",
    "solve ψ rev",
    "solve Hv=g",
    "solve store-notify",
    "riccati solve bwd",
    "riccati solve fwd",
}
updowndate_step_names = {"updowndate", "updowndate H", "updowndate Ψ"}

labels = {
    "factor prep": r"Factor $H$, compute $V,W$",
    "factor psi": r"Factor $\Psi$",
    "build Ψ": r"Build $\Psi$",
    "factor Ψ": r"Factor $\Psi$",
    "factor Ψ YY": r"Update $\Psi - YY^\top$",
    "unpack Ψ": r"Unpack $\Psi$",
    "factor Ψ scalar": r"Factor $\Psi$ (scalar)",
    "factor Ψ YY scalar": r"Update $\Psi - YY^\top$ (scalar)",
    "updowndate": r"Up/downdate preparation",
    "updowndate H": r"Up/downdate $H$",
    "updowndate Ψ": r"Up/downdate $\Psi$",
    "solve Hd=-g-MᵀΔλ": r"Solve $H\Delta x$",
    "solve ψ fwd": r"Forward substitution $\Psi$",
    "solve ψ rev": r"Backward substitution $\Psi$",
    "solve Hv=g": r"Solve $Hv$",
    "solve store-notify": r"Notification overhead",
    "riccati factor": r"Riccati factor",
    "riccati solve bwd": r"Riccati backward solve",
    "riccati solve fwd": r"Riccati forward solve",
    "linesearch breakpoints": r"Line search compute breakpoints",
    "linesearch breakpoints cyqlone": r"Line search compute breakpoints",
    "linesearch sort": r"Line search sort breakpoints",
    "linesearch partition": r"Line search partition breakpoints",
    "linesearch find": r"Line search find minimum",
}
labels = {
    "factor prep": r"$\text{Factor }H,\text{ compute }V,W$",
    "factor psi": r"$\text{Factor }\Psi$",
    "build Ψ": r"$\text{Build }\Psi$",
    "factor Ψ": r"$\text{Factor }\Psi$",
    "factor Ψ YY": r"$\text{Update }\Psi - YY^\top$",
    "unpack Ψ": r"$\text{Unpack }\Psi$",
    "factor Ψ scalar": r"$\text{Factor }\Psi \text{ (scalar)}$",
    "factor Ψ YY scalar": r"$\text{Update }\Psi - YY^\top \text{ (scalar)}$",
    "updowndate": r"$\text{Up/downdate preparation}$",
    "updowndate H": r"$\text{Up/downdate }H$",
    "updowndate Ψ": r"$\text{Up/downdate }\Psi$",
    "solve Hd=-g-MᵀΔλ": r"$\text{Solve }H\Delta x$",
    "solve ψ fwd": r"$\text{Forward substitution }\Psi$",
    "solve ψ rev": r"$\text{Backward substitution }\Psi$",
    "solve Hv=g": r"$\text{Solve }Hv$",
    "solve store-notify": r"$\text{Notification overhead}$",
    "riccati factor": r"$\text{Riccati factor}$",
    "riccati solve bwd": r"$\text{Riccati backward solve}$",
    "riccati solve fwd": r"$\text{Riccati forward solve}$",
    "linesearch breakpoints": r"$\text{Line search compute breakpoints}$",
    "linesearch breakpoints cyqlone": r"$\text{Line search compute breakpoints}$",
    "linesearch sort": r"$\text{Line search sort breakpoints}$",
    "linesearch partition": r"$\text{Line search partition breakpoints}$",
    "linesearch find": r"$\text{Line search find minimum}$",
}

labels = {
    "Riccati init": "Modified Riccati",
    "Riccati QRS": None,
    "Riccati last": None,
    "Invert Q": None,
    "Compute first U": "Schur complement",
    "Compute first Y": None,
    "Compute L⁻ᵀL⁻¹": None,
    "Compute (BA)(BA)ᵀ": None,
    "Factor D": "CR factor L",
    "Trsm U": "CR solve U",
    "Trsm Y": "CR solve Y",
    "Compute U": "CR multiply YUᵀ",
    "Compute Y": "CR multiply UYᵀ",
    "Subtract YYᵀ": "CR multiply YYᵀ",
    "Subtract UUᵀ": "CR multiply UUᵀ",
    "riccati": "Riccati",
}

# colors = {
#     "Riccati init": "wheat",
#     "Riccati QRS": "wheat",
#     "Riccati last": "wheat",
#     "Invert Q": "wheat",
#     "Compute first U": "purple",
#     "Compute first Y": "purple",
#     "Compute L⁻ᵀL⁻¹": "purple",
#     "Compute (BA)(BA)ᵀ": "purple",
#     "Factor D": "#f2a28c",
#     "Trsm U": "#80e880",
#     "Trsm Y": "#fed27c",
#     "Compute U": "#2f2fff",
#     "Compute Y": "#6699ff",
#     "Subtract YYᵀ": "#ff6666",
#     "Subtract UUᵀ": "#da0000",
#     "riccati": "gray",
# }
# colors = {
#     "Riccati init": "wheat",
#     "Riccati QRS": "wheat",
#     "Riccati last": "wheat",
#     "Invert Q": "wheat",
#     "Compute first U": "purple",
#     "Compute first Y": "purple",
#     "Compute L⁻ᵀL⁻¹": "purple",
#     "Compute (BA)(BA)ᵀ": "purple",
#     "Factor D": "#f58464",
#     "Trsm U": "#3ccb3c",
#     "Trsm Y": "#ffb829",
#     "Compute U": "#2f2fff",
#     "Compute Y": "#0055ff",
#     "Subtract YYᵀ": "#FF0000",
#     "Subtract UUᵀ": "#da0000",
#     "riccati": "gray",
# }
colors = {
    "factor prep": "red",
    "factor psi": "blue",
    "build Ψ": "tomato",
    "factor Ψ": "blue",
    "factor Ψ YY": "navy",
    "unpack Ψ": "tan",
    "factor Ψ scalar": "teal",
    "factor Ψ YY scalar": "dodgerblue",
    "updowndate": "navy",
    "updowndate H": "cyan",
    "updowndate Ψ": "yellow",
    "solve Hd=-g-MᵀΔλ": "cornflowerblue",
    "solve ψ fwd": "darkgreen",
    "solve ψ fwd 2": "darkgreen",
    "solve ψ fwd scalar": "seagreen",
    "solve ψ fwd scalar 2": "seagreen",
    "pack Δλ": "tan",
    "unpack Δλ": "tan",
    "solve ψ rev": "purple",
    "solve ψ rev 2": "purple",
    "solve ψ rev scalar": "mediumpurple",
    "solve ψ rev scalar 2": "mediumpurple",
    "solve Hv=g": "orange",
    "eval rhs Ψ": "goldenrod",
    "solve store-notify": "crimson",
    "updowndate wait": "black",
    "updowndate body": "gray",
    "riccati factor": "slateblue",
    "riccati solve bwd": "greenyellow",
    "riccati solve fwd": "hotpink",
    "linesearch breakpoints": "pink",
    "linesearch sort": "green",
    "linesearch partition": "green",
    "linesearch find": "tan",
    "prop_diag_rev": "navy",
    "prop_diag_fwd": "blue",
    "Subtract UUᵀ": "navy",
    "Subtract YYᵀ": "blue",
    "prop_subdiag": "cyan",
    "Invert Q": "teal",
    "Compute first U": "lawngreen",
    "Compute first Y": "orange",
    "Compute U": "lawngreen",
    "Trsm U": "darkgreen",
    "Compute Y": "orange",
    "Trsm Y": "brown",
    "Compute L⁻ᵀL⁻¹": "mediumpurple",
    "Compute (BA)(BA)ᵀ": "purple",
    "Wait D": "red",
    "Riccati init": "tomato",
    "Riccati QRS": "tan",
    "Riccati update AB": "goldenrod",
    "Riccati last": "darksalmon",
    "wait": "crimson",
    "wait D": "crimson",
    "wait UY": "crimson",
    "wait D(U)": "crimson",
    "wait D(Y)": "crimson",
    "Factor D": "deepskyblue",
    "notify": "yellow",
    "notify U": "yellow",
    "notify Y": "yellow",
    "solve Ψ pcg": "lawngreen",
} | {
    "Riccati init": "#4abdea",
    "Riccati QRS": "#4abdea",
    "Riccati update AB": "#4abdea",
    "Riccati last": "#4abdea",
    "Invert Q": "navajowhite",
    "Compute first U": "navajowhite",
    "Compute first Y": "navajowhite",
    "Compute L⁻ᵀL⁻¹": "navajowhite",
    "Compute (BA)(BA)ᵀ": "navajowhite",
    "Trsm Y": "brown",
    "Factor D": "tomato",
}

fontsizes = {
    "factor prep": 9,
    "factor psi": 9,
    "factor Ψ": 9,
}

width_label_threshold = 7
width_label_rotation_threshold = 25


def map_thread_ids(df: pd.DataFrame):
    """
    Map thread IDs in the DataFrame to a compact range [0, nthreads-1].
    :param df: The processed DataFrame containing a 'thread_id' column.
    :return: The DataFrame with an updated 'thread_id' mapping.
    """
    included_thread_ids = df["name"] == "thread_id"
    if included_thread_ids.any():
        thread_map = {d["thread_id"]: d["instance"] for _, d in df[included_thread_ids].iterrows()}
        df.drop(df[included_thread_ids].index, inplace=True)
        pass
    else:
        unique_thread_ids = df["thread_id"].unique()
        thread_map = {tid: idx for idx, tid in enumerate(unique_thread_ids)}
    df["thread_id"] = df["thread_id"].map(thread_map)
    return df


def load_csv(file_path):
    # Load the CSV file into a DataFrame
    df = pd.read_csv(file_path)
    # Add a new column 'gflops' by dividing 'flop_count' by 'duration' (in nanoseconds)
    df["gflops"] = df["flop_count"] / df["duration"]
    df["end_time"] = df["start_time"] + df["duration"]
    df = map_thread_ids(df)
    return df


def calculate_time_range(df):
    """
    Calculate the first and last timestamps in the DataFrame.
    :param df: The processed DataFrame containing 'start_time' and 'duration' columns.
    :return: A tuple (first_time, last_time).
    """
    last_time = (df["start_time"] + df["duration"]).max()
    first_time = df["start_time"].min()
    return first_time, last_time


def visualize_scheduling(
    fig: go.Figure,
    task_data: pd.DataFrame,
    row: int,
    col: int,
    *,
    bs=None,
    xlim_margin=0.1,
    exclude_legends=None,
    n_threads=1,
    gflops_max=20,
):
    """
    Visualizes the scheduling of a parallel algorithm using Plotly within a subplot.

    Parameters:
        fig (go.Figure): The Plotly figure to add the subplot to.
        task_data (pd.DataFrame): DataFrame containing task data.
        row (int): Row index of the subplot (1-based).
        col (int): Column index of the subplot (1-based).
        bs (int, optional): Block size for task labeling. Defaults to None.
        xlim_margin (float, optional): Margin for the x-axis limit. Defaults to 0.1.
    """
    skip = {"solve wait", "factor wait", "solve", "factor", "barrier-complete", "barrier-arrive", "barrier-arrive-and-wait"}
    barriers = task_data[task_data["name"] == "barrier-complete"]
    barriers_arrive = task_data[task_data["name"].str.startswith("barrier-arrive")]
    task_data = task_data[~task_data["name"].isin(skip)]
    first_time, last_time = calculate_time_range(task_data)
    total_duration = last_time - first_time
    total_duration /= 1000

    if exclude_legends is None:
        exclude_legends = set()

    # Add rectangles for tasks with flop_count < 0
    bar_data = DefaultDict(lambda: DefaultDict(lambda: []))
    for _, td in task_data[task_data["flop_count"] < 0].iterrows():
        task_name = td["name"]
        task_id = td["instance"]
        start_time = td["start_time"]
        duration = td["duration"]
        thread_id = td["thread_id"]
        with suppress(ValueError):
            n_threads = max(int(thread_id) + 1, n_threads)

        start_time -= first_time
        start_time /= 1000
        duration /= 1000

        task_label = str(task_id)
        if bs and bs != 1:
            # if duration > 20 * total_duration / 1200:
            #     task_label = f"{bs * task_id}–{bs * task_id + bs - 1}"
            # else:
            #     task_label = f"{bs * task_id}-{bs * task_id + bs - 1}"
            task_label = f"{bs * task_id}"

        bar_data[task_name]["x"] += [duration]
        bar_data[task_name]["y"] += [thread_id - 0.35]
        bar_data[task_name]["base"] += [start_time]
        bar_data[task_name]["text"] += [task_label]
        bar_data[task_name]["texttemplate"] += [
            task_label if duration > 8.5 * total_duration / 1000 else ""
        ]
        bar_data[task_name]["hovertemplate"] += [
            f"Instance: {task_id}<br>Start: {start_time:.3f}s<br>Duration: {duration:.3f}s"
        ]
    for task_name, v in bar_data.items():
        color = colors.get(task_name, "gray")
        color = "rgba(" + ",".join(map(str, 255 * np.array(mcolors.to_rgb(color)))) + ",0.6)"
        label = labels.get(task_name, task_name)
        # Add rectangle to the subplot
        bar = go.Bar(
            **v,
            offset=0,
            textfont_size=8.5,
            width=0.75,
            orientation="h",
            marker=dict(color=color, line=dict(color="black", width=0.5)),
            name=label,
            textposition="inside",
            insidetextanchor="middle",
            showlegend=(label is not None and task_name not in exclude_legends),
            legendrank=v["base"][0] + 99999 * row,
        )
        fig.add_trace(bar, row=row, col=col)
        if task_name not in exclude_legends:
            exclude_legends.add(task_name)

    factor_steps = task_data[task_data["name"].isin(factor_step_names)]
    last_factor = factor_steps["end_time"].max()
    solve_steps = task_data[task_data["name"].isin(solve_step_names)]
    first_solve = solve_steps["start_time"].min()
    last_solve = solve_steps["end_time"].max()
    updowndate_steps = task_data[task_data["name"].isin(updowndate_step_names)]
    first_updowndate = updowndate_steps["start_time"].min()
    first_solve = first_solve if np.isfinite(first_solve) else last_factor
    if np.isfinite(last_factor + first_solve):
        fig.add_vline(
            x=(last_factor + first_solve - 2 * first_time) / 2000,
            line=dict(color="black", width=1, dash="dot"),
            row=row,
            col=col,
        )
    first_updowndate = first_updowndate if np.isfinite(first_updowndate) else last_solve
    if np.isfinite(last_solve + first_updowndate):
        fig.add_vline(
            x=(last_solve + first_updowndate - 2 * first_time) / 2000,
            line=dict(color="black", width=1, dash="dot"),
            row=row,
            col=col,
        )

    # # Add rectangles for tasks with flop_count > 0
    # for _, td in task_data[task_data["flop_count"] > 0].iterrows():
    #     bar_data = DefaultDict(lambda: [])
    #     start_time = td["start_time"]
    #     duration = td["duration"]
    #     thread_id = td["thread_id"]
    #     gflops = td["gflops"]

    #     start_time -= first_time
    #     start_time /= 1000
    #     duration /= 1000

    #     color = f"rgba({255 - int(255 * gflops / 20)}, {int(255 * gflops / 20)}, 0, 0.9)"
    #     bar_data["x"] += [duration]
    #     bar_data["y"] += [thread_id - 0.35]
    #     bar_data["base"] += [start_time]
    #     bar_data["text"] += [f"{gflops:.2f} GFLOPS"]
    #     bar_data["hovertemplate"] +=[
    #         f"<b>{td['name']}</b><br>GFLOPS: {gflops:.2f}<br>Start: {start_time:.3f}s<br>Duration: {duration:.3f}s"
    #     ]
    #     bar = go.Bar(
    #         **bar_data,
    #         width=0.15,
    #         orientation="h",
    #         showlegend=False,
    #         marker=dict(color=color, line=dict(color="black", width=0.5)),
    #     )
    #     fig.add_trace(bar, row=row, col=col)

    # map_gflops_color = (
    #     lambda gflops: f"rgba({255 - int(255 * gflops / 20)}, {int(255 * gflops / 20)}, 0, 0.9)"
    # )
    # map_gflops_color = lambda gflops: f"hsla({120 * gflops / 20:.2f},100,45,0.9)"

    def map_gflops_color(gflops):
        r, g, b, _ = plt.cm.RdYlGn(gflops / gflops_max)
        return f"rgba({255 * r}, {255 * g}, {255 * b}, 0.9)"

    colors_list = [
        map_gflops_color(td["gflops"])
        for _, td in task_data[task_data["flop_count"] >= 0].iterrows()
    ]
    durations = task_data[task_data["flop_count"] >= 0]["duration"] / 1000
    start_times = (task_data[task_data["flop_count"] >= 0]["start_time"] - first_time) / 1000
    thread_ids = task_data[task_data["flop_count"] >= 0]["thread_id"] - 0.5

    bar = go.Bar(
        name="",
        text="",
        offset=0,
        x=durations,
        y=thread_ids,
        base=start_times,
        width=0.15,
        orientation="h",
        marker=dict(color=colors_list, line=dict(color="black", width=0.5)),
        textposition="inside",
        insidetextanchor="middle",
        hovertemplate=[
            f"<b>{td['name']}</b><br>"
            f"GFLOPS: {td['gflops']:.2f}<br>"
            f"FLOPs: {td['flop_count']:.2f}<br>"
            f"Start: {(td['start_time']-first_time)/1000:.3f}s<br>"
            f"Duration: {td['duration']/1000:.3f}s"
            for _, td in task_data[task_data["flop_count"] >= 0].iterrows()
        ],
        showlegend=False,  # Adjust if legends are required
    )
    fig.add_trace(bar, row=row, col=col)

    # Add rectangles for barrier operations
    bar_data = DefaultDict(lambda: DefaultDict(lambda: []))
    for _, td in barriers_arrive.iterrows():
        task_name = td["name"]
        task_id = td["instance"]
        start_time = td["start_time"]
        duration = td["duration"]
        thread_id = td["thread_id"]

        start_time -= first_time
        start_time /= 1000
        duration /= 1000

        task_label = str(task_id)
        if bs and bs != 1:
            task_label = f"{bs * task_id}"

        bar_data[task_name]["x"] += [duration]
        bar_data[task_name]["y"] += [thread_id - 0.53]
        bar_data[task_name]["base"] += [start_time]
        bar_data[task_name]["text"] += [task_label]
        bar_data[task_name]["texttemplate"] += [
            task_label if duration > 8.5 * total_duration / 1000 else ""
        ]
        bar_data[task_name]["hovertemplate"] += [
            f"Instance: {task_id}<br>Start: {start_time:.3f}s<br>Duration: {duration:.3f}s"
        ]
    for task_name, v in bar_data.items():
        color = "#550055" if task_name == "barrier-arrive" else "#aa00aa"
        color = "rgba(" + ",".join(map(str, 255 * np.array(mcolors.to_rgb(color)))) + ",0.6)"
        label = labels.get(task_name, task_name)
        # Add rectangle to the subplot
        bar = go.Bar(
            **v,
            offset=0,
            textfont_size=8.5,
            width=0.025,
            orientation="h",
            marker=dict(color=color, line=dict(color="black", width=0.5)),
            name=label,
            textposition="inside",
            insidetextanchor="middle",
            showlegend=(label is not None and task_name not in exclude_legends),
            legendrank=v["base"][0] + 99999 * row,
        )
        fig.add_trace(bar, row=row, col=col)
        if task_name not in exclude_legends:
            exclude_legends.add(task_name)

    # Add vertical lines for barrier completion times
    for _, td in barriers.iterrows():
        start_time = td["start_time"]
        start_time -= first_time
        start_time /= 1000
        fig.add_vline(
            x=start_time,
            line=dict(color="#550055", width=2, dash="dash"),
            row=row,
            col=col,
        )

    # Set layout for the subplot
    fig.update_xaxes(
        range=[0, (1 + xlim_margin) * 1e-3 * (last_time - first_time)],
        row=row,
        col=col,
    )
    fig.update_yaxes(
        title_text="Thread",
        tickmode="array",
        tickvals=np.arange(0, n_threads),
        range=[-0.55, n_threads - 0.55],
        row=row,
        col=col,
    )


project_dir = Path(__file__).parent.parent.parent
data_to_plot = {
    "Cyqlone": (
        (
            "traces/36ea2dd026325546f1eaaa99ff01457e79f50034/nx=68-nu=20-ny=50-N=256-thr=8-vl=16-pcg=stair-alt-rm/factor_cyclic_new.csv",
            "traces/36ea2dd026325546f1eaaa99ff01457e79f50034/nx=68-nu=20-ny=50-N=256-thr=4-vl=16-pcg=stair-alt-rm/factor_cyclic_new.csv",
        ),
        dict(
            n_threads=4,
            xlim_margin=0.15,
            title="Thread-level execution traces of KKT factorization methods",
        ),
    )
}

# Human-readable titles of the different data files
subtitles = {}
# Vector lengths
batch_sizes = {}

for name, opts in data_to_plot.items():
    data = {}  # TODO
    if name.endswith(".csv"):
        data = {name: Path(name) if Path(name).is_absolute() else project_dir / name}
    if isinstance(opts, tuple):
        names, opts = opts
        data = {
            name: Path(name) if Path(name).is_absolute() else project_dir / name for name in names
        }

    title = opts.pop(
        "title", "Parallel execution traces for KKT factor, solve and update algorithms"
    )

    num_threads = 4

    exclude_legends = set()
    fig = make_subplots(
        rows=len(data),
        cols=1,
        shared_xaxes=True,
        shared_yaxes=True,
        subplot_titles=list(map(lambda lbl: subtitles.get(lbl, lbl), data)),
        vertical_spacing=0.1,
        figure=go.Figure(
            layout=go.Layout(font=dict(family="Fira Sans Light", size=24, color="black"))
        ),
    )
    for i, (k, d) in enumerate(data.items()):
        visualize_scheduling(
            fig,
            load_csv(d),
            row=i + 1,
            col=1,
            bs=batch_sizes.get(k),
            exclude_legends=exclude_legends,
            **opts,
        )
    fig.update_xaxes(title_text="Time [µs]", row=len(data), col=1)

    fig.update_layout(
        title=dict(
            text=title,
            x=0.5,
            xanchor="center",
        ),
        legend=dict(
            yanchor="middle",
            y=0.5,
            xanchor="right",
            x=0.99,
        ),
        plot_bgcolor="#ececec",
        uniformtext=dict(minsize=5, mode="show"),
    )
    fig.show(include_mathjax=True)

    fig.update_layout(
        autosize=False,
        width=1200 * 0.9,
        height=680 * 0.9,
        legend=dict(
            yanchor="top",
            y=1,
            xanchor="right",
            x=1,
        ),
        uniformtext=dict(minsize=4, mode="show"),
    )
    for annotation in fig["layout"]["annotations"]:
        annotation["font"] = dict(size=28, family="Fira Sans Light", color="black")
    # fig.update_layout(showlegend=False)
    pdf_file = project_dir / "fig" / Path(name + ".pdf")
    pdf_file.parent.mkdir(parents=True, exist_ok=True)
    fig.write_image(pdf_file)
    time.sleep(1)
    fig.write_image(pdf_file)
