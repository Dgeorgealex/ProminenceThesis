import argparse
import os
import sys
import math
import numpy
from PIL import Image as im

INPUT_FILE = "../data/Romania/N45E024.hgt"
HEIGHT = 1100


def get_parameters():
    if len(sys.argv) == 1:
        return INPUT_FILE, HEIGHT
    parser = argparse.ArgumentParser(description="Visualize the tile islands")
    parser.add_argument('path', type=str, help='Path to the tile')
    parser.add_argument('height', type=int, help='Height of the water')
    args = parser.parse_args()

    return args.path, args.height


def main():
    (input_path, height) = get_parameters()
    siz = os.path.getsize(input_path)
    dim = int(math.sqrt(siz / 2))

    data = numpy.fromfile(input_path, numpy.dtype('>i2'), dim * dim)
    data[data < height] = 255
    data[data >= height] = 0

    data = numpy.array(data, numpy.uint8)
    data = data.reshape((dim, dim))

    for i in range(dim):
        data[i][0] = data[i][dim - 1] = data[0][i] = data[dim - 1][i] = 0

    picture = im.fromarray(data)
    picture.save("island.png")


if __name__ == "__main__":
    main()
