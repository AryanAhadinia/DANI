# DANI

This is an official implementation for paper titled "DANI: Fast Diffusion Aware Network Inference with Preserving Topological Structure Property".

## How to Run C++ Version

The default implementation of DANI is in C++, which is way faster than the python version due to C++ characteristics.

### Compile the Code

To compile the code, run the command below.

```bash
make
```

### Input Data Format

Only input required for the code is a file containing nodes names and cascades sorted in ascending order regarding infection timestamps. The input file contains two parts which are separated by a blank line:

1. The first $n$ lines of the file contain the name of nodes in the network. Each line contain `<id>` and `<name>` where node ids are integers starting from 0 to $n-1$.

    ```text
    <id>,<name>
    ```

2. The next $m$ lines contains $m$ cascades, each on a line. Each cascade is represented by a sequence of `<id>` and the corresponding `<timestamp>` of infection pairs where `<id>` and `<timestamp>` are separated by a comma and pairs are separated by a semi-colon. The cascades are sorted in ascending order regarding infection timestamps.

    ```text
    <id>,<timestamp>;<id>,<timestamp>;<id>,<timestamp>;...;<id>,<timestamp>
    ```

An example of input file is in file [`cascades.txt`](dataset/cascades.txt) which contains 10000 cascades generated on the network in [`dataset/groundtruth/network.txt`](dataset/groundtruth/network.txt).

### Run the Code

Here is the list of arguments that can be passed to the code:

- `-i:` which is the address of input file.
- `-Y:` which is the address of output file.

Run the code using following command:

```bash
./main -i:<input_file> -Y:<output_file>
```

To run the code on the example input file, run the command below:

```bash
./main -i:dataset/cascades.txt -Y:dataset/inferred/network.txt
```

### Output

The output of the model is in _Tab Separated Triples_ format. Each line contains a triple in format of $u\space v\space \alpha$ in which alpha is scored assigned to directed edge $(u,\space v)$ by the model. Note that edges are sorted in descending order.

## How to Run Python Version

DANI has also a python version. If you want to use the python version, follow the instructions in README.md in `python` directory.

## How to Run Evaluation

### Compiling OSLOM2

We use OSLOM2 [2] for the community detection. You have to download [OSLOM2](http://www.oslom.org/code/OSLOM2.tar.gz) and unpack it in the `./OSLOM2` and compile it to do the evaluation. You can use the script below to do these works.

```bash
wget http://www.oslom.org/code/OSLOM2.tar.gz
tar -xvzf OSLOM2.tar.gz
cd OSLOM2
chmod 744 compile_all.sh
./compile_all.sh
cd ..
```

Note: Compiling OSLOM may result in some errors but it is OK for our usage.

### Calculating Metrics

To do the evaluation, run the evaluation code as below.

```bash
cd evaluation
python3 main.py --gt <gt> --pr <pr>
```

Where `<gt>` is a path to a directory contatining a file named `network.txt` containing true underlying network and `<pr>` is a path to a directory contatining a file named `network.txt` containing inferred network. For example, you can use the following command to run the evaluation on the example dataset.

```bash
cd evaluation
python3 main.py --gt ../dataset/groundtruth --pr ../dataset/inferred
```

## Compatibility

This code has been compiled and tested on Ubuntu 22.04 LTS using Python 3.10.

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

## References

We have used SNAP library and OSLOM2 to develop this method.

[1] Leskovec, J. and Sosič, R., 2016. Snap: A general-purpose network analysis and graph-mining library. ACM Transactions on Intelligent Systems and Technology (TIST), 8(1), pp.1-20.

[2] Lancichinetti, A., Radicchi, F., Ramasco, J.J. and Fortunato, S., 2011. Finding statistically significant communities in networks. PloS one, 6(4), p.e18961.
