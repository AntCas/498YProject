# How to Run

1. Set up ns3 (if not already set up)

   * clone the main ns3 project from https://github.com/nsnam/ns-3-dev-git

   `git clone https://github.com/nsnam/ns-3-dev-git`

   *  Make sure that the project builds without errors
   
   `./waf build`

2. Install dependencies

   You will need python2, numpy, and matplotlib installed in order to create a view the graphs. We used gnome-open to open .png files, you may need to sub in some other photo-viewer depending on your operating system if gnome-open is not available. We also used zsh's globbing syntax to find all .png files, which may need to be rewritenn using ```find ... -exec {} ...``` or opened manually.

3. Set up this project

   * navigate to the scratch directory

   `cd <ns3-root>/scratch/ `

   * copy the *contents* of this repository into the scratch directory

   * Run the setup script
   
   `./modify_ns3_files.sh`

   * Rebuild ns3
   
   `./waf build`
 

## Usage:

1. Rebuild the project

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
