#NS_LOG='PartOne:level_info' 498YProject/498YProject 

mkdir a_rtscts
mkdir a_no_rtscts
mkdir b_rtscts
mkdir b_no_rtscts
rm a_rtscts/*
rm a_no_rtscts/*
rm b_rtscts/*
rm b_no_rtscts/*

../../build/scratch/one/one
python2 make_graph.py

