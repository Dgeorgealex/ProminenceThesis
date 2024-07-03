import argparse
import sys

import simplekml

THRESHOLD = 0.0005
INPUT_FILE = './results.txt'
NAME_FILE = './names.txt'
PROM_THRESHOLD = 600


def get_parameters():
    global INPUT_FILE, NAME_FILE, PROM_THRESHOLD
    if len(sys.argv) == 1:
        return

    parser = argparse.ArgumentParser(description="Generate mountain KML and list")
    parser.add_argument('path', type=str, help='Path to results')
    parser.add_argument('-n', type=str, help='Path to names')
    parser.add_argument('-t', type=int, help='Prominence threshold')

    args = parser.parse_args()

    INPUT_FILE = args.path

    if args.n is not None:
        NAME_FILE = args.n

    if args.t is not None:
        PROM_THRESHOLD = args.t


def get_mountains():
    with open(INPUT_FILE, 'r') as file:
        lines = file.readlines()

    lines = [line.split(',') for line in lines]
    mountains = []
    for line in lines:
        m = {
            'lat': float(line[0]),
            'lon': float(line[1]),
            'ele': int(line[2]),
            'prom': int(line[3]),
            'p_lat': float(line[4]),
            'p_lon': float(line[5]),
            'name': ''
        }
        mountains.append(m)

    return mountains


def acceptable(name, mountain):
    if mountain['ele'] - 15 > name[0]:
        return False

    if name[0] - mountain['ele'] > 100:
        return False

    dx = mountain['lat'] - name[2]
    dy = mountain['lon'] - name[3]

    if dx * dx + dy * dy > THRESHOLD:
        return False

    return True


def create_description(m):
    return (f"<b>prom: {m['prom']}</font></b><br>"
            f"ele: {m['ele']}</font><br>"
            f"lat: {m['lat']}</font><br>"
            f"lon: {m['lon']}</font><br>"
            f"p_lat: {m['p_lat']}</font><br>"
            f"p_lon: {m['p_lon']}</font><br>"
            f"PB DB search: = <a href=https://peakbagger.com/search.aspx?tid=R&lat={m['lat']}&lon={m['lon']}>Peak</a"
            f">, <a href=https://peakbagger.com/search.aspx?tid=R&lat={m['p_lat']}&lon={m['p_lon']}>PPeak</a></font>")


def generate_kml(mountains):
    kml = simplekml.Kml()
    point_style = simplekml.Style()
    point_style.iconstyle.icon.href = 'https://maps.google.com/mapfiles/kml/shapes/placemark_circle_highlight.png'
    point_style.iconstyle.scale = 1.5
    point_style.labelstyle.color = simplekml.Color.red
    point_style.labelstyle.scale = 0

    for m in mountains:
        if m['name'] != '+' and m['prom'] >= PROM_THRESHOLD:
            point = kml.newpoint(name=f"{m['name']}",
                                 description=create_description(m),
                                 coords=[(m['lon'], m['lat'])])
            point.style = point_style

    kml.save("mountains.kml")


def print_mountains(mountains):
    mountains.sort(key=lambda x: x['prom'], reverse=True)
    with open('mountains.txt', 'w', encoding='utf-8') as f:
        cnt = 0
        for m in mountains:
            if m['name'] != '+' and m['prom'] >= PROM_THRESHOLD:
                cnt += 1
                print(f"{cnt},{m['name']},{m['prom']},{m['ele']}", file=f)


def name_mountains(mountains):
    if NAME_FILE == '':
        return mountains

    names = []

    with open(NAME_FILE, encoding='utf-8') as f:
        lines = f.readlines()
        for line in lines:
            values = line.split(',')
            names.append((int(values[0]), values[1], float(values[2]), float(values[3])))
        names.sort(key=lambda x: x[0], reverse=True)

    unnamed = 0
    for m in mountains:
        best_name = None
        for name in names:
            if acceptable(name, m):
                best_name = name
                break

        if best_name is not None:
            delta = best_name[0] - m['ele']
            m['ele'] = best_name[0]
            m['prom'] += delta
            m['name'] = best_name[1]
        else:
            m['name'] = '+'
            unnamed += 1

    print(f"Unnamed = {unnamed}")
    return mountains


def main():
    get_parameters()
    mountains = get_mountains()
    mountains = name_mountains(mountains)
    generate_kml(mountains)
    print_mountains(mountains)


if __name__ == "__main__":
    main()
