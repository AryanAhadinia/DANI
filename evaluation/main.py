import argparse
from interfaces import network_interfaces, oslom_interface
from metrics_ import metrics_ as metrics
from pathlib import Path
import pprint


def main():
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--gt", type=Path, help="Path to the ground truth network.")
    arg_parser.add_argument("--pr", type=Path, help="Path to the predicted network.")
    args = arg_parser.parse_args()

    gt_network_interface = network_interfaces.get_compatible_interface(args.gt / "network.txt")
    num_nodes = gt_network_interface.nodes_count
    gt_links, n = gt_network_interface.get_predicted_links()
    gt_matrix = gt_network_interface.get_adjacency_matrix(n)
    k = gt_matrix.sum()

    pr_network_interface = network_interfaces.get_compatible_interface(args.pr / "network.txt", k)
    pr_network_interface.nodes_count = num_nodes
    pr_links, _ = pr_network_interface.get_predicted_links()
    pr_matrix = pr_network_interface.get_adjacency_matrix(n)

    gt_oslom = oslom_interface.OSLOM(gt_links, num_nodes, args.gt)
    gt_communities, gt_num_communities = gt_oslom.get_results()

    pr_oslom = oslom_interface.OSLOM(pr_links, num_nodes, args.pr)
    pr_communities, pr_num_communities = pr_oslom.get_results()

    precision, recall, f1 = metrics.get_f1(gt_matrix, pr_matrix)
    micro_f1, macro_f1 = metrics.get_micro_macro_f1(gt_matrix, pr_matrix)
    accuracy = metrics.get_accuracy(gt_matrix, pr_matrix)
    nmi = metrics.get_nmi(gt_communities, pr_communities)
    nca = metrics.get_number_of_communities_accuracy(
        gt_num_communities, pr_num_communities
    )
    js, _, _, _, _, _ = metrics.get_jensenshannon(gt_matrix, pr_matrix, axis=2)

    gt_average_community = metrics.get_average_community_size(gt_communities)
    pr_average_community = metrics.get_average_community_size(pr_communities)

    gt_conductance = metrics.get_conductance(
        gt_matrix, gt_communities, gt_num_communities
    )
    pr_conductance = metrics.get_conductance(
        pr_matrix, pr_communities, pr_num_communities
    )

    gt_number_of_connected_nodes = metrics.get_number_of_connected_nodes(gt_matrix)
    pr_number_of_connected_nodes = metrics.get_number_of_connected_nodes(pr_matrix)

    gt_clustering_coef = metrics.get_clustering_coefficient(gt_matrix)
    pr_clustering_coef = metrics.get_clustering_coefficient(pr_matrix)

    _, _, pwf = metrics.get_pairwise_f1(gt_communities, pr_communities, num_nodes)

    gt_density = metrics.get_density(gt_matrix, gt_communities, gt_num_communities)
    pr_density = metrics.get_density(pr_matrix, pr_communities, pr_num_communities)

    metrics_values = {
        "precision": precision,
        "recall": recall,
        "f1": f1,
        "micro-f1": micro_f1,
        "macro-f1": macro_f1,
        "accuracy": accuracy,
        "nmi": nmi,
        "nca": nca,
        "js": js,
        "gt_average_community": gt_average_community,
        "pr_average_community": pr_average_community,
        "gt_conductance": gt_conductance,
        "pr_conductance": pr_conductance,
        "gt_number_of_connected_nodes": gt_number_of_connected_nodes,
        "pr_number_of_connected_nodes": pr_number_of_connected_nodes,
        "gt_clustering_coef": gt_clustering_coef,
        "pr_clustering_coef": pr_clustering_coef,
        "pwf": pwf,
        "gt_density": gt_density,
        "pr_density": pr_density,
    }
    pprint.pprint(metrics_values)


if __name__ == "__main__":
    main()
