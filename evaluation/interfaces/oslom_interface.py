import numpy as np
from pathlib import Path
import os
import subprocess
from typing import List


OSLOM_DIR_EXECUTIVE_PATH = Path("../OSLOM2/oslom_dir")


class OSLOM(object):
    def __init__(
        self,
        links: List,
        num_nodes: int,
        working_dir: Path,
    ) -> None:
        self.links = links
        self.num_nodes = num_nodes
        self.working_dir = working_dir

    def _write_temp_oslom_network_matrix(self) -> None:
        with open(self.working_dir / "_oslom_matrix.txt", "w") as links_file:
            for u, v, w in self.links:
                links_file.write(f"{u}\t{v}\n")

    def _run_oslom(self) -> None:
        executive = OSLOM_DIR_EXECUTIVE_PATH
        command = [executive, "-f", f"{self.working_dir}/_oslom_matrix.txt", "-uw"]
        print("Running OSLOM")
        print(" ".join(list(map(str, command))))
        subprocess.run(command)
        print("OSLOM Done")

    def get_results(self, force_recalculation=False) -> np.ndarray:
        if force_recalculation or not os.path.exists(
            self.working_dir / "_oslom_matrix.txt_oslo_files"
        ):
            self._write_temp_oslom_network_matrix()
            self._run_oslom()
        communities = np.zeros(self.num_nodes, dtype=np.int32)
        while True:
            try:
                with open(
                    self.working_dir / "_oslom_matrix.txt_oslo_files" / "tp"
                ) as tp_file:
                    lines = tp_file.readlines()[1::2]
                    for i, line in enumerate(lines):
                        line = line.strip()
                        if line == "":
                            raise ValueError("Empty line in community file")
                        nodes_in_community = line.split()
                        for node in nodes_in_community:
                            communities[int(node)] = i + 1
                break
            except FileNotFoundError:
                pass
        return communities, max(communities) + 1
