import numpy as np
import networkx as nx
import pandas as pd
from sklearn.metrics import normalized_mutual_info_score
from scipy.spatial import distance


def get_f1(gt, pr):
    gt = gt.flatten()
    pr = pr.flatten()
    tp = (gt & pr).sum()
    fp = (~gt & pr).sum()
    fn = (gt & ~pr).sum()
    precision = tp / (tp + fp)
    recall = tp / (tp + fn)
    f1 = 2 * precision * recall / (precision + recall)
    return precision, recall, f1


def get_micro_macro_f1(gt, pr):
    gt = gt.flatten()
    pr = pr.flatten()
    classes = np.unique(gt)
    table = pd.DataFrame(
        {
            "tp": np.zeros(len(classes), dtype=int),
            "fp": np.zeros(len(classes), dtype=int),
            "fn": np.zeros(len(classes), dtype=int),
            "w": np.zeros(len(classes), dtype=int),
        },
        index=classes,
    )
    for label in classes:
        table.loc[label, "tp"] = ((gt == label) & (pr == label)).sum()
        table.loc[label, "fp"] = ((gt != label) & (pr == label)).sum()
        table.loc[label, "fn"] = ((gt == label) & (pr != label)).sum()
        table.loc[label, "w"] = (gt == label).sum() / len(gt)
    table["pr"] = table["tp"] / (table["tp"] + table["fp"])
    table["rc"] = table["tp"] / (table["tp"] + table["fn"])
    table["f1"] = (2 * table["pr"] * table["rc"]) / (table["pr"] + table["rc"])
    micro_f1 = (table["f1"] * table["w"]).sum()
    macro_f1 = table["f1"].mean()
    return micro_f1, macro_f1


def get_accuracy(gt, pr):
    return 1 - np.mean(gt != pr)


def get_jensenshannon(gt, pr, axis=1):  # 0: in degree, 1: out degree, 2: all
    if axis == 0 or axis == 1:
        gt_degrees = gt.sum(axis)
        pr_degrees = pr.sum(axis)
    elif axis == 2:
        gt_degrees = gt.sum(0) + gt.sum(1)
        pr_degrees = pr.sum(0) + pr.sum(1)
    else:
        raise KeyError("axis out of bound. 0: in degree, 1: out degree, 2: all")
    gt_degrees = np.array(gt_degrees).flatten()
    pr_degrees = np.array(pr_degrees).flatten()
    max_degree = max(gt_degrees.max(), pr_degrees.max())
    gt_dist = np.bincount(gt_degrees, minlength=max_degree + 1) / len(gt_degrees)
    pr_dist = np.bincount(pr_degrees, minlength=max_degree + 1) / len(pr_degrees)
    return (
        distance.jensenshannon(gt_dist, pr_dist),
        gt_degrees,
        pr_degrees,
        gt_dist,
        pr_dist,
        max_degree,
    )


def get_graph(links, digraph=True):
    g = nx.DiGraph() if digraph else nx.Graph()
    for i, j in zip(*links.nonzero()):
        g.add_edge(i, j)
    return g


def get_clustering_coefficient(adjacency_matrix, digraph=True):
    g = get_graph(adjacency_matrix, digraph)
    return nx.average_clustering(g)


def get_nmi(gt_communities, pr_communities):
    return normalized_mutual_info_score(gt_communities, pr_communities)


def get_number_of_communities_accuracy(gt_num_communities, pr_num_communities):
    return abs(gt_num_communities - pr_num_communities) / gt_num_communities


def get_pairwise_f1(gt_comm, pr_comm, nodes_count):
    gt = gt_comm == gt_comm[:, None]
    pr = pr_comm == pr_comm[:, None]

    gt[np.diag_indices_from(gt)] = False
    gt = np.triu(gt)
    pr[np.diag_indices_from(pr)] = False
    pr = np.triu(pr)

    tp = (gt & pr).sum()
    fp = (~gt & pr).sum()
    fn = (gt & ~pr).sum()

    precision = tp / (tp + fp)
    recall = tp / (tp + fn)
    f1 = 2 * precision * recall / (precision + recall)
    return precision, recall, f1


def get_conductance(links, communities, num_communities, digraph=True):
    links_communities = np.zeros((num_communities, num_communities), dtype=np.int64)
    for i, j in zip(*links.nonzero()):
        links_communities[communities[i], communities[j]] += 1
        if not digraph:
            links_communities[communities[j], communities[i]] += 1
    m = links_communities.diagonal()
    c = links_communities.sum(axis=1) - m
    m = m[c != 0]
    c = c[c != 0]
    return np.sum(c / (2 * m + c)) / num_communities


def get_number_of_connected_nodes(links):
    connected_nodes = set()
    for i, j in zip(*links.nonzero()):
        connected_nodes.add(i)
        connected_nodes.add(j)
    return len(connected_nodes)


def get_density(links, communities, num_communities):
    comm_edges_set = [set() for _ in range(num_communities)]
    for i, j in zip(*links.nonzero()):
        if communities[i] == communities[j]:
            comm_edges_set[communities[i]].add((i, j))
    comm_density = list()
    for i in range(num_communities):
        num_nodes_in = (communities == i).sum()
        if num_nodes_in == 0 or num_nodes_in == 1:
            continue
        comm_density.append(len(comm_edges_set[i]) / num_nodes_in / (num_nodes_in - 1))
    return sum(comm_density) / len(comm_density)


def get_average_community_size(communities):
    return np.bincount(communities).mean()
