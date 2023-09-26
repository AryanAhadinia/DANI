from abc import abstractclassmethod
from pathlib import Path
from typing import List, Tuple

import numpy as np


class NetworkInterface(object):
    def __init__(self, network_file_path: Path, k=None) -> None:
        self.network_file_path = network_file_path
        self.predicted_links = None
        self.nodes_count = None
        self.adjacency_matrix = None
        self.k = k

    @abstractclassmethod
    def get_predicted_links(self) -> List[Tuple[int, int, float]]:
        pass

    @abstractclassmethod
    def get_adjacency_matrix(self, symmetric: bool = False):
        pass

    @abstractclassmethod
    def is_compatible(self) -> bool:
        pass

    @abstractclassmethod
    def release_resources(self) -> None:
        pass

    def __repr__(self) -> str:
        return f"NetworkInterface({self.network_file_path})"

    def __str__(self) -> str:
        return self.__repr__()


class SeparatedLinesInterface(NetworkInterface):
    def __init__(
        self, network_file_path: Path, separator: str, elements_per_line: int, k=None
    ) -> None:
        super().__init__(network_file_path, k)
        self.separator = separator
        self.elements_per_line = elements_per_line

    def get_predicted_links(self) -> Tuple[List[Tuple[int, int, float]], int]:
        if self.predicted_links:
            return self.predicted_links, self.nodes_count
        max_node_id = 0
        predicted_links = []
        with open(self.network_file_path, "r") as network_file:
            lines = network_file.readlines()
            if lines.count("\n") == 1 and lines[-1] != "\n":
                idx = lines.index("\n")
                max_node_id = idx - 1
                lines = lines[idx + 1 :]
            lines_read = 0
            for i, line in enumerate(lines):
                if self.k:
                    if lines_read < self.k:
                        lines_read += 1
                    else:
                        break
                line = line.strip()
                if line == "":
                    raise ValueError("Empty line in network file")
                elements = line.split(self.separator)
                if len(elements) != self.elements_per_line:
                    raise ValueError(
                        "Line {} has {} elements, expected {}".format(
                            i, len(elements), self.elements_per_line
                        )
                    )
                try:
                    u = int(elements[0])
                    v = int(elements[1])
                except:
                    raise ValueError(
                        "Line {} has non-integer elements as node name.".format(i)
                    )
                if len(elements) == 3:
                    predicted_links.append((u, v, float(elements[2])))
                else:
                    predicted_links.append((u, v, 1))
                max_node_id = max(max_node_id, u, v)
        self.predicted_links = predicted_links
        self.nodes_count = max_node_id + 1
        return self.predicted_links, self.nodes_count

    def get_adjacency_matrix(self, nodes_count):
        if self.adjacency_matrix:
            return self.adjacency_matrix
        predicted_links, _ = self.get_predicted_links()
        adjacency_matrix = np.zeros((nodes_count, nodes_count), dtype=np.bool_)
        for u, v, w in predicted_links:
            adjacency_matrix[u, v] = 1
        self.adjacency_matrix = adjacency_matrix
        return adjacency_matrix

    def is_compatible(self) -> bool:
        try:
            self.get_predicted_links()
            return True
        except:
            return False

    def release_resources(self) -> None:
        self.predicted_links = None
        self.nodes_count = None
        self.adjacency_matrix = None

    def __repr__(self) -> str:
        return f"SeparatedLinesInterface({self.network_file_path}, {self.separator}, {self.elements_per_line})"

    def __str__(self) -> str:
        return self.__repr__()


class SeparatedTuplesInterface(SeparatedLinesInterface):
    def __init__(self, network_file_path: Path, separator: str, k=None) -> None:
        super().__init__(network_file_path, separator, 2, k)

    def __repr__(self) -> str:
        return f"SeparatedTuplesInterface({self.network_file_path}, {self.separator})"

    def __str__(self) -> str:
        return self.__repr__()


class SeparatedTriplesInterface(SeparatedLinesInterface):
    def __init__(self, network_file_path: Path, separator: str, k=None) -> None:
        super().__init__(network_file_path, separator, 3, k)

    def __repr__(self) -> str:
        return f"SeparatedTriplesInterface({self.network_file_path}, {self.separator})"

    def __str__(self) -> str:
        return self.__repr__()


class TabSeparatedTuplesInterface(SeparatedTuplesInterface):
    def __init__(self, network_file_path: Path, k=None) -> None:
        super().__init__(network_file_path, "\t", k)

    def __repr__(self) -> str:
        return f"TabSeparatedTuplesInterface({self.network_file_path})"

    def __str__(self) -> str:
        return self.__repr__()


class SpaceSeparatedTuplesInterface(SeparatedTuplesInterface):
    def __init__(self, network_file_path: Path, k=None) -> None:
        super().__init__(network_file_path, " ", k)

    def __repr__(self) -> str:
        return f"SpaceSeparatedTuplesInterface({self.network_file_path})"

    def __str__(self) -> str:
        return self.__repr__()


class CommaSeparatedTuplesInterface(SeparatedTuplesInterface):
    def __init__(self, network_file_path: Path, k=None) -> None:
        super().__init__(network_file_path, ",", k)

    def __repr__(self) -> str:
        return f"CommaSeparatedTuplesInterface({self.network_file_path})"

    def __str__(self) -> str:
        return self.__repr__()


class TabSeparatedTriplesInterface(SeparatedTriplesInterface):
    def __init__(self, network_file_path: Path, k=None) -> None:
        super().__init__(network_file_path, "\t", k)

    def __repr__(self) -> str:
        return f"TabSeparatedTriplesInterface({self.network_file_path})"

    def __str__(self) -> str:
        return self.__repr__()


class SpaceSeparatedTriplesInterface(SeparatedTriplesInterface):
    def __init__(self, network_file_path: Path, k=None) -> None:
        super().__init__(network_file_path, " ", k)

    def __repr__(self) -> str:
        return f"SpaceSeparatedTriplesInterface({self.network_file_path})"

    def __str__(self) -> str:
        return self.__repr__()


class CommaSeparatedTriplesInterface(SeparatedTriplesInterface):
    def __init__(self, network_file_path: Path, k=None) -> None:
        super().__init__(network_file_path, ",", k)

    def __repr__(self) -> str:
        return f"CommaSeparatedTriplesInterface({self.network_file_path})"

    def __str__(self) -> str:
        return self.__repr__()


def get_compatible_interface(network_file_path: Path, k=None) -> NetworkInterface:
    interfaces = [
        CommaSeparatedTuplesInterface,
        TabSeparatedTuplesInterface,
        SpaceSeparatedTuplesInterface,
        CommaSeparatedTriplesInterface,
        TabSeparatedTriplesInterface,
        SpaceSeparatedTriplesInterface,
    ]
    for interface in interfaces:
        try:
            network_interface = interface(network_file_path, k)
            network_interface.get_predicted_links()
            return network_interface
        except:
            pass
    raise ValueError(
        f"No compatible interface found for network file {network_file_path}"
    )
