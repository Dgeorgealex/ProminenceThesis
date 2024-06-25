import argparse
import statistics
import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import Normalize

INPUT_FILE = "../data/RomaniaANS/info.txt"


def get_path():
    if len(sys.argv) == 1:
        return INPUT_FILE

    parser = argparse.ArgumentParser(description="Compute tile statistics")
    parser.add_argument('path', type=str, help='Path to the info file')
    args = parser.parse_args()

    return args.path


def read_data(input_path):
    data = []
    with open(input_path) as file:
        for line in file:
            t = tuple(map(int, line.split(',')))
            if t[2] == 0:
                continue

            percentage = t[3] / t[2] * 100
            t = t +(percentage, )
            data.append(t)

    return data


def calculate_statistics_with_index(data, e):
    element_with_index = [(i, item[e]) for i, item in enumerate(data)]
    elements = [item[1] for item in element_with_index]

    minimum = min(elements)
    maximum = max(elements)

    sorted_elements = sorted(elements)
    n = len(elements)
    med = n // 2 - (n + 1) % 2
    median = sorted_elements[med]

    min_index = elements.index(minimum)
    max_index = elements.index(maximum)
    median_index = elements.index(median)
    avg = sum(elements) / len(elements)
    q = [statistics.quantiles(elements, n=4)[0], statistics.quantiles(elements, n=4)[2]]
    return (minimum, min_index), (maximum, max_index), avg, (median, median_index), q, sum(elements)


def print_data(name, data):
    print(name)
    print(f"min = {data[0][0]}; max = {data[1][0]}; avg = {data[2]}; median = {data[3][0]}; sum = {data[5]}")


def main():
    input_path = get_path()

    data = read_data(input_path)
    peaks_data = calculate_statistics_with_index(data, 2)
    print_data("Peaks data", peaks_data)
    outside_peaks_data = calculate_statistics_with_index(data, 3)
    print_data("Outside peaks data", outside_peaks_data)
    percentage_data = calculate_statistics_with_index(data, 4)
    print_data("Percentage data", percentage_data)

    min_lat = min(data, key=lambda x: x[0])[0]
    max_lat = max(data, key=lambda x: x[0])[0]
    min_lon = min(data, key=lambda x: x[1])[1]
    max_lon = max(data, key=lambda x: x[1])[1]

    lin = max_lat - min_lat + 1
    col = max_lon - min_lon + 1
    array = np.random.rand(max_lat - min_lat + 1, max_lon - min_lon + 1)

    for d in data:
        array[lin - (d[0] - min_lat) - 1][d[1] - min_lon] = d[3]

    norm = Normalize(vmin=np.min(array), vmax=np.max(array))

    plt.imshow(array, cmap='viridis', norm=norm)
    plt.colorbar(shrink=0.5)
    plt.axis('off')  # Remove the axis
    plt.savefig("heatmap.png")


if __name__ == "__main__":
    main()
