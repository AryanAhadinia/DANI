# DANI (Python Version)

This is the official implementation of DANI method in Python language.

## Input Data

The only input of the method is a file containing cascades with a header of nodes' names. Each cascade is a list of node-time pair of participation in information diffusion. Node IDs, which are zero-indexed integers, and times, which are floats, are separated using commas and pairs are separated using semicolons.

```txt
1,1
2,2
3,3
4,4
5,5

4,3.12;2,6.73;3,2.17
1,4.43;5,7.43;3,8.53;2,1.15
```

Note that cascades are not necessarily sorted regarding the timestamp of participation.

## How to Run?

```bash
python3 main.py --cascades /path/to/cascade/file.txt --output /path/for/writing/results.txt
```

## Cite us

```bibtex
@misc{DANI,
    title={{DANI}: Fast Diffusion Aware Network Inference with Preserving Topological Structure Property}, 
    author={Maryam Ramezani and Aryan Ahadinia and Erfan Farhadi and Hamid R. Rabiee},
    year={2023},
    eprint={2310.01696},
    archivePrefix={arXiv},
    primaryClass={cs.SI}
}
```
