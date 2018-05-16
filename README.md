# bpistats


## Build instructions

Requirements:
 - cmake 3.10 (or greater, recommend 3.11.1)
    - instructions, below, assume installation to:  ~/bin/cmake_3_11_1/bin/cmake
 - boost 1.66 (or greater, recommend 1.67.0)
    - set up BOOST_ROOT environment variable to point to isntallation folder (or use "shortcut" below)
    - can install with `toolchain/install-boost.sh -h`
    - instructions, below, assume installation to:  ~/bin/boost/1_67_0
 - googletest (auto installs on make)

### Run cmake (with BOOST_ROOT):
```
git clone https://github.com/deanaddison/bpistats.git
cd bpistats
mkdir build
cd build
```

If you have BOOST_ROOT set appropriately and cmake in PATH, then (from build folder):
```
cmake ..
```
Otherwise, you can use this "shortcut" (from build folder):
```
BOOST_ROOT=~/bin/boost/1_67_0 ~/bin/cmake_3_11_1/bin/cmake ..
```

### Make the application and tests:
```
make
```

### Run the application:
```
bpistats 2018-04-10 2018-05-10
```
Usage:
```
Usage:
    bpistats [options | path | begin_date end_date]

Options:
  -h [ --help ]        usage for this application (this info).

  path                 Analyses the coindesk bitcoin historical close JSON from a local
                       file.

  begin_date end_date  Fetches and analyses the bitcoin historical close data for the
                       inclusive date range (YYYY-MM-DD)

```
Application modes:
 1.  Fetch local file:  _bpistats file.json_
        - _file.json_ is the path to a local file
        - contains the JSON object in the same format as that returned from:  https://api.coindesk.com/v1/bpi/historical/close.json
 1.  Fetch from coindesk url with date-range:  bpistats begin_date end_date
        - _begin_date_, _end_date_:  ISO 8601 format, _YYYY-MM-DD_ dates
        - e.g. _bpistats 2018-04-10 2018-05-10_
            - fetches the JSON object from:  https://api.coindesk.com/v1/bpi/historical/close.json?start=2018-04-10&end=2018-05-10

### Run the tests:
```
bpistats_tests
```

## Acknowledgements:
This repository was intentionally intended to mimic the [Bluzelle](https://bluzelle.com) [swarmDB](https://github.com/bluzelle/swarmDB) development environment.
It uses the toolchain/install-boost.sh bash scripts from there as well as the [googletest cmake helper functions](https://github.com/bluzelle/swarmDB/tree/devel/cmake).
