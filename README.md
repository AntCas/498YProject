# How to Run

1. Set up ns3 (if not already set up)

   * clone the main ns3 project from https://github.com/nsnam/ns-3-dev-git

`git clone https://github.com/nsnam/ns-3-dev-git`

   *  Make sure that the project builds without errors
   
`./waf build`

2. Set up this project

   * navigate to the scratch directory

`cd <ns3-root>/scratch/ `

   * copy this repository into the scract directory

   * Run the setup script
   
`./modify_ns3_files.sh`

3. Have python2, numpy, and matplotlib installed. I used gnome-open to open .png files, you may need to sub in some photo-viewer. I also used zsh's globbing syntax to find all .png files, you may need to rewrite using ```find ... -exec {} ...``` or open manually.

## Usage:

1. rebuild the project

`./waf build`

2. Run the tests

`./498Y --do_rts=[true|false]`

`../waf build`

   * Run test one
   
`cd one`

`./run_one.sh`

   * Run test two
   
`cd two`

`./run_two.sh`

   * Run Test three
   
`cd three`

`./run_three.sh`

   * View the results

`gnome-open **/**.png`
