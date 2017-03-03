# OpenMp-MPI-project
The goal of this project is to analyze a stream of messages and replies (i.e., comments) coming from a social network. 
The project was inspired by the DEBS 2016 Grand Challenge and follow the same format for data.
In particular, the input data is organized in two separate streams, each provided as a text file sorted based on the timestamp field.
The solution leverages OpenMP and MPI to take advantage of a set of hosts/cores in producing the expected result.
