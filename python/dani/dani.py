from pathlib import Path
from typing import List
from typing import Tuple

import numpy as np


def run_dani(
    num_nodes: int,
    cascades: Tuple[int, List[List[Tuple[int, float]]]],
    lambda_coef: float,
    lambda_exp: float,
    theta_exp: float,
) -> List[Tuple[int, int, float]]:
    prt, joint_prt = calculate_participation(cascades, num_nodes)
    lambda_matrix = calculate_lambda(cascades, num_nodes, lambda_coef)
    theta, abs_links_mask = calculate_theta(prt, joint_prt, num_nodes)
    weights = (lambda_matrix**lambda_exp) * (theta**theta_exp)
    np.place(weights, abs_links_mask, 1)
    links = matrix_to_list(weights)
    links.sort(key=lambda link: link[-1], reverse=True)
    return links


def calculate_participation(
    cascades: Tuple[int, List[List[Tuple[int, float]]]],
    num_nodes: int,
) -> Tuple[np.array, np.array]:
    participation = np.zeros((num_nodes), dtype=int)
    joint_participation = np.zeros((num_nodes, num_nodes), dtype=int)
    for cascade in cascades:
        for i, (node_i, _) in enumerate(cascade):
            participation[node_i] += 1
            for node_j, _ in cascade[i + 1:]:
                joint_participation[node_i, node_j] += 1
    return participation, joint_participation


def calculate_lambda(
    cascades: Tuple[int, List[List[Tuple[int, float]]]],
    num_nodes: int,
    lambda_coef: float,
) -> np.array:
    lambda_matrix = np.zeros((num_nodes, num_nodes), dtype=float)
    for c in cascades:
        d = np.zeros((num_nodes, num_nodes), dtype=float)
        for i in range(len(c)):
            for j in range(i):
                d[c[j][0], c[i][0]] = lambda_coef / (i * (i - j))
        sum_d = d.sum(axis=1)
        np.place(sum_d, sum_d == 0, 1)
        lambda_c = d / sum_d.reshape(-1, 1)
        lambda_matrix += lambda_c
    sum_lambda = np.sum(lambda_matrix, axis=1)
    np.place(sum_lambda, sum_lambda == 0, 1)
    lambda_matrix /= sum_lambda.reshape(-1, 1)
    return lambda_matrix


def calculate_theta(
    participation: np.array,
    joint_participation: np.array,
    num_nodes: int,
) -> Tuple[np.array, np.array]:
    theta = np.zeros((num_nodes, num_nodes), dtype=float)
    abs_links_mask = np.zeros((num_nodes, num_nodes), dtype=bool)
    for i in range(num_nodes):
        for j in range(num_nodes):
            intersection = joint_participation[i, j]
            union = participation[i] + participation[j] - intersection
            if intersection == union and intersection != 0:
                abs_links_mask[i, j] = True
            if union != 0:
                theta[i, j] = intersection / union
    return theta, abs_links_mask


def split_cascade_node_time(pair: str) -> Tuple[int, float]:
    node, time = pair.split(",")
    return int(node), float(time)


def read_cascades(path: Path) -> Tuple[int, List[List[Tuple[int, float]]]]:
    with open(path) as f:
        lines = f.readlines()
    lines = list(map(str.strip, lines))
    num_nodes = lines.index("")
    cascades_lines = lines[num_nodes + 1:]
    cascades = []
    for cascades_line in cascades_lines:
        cascade = sorted(
            list(map(split_cascade_node_time, cascades_line.split(";"))),
            key=lambda p: p[1],
        )
        cascades.append(cascade)
    return num_nodes, cascades


def matrix_to_list(weights) -> List[Tuple[int, int, float]]:
    num_nodes, _ = weights.shape
    links = []
    for i in range(num_nodes):
        for j in range(num_nodes):
            if weights[i][j] != 0:
                links.append((i, j, weights[i][j]))
    return links


def write_results(path: Path, links: List[Tuple[int, int, float]]) -> None:
    with open(path, "w") as f:
        for i, j, w in links:
            f.write(f"{i}\t{j}\t{w}\n")
