import sys
import simplekml
import argparse

INPUT_FILE = "../data/RomaniaANS/tree.txt"


def get_path():
    if len(sys.argv) == 1:
        return INPUT_FILE

    parser = argparse.ArgumentParser(description="Generate TileTree KML")
    parser.add_argument('path', type=str, help='Path to the TileTree file')
    args = parser.parse_args()

    return args.path


def create_tree_kml(nodes, edges):
    kml = simplekml.Kml()

    point_style = simplekml.Style()
    point_style.iconstyle.icon.href = 'https://maps.google.com/mapfiles/kml/shapes/placemark_circle_highlight.png'
    point_style.iconstyle.scale = 1.5
    point_style.labelstyle.color = simplekml.Color.red
    point_style.labelstyle.scale = 0

    for (i, n) in enumerate(nodes):
        point = kml.newpoint(name=f"Node {i}",
                             description=f"lat: {n[0]}<br>"
                                         f"lon: {n[1]}<br>"
                                         f"PB DB search: = <a href=https://peakbagger.com/search.aspx?tid=R&lat={n[0]}&lon={n[1]}>Peak</a>",
                             coords=[(n[1], n[0])])
        point.style = point_style

    for (i, e) in enumerate(edges):
        kml.newlinestring(name=f"Edge {i}",
                          description="",
                          coords=[(e[0][1], e[0][0]), (e[1][1], e[1][0])])

    kml.save("tree.kml")


def get_tree(input_path):
    with open(input_path, 'r') as file:
        lines = file.readlines()

    lines = [line.split(',') for line in lines]
    nodes = []
    edges = []
    for line in lines:
        if line[0] == 'n':
            nodes.append((float(line[1]), float(line[2])))
        else:
            edges.append((nodes[int(line[1])], nodes[int(line[2])]))

    return nodes, edges


def main():
    input_path = get_path()
    nodes, edges = get_tree(input_path)
    create_tree_kml(nodes, edges)


if __name__ == "__main__":
    main()
