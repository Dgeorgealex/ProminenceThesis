import argparse
import os
import sys
import geojson

INPUT_FILE = "../data/Romania.geojson"


def get_path():
    if len(sys.argv) == 1:
        return INPUT_FILE

    parser = argparse.ArgumentParser(description="Parse geoJSON")
    parser.add_argument('path', type=str, help='Path to the geoJSON')
    args = parser.parse_args()

    return args.path


def is_float(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


def extract_mountains(data):
    mountain_data = []

    # Iterate over each feature in the GeoJSON data
    for feature in data['features']:
        properties = feature['properties']
        coordinates = feature['geometry']['coordinates']

        # Check if 'ele' and 'name' properties exist
        if 'ele' in properties and 'name' in properties:
            point_info = {
                'ele': properties['ele'],
                'name': properties['name'],
                'coordinates': coordinates
            }
            mountain_data.append(point_info)

    return mountain_data


def read_geojson(input_path):
    with open(input_path, encoding='utf-8') as file:
        return geojson.load(file)


def main():
    input_path = get_path()
    output_path = os.path.join('.', 'names.txt')

    geojson_data = read_geojson(input_path)

    mountains = extract_mountains(geojson_data)

    with open(output_path, 'w', encoding='utf-8') as f:
        for mountain in mountains:
            if is_float(mountain['ele']) and float(mountain['ele']) > 300:
                print(
                    f"{int(float(mountain['ele']))},{mountain['name']},{mountain['coordinates'][1]},{mountain['coordinates'][0]}",
                    file=f)


if __name__ == "__main__":
    main()
