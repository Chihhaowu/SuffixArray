// buildsa

#include <iostream>
#include <fstream>
#include "libsais.h"
#include "cereal/archives/binary.hpp"
#include <chrono>
#include <map>

using namespace std;

int main(int argc, char* argv[]) {
	
	clock_t start, end;
	int k;
	string outFileName;

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << "--preftab <k> reference output" << std::endl;
		return 1;
	}

	k = stoi(argv[1]);
	outFileName = argv[3];

	/* First, create a constant array which elements are the characters
	 * of the string taken as the input. Then, create a suffix array from
	 * this using the method defined in the libsais library.
	 */
	string readString;

	/* script to read Fasta formatted sequence files; modified from
	 * https://rosettacode.org/wiki/FASTA_format#C.2B.2B
	 */
	ifstream input(argv[2]);
	string line, name, content;

	while (getline(input, line).good()) {
		if (line.empty() || line[0] == '>') {
			if (!name.empty()) {
				cout << name  << " : " << content << endl;
				name.clear();
			}
			if (!line.empty()) {
				name = line.substr(1);
			}
			content.clear();
			} else if (!name.empty()) {
				if (line.find(' ') != string::npos) {
					name.clear();
					content.clear();
				} else {
					content += line;
				}
			}
		}
	if (!name.empty()) { 
		readString = content;
	}

	const uint8_t * characterArray = reinterpret_cast<const uint8_t*>(readString.c_str());
	//cout << readString.size();
	unsigned long int len = readString.length() + 1;

	// save string to file
	ofstream file1("string.bin", ofstream::binary);
	file1.write(readString.c_str(), readString.size());
	file1.close();
		
	/* hardcoded value as workaround for serialisation problem;
	 * corresponds to the number of elements in the suffix array,
	 * length of reference plus sentinel character.
	 */
	// CHANGE INT HERE
	const int lenSerialisedArray = 4639676;
	int32_t * suffixArray = new int32_t[lenSerialisedArray];

	start = clock();
	libsais(characterArray, suffixArray, len, 0, NULL);
	
	/* optionally construct the prefix table / auxiliary index
	 * walk the suffix array, if the first k characters are different, then start
	 * a new interval
	 */
	//cout << readString.substr(suffixArray[1],k) << endl;
	map<string, int> mapOfPrefixesLowerBound;	
	map<string, int> mapOfPrefixesUpperBound;
  	int previousIndex = 0;
	string previousString = "";
	string currentString;
        if (k > 0) { 
		for (int i=0; i<lenSerialisedArray; i++) {
			currentString = readString.substr(suffixArray[i], k);
			if (currentString.compare(previousString) != 0) {
				previousString = currentString;
				//cout << i+1 << ":" << previousIndex << " " << currentString << endl;  
				mapOfPrefixesLowerBound.insert(make_pair(currentString, previousIndex));
				mapOfPrefixesUpperBound.insert(make_pair(currentString, i+1));
				previousIndex = i;
			}
		}
	}
	//auto lb =  mapOfPrefixesLowerBound.find("AAA"), ub = mapOfPrefixesUpperBound.find("AAA");
	//cout <<  lb->first << " = " << lb->second << ","  << ub->second << endl; 
	
	// serialise and write SA
	{
		ofstream out(outFileName, ios::binary);
		cereal::BinaryOutputArchive oarchive(out);
		int32_t * data = new int32_t[lenSerialisedArray];
		for (int i=0; i<len; i++) {
			data[i] = suffixArray[i];
		}
		oarchive( cereal::binary_data( data, sizeof(int32_t) * len ) );
	}

	end = clock();
	printf ("Construction time: %0.8f sec \n",((float) end - start)/CLOCKS_PER_SEC);
	
	return 0;
}

