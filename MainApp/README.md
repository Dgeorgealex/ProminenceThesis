The main prominence algorithm. It is able to compute mountain prominence starting from SRTM data.

```
Usage: prominence.exe [OPTIONS] minLat maxLat minLon maxLon

Positionals:
  minLat INT REQUIRED         Minimum latitude
  maxLat INT REQUIRED         Maximum latitude
  minLon INT REQUIRED         Minimum longitude
  maxLon INT REQUIRED         Maximum longitude

Options:
  -h,--help                   Print this help message and exit
  -i,--inputDir TEXT          Input directory, default = "."
  -o,--outputDir TEXT         Output directory, default = "."
  -d,--dataType TEXT          Data type: "SRTM30", "SRTM90", default = "SRTM90"
  -p,--pThreshold INT         Prominence threshold, default = 0
  -t,--threadNum INT          Number of threads used, default = 1
  -e,--expTree                Export tree flag, default = false

```
