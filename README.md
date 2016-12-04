# How to Run

1. clone the main ns3 project from https://github.com/nsnam/ns-3-dev-git

`git clone https://github.com/nsnam/ns-3-dev-git`

* Make sure that the project builds without errors
`./waf build`

* navigate to the scratch directory

`cd ~/ns-3-dev-git/scratch`

2. Place this repo inside `<ns3-root>/scratch/ `

* Run the setup script
`./modify_ns3_files.sh`

3. Have python2, numpy, and matplotlib installed. I used gnome-open to open .png files, you may need to sub in some photo-viewer. I also used zsh's globbing syntax to find all .png files, you may need to rewrite using ```find ... -exec {} ...``` or open manually.

## Usage:

1. rebuild the project

`./waf build`

2. Run the tests

`./498Y --do_rts=[true|false]`

`../waf build`

`cd one`
`./run_one.sh`

`cd two`
`./run_two.sh`

`cd three`
`./run_three.sh`

gnome-open **/**.png
