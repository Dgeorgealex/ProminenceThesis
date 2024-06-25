import os
import argparse
import sys

DIRECTORY = "../data/RomaniaANS"


def get_directory():
    if len(sys.argv) == 1:
        return DIRECTORY

    parser = argparse.ArgumentParser(description="Append all the results into a single file.")
    parser.add_argument('directory', type=str, help='The path to the directory containing the results')
    args = parser.parse_args()
    return args.directory


def main():
    directory = get_directory()

    output_file = os.path.join('.', 'results.txt')

    with open(output_file, 'w') as outfile:
        for filename in os.listdir(directory):
            if filename.startswith('prominence-') and filename.endswith('.txt'):
                file_path = os.path.join(directory, filename)
                with open(file_path, 'r') as infile:
                    contents = infile.read()
                    outfile.write(contents)


if __name__ == "__main__":
    main()
