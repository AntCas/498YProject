# How to Run

1) clone the main ns3 project from https://github.com/nsnam/ns-3-dev-git

~$ git clone https://github.com/nsnam/ns-3-dev-git

1a) Make sure that the project builds without errors

~$ ./waf build

1b) navigate to the scratch directory

~$ cd ~/ns-3-dev-git/scratch

2) Clone this project

~$ git clone https://github.com/AntCas/498YProject

2a) Run the setup script

~$ ./modify_ns3_files.sh

2b) rebuild the project

~$ ./waf build

3) Run the tests

~$ ./498Y --do_rts=[true|false]
