# How to Run

Place this repo inside <ns3-root>/scratch/

Have python2, numpy, and matplotlib installed. I used gnome-open to open .png files, you may need to sub in some photo-viewer.
I also used zsh's globbing syntax to find all .png files, you may need to rewrite using 'find ... -exec {} ...' or open manually.

## Usage:

./modify_ns3_files.sh
../waf build

cd one
./run_one.sh

cd two
./run_two.sh

cd three
./run_three.sh

gnome-open **/**.png
