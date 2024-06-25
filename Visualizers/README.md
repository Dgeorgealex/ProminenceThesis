# Visualizers

### mountains_kml

A tool to generate a KML and a list containing all the mountains from a run of the main algorithm. As a optional parameter, the user can specify a file that contains information about mountains in the form ```elevation,name,latitude,longitude``` and mountains_kml will match the names with results from the main algorithm and do a prominence correction step.

```
usage: mountains_kml.py [-h] [-n N] [-t T] path

Generate mountain KML and list

positional arguments:
  path        Path to results

options:
  -h, --help  show this help message and exit
  -n N        Path to names
  -t T        Prominence threshold
```


### tile_island

A tool that takes a .hgt file and a height and generates an image in which all the points that have an elevation over the given height are colored in black.

```
usage: tile_island.py [-h] path height

Visualize the tile islands

positional arguments:
  path        Path to the tile
  height      Height of the water

options:
  -h, --help  show this help message and exit
```

### tile_statistics

Starting from the info.txt file which resulted from running the the main algoritm and has the format ```latitude,longitude,#peaks,#outside_peaks``` it generates statistics such as minumum, maximum, median and average and also a heatmap for the user to see where are the most outside peaks.

```
usage: tile_statistics.py [-h] path

Compute tile statistics

positional arguments:
  path        Path to the info file

options:
  -h, --help  show this help message and exit
```


### tiletree_kml

Starting from a TileTree export file whcich results from the main algorithm and has the format ```n,latitude,longitude``` or ```e,id,id``` it generates a KML of the TileTree.

```
usage: tiletree_kml.py [-h] path

Generate TileTree KML

positional arguments:
  path        Path to the TileTree file

options:
  -h, --help  show this help message and exit
```
