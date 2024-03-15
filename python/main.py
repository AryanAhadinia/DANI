import argparse
from pathlib import Path

from dani import dani


def main() -> None:
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-c",
        "--cascades",
        type=Path,
        help="Path to cascades file.",
        required=True,
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        help="Path for writing results.",
        required=True,
    )
    parser.add_argument(
        "--lambda_coef",
        type=Path,
        help="Lambda coefficient.",
        default=10,
    )
    parser.add_argument(
        "--lambda_exp",
        type=Path,
        help="Lambda exponent.",
        default=10,
    )
    parser.add_argument(
        "--theta_exp",
        type=Path,
        help="Theta exponent.",
        default=2,
    )

    args = parser.parse_args()

    num_nodes, cascades = dani.read_cascades(args.cascades)
    links = dani.run_dani(
        num_nodes,
        cascades,
        args.lambda_coef,
        args.lambda_exp,
        args.theta_exp,
    )
    dani.write_results(args.output, links)


if __name__ == "__main__":
    main()
