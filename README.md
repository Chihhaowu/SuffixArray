# SuffixArray
Implementation of a naive and accelerated algorithm to query a suffix array written in C++. Link to assignment specifications: https://rob-p.github.io/CMSC858D_S22/assignments/02_suffix_arrays

buildSA.cpp : constructing the suffix array and an auxiliary index

  The input consists of one optional parameter, as well as 2 required arguments, given in this order:

  --preftab <k> - if the option --preftab is passed to the buildsa executable (with the parameter k), then a prefix table will be built atop the suffix   array, capable of jumping to the suffix array interval corresponding to any prefix of length k. reference - the path to a FASTA format file containing the reference of which you will build the suffix array. output - the program will write a single binary output file to a file with this name, that contains a serialized version of the input string and the suffix array.

querySA.cpp : querying the suffix array
  
  The input consists of one optional parameter, as well as 2 required arguments, given in this order:
  
  index - the path to the binary file containing your serialized suffix array (as written by buildsa above).
queries - the path to an input file in FASTA format containing a set of records. You will need to care about both the name and sequence of these fasta records, as you will report the output using the name that appears for a record. Note, query sequences can span more than one line (headers will always be on one line). query mode - this argument should be one of two strings; either naive or simpaccel. If the string is naive you should perform your queries using the naive binary search algorithm. If the string is simpaccel you should perform your queries using the “simple accelerant” algorithm we covered in class. Note: If you are reading a serialized input file with no prefix lookup table, then these algorithms are to be run on each query on the full suffix array, and if you are reading in a prefix lookup table as well, then this is the algorithm that should be used on the relevant interval for each query.
output - the name to use for the resulting output.

Resources used for this assignment:
  
  Construction of the Suffix Array, https://github.com/IlyaGrebnov/libsais
  
  Searching the Array, (modified) binary search https://github.com/BenLangmead/comp-genomics-class
  
  Serialisation, https://uscilab.github.io/cereal/
  
  Parsing FASTA formatted sequences, https://rosettacode.org/wiki/FASTA_format#C.2B.2B
