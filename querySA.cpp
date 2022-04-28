// querysa

#include <iostream>
#include <fstream>
#include "libsais.h"
#include "cereal/archives/binary.hpp"
#include <cstring>
#include <chrono>
#include <map>

using namespace std;

int binarySearch(int arr[], string str, string x, int lb, int ub) {

	int low = lb;//0;
	int high = ub;//str.length() - 1;
	int mid = (low + high) / 2;

	//cout << low << " " << mid << " " << high << endl;

	while (low < high) {

		//cout << mid << " " << x << " " << str.substr (arr[mid]) << endl; 
		int compareString = str.substr (arr[mid]).compare(x); 
	
		if (compareString > 0) {
			if (mid == low + 1) {
				//return mid;
				return (str.substr(arr[mid]).find(x.substr(0,x.length()-1)) == 0 || str.substr(arr[low]).find(x.substr(0,x.length()-1)) == 0) ? mid : -1;
			} else {
				high = mid;
			}
		
		} else if (compareString < 0) {
			if (mid == high - 1) {
				//return high;
				return (str.substr(arr[mid]).find(x.substr(0,x.length()-1)) == 0 || str.substr(arr[high]).find(x.substr(0,x.length()-1)) == 0) ? high : -1;
			} else {
				low = mid;
			}
		}
		mid = (high + low) / 2;
	}
	return mid;
}

int lcpWithQuery(string str, string suffix) {
	
	int count = 0;
	int len = min(str.length(), suffix.length());
	for (int i=0; i<len; i++) {
		if (str.at(i) == suffix.at(i)) { 
			count++;
		} else {
			break;
		}
	}
	return count;
}

int simpleAccel(int arr[], string str, string pattern, int lb, int ub) {

	int low = lb;
	int high = ub;
	int mid = (low + high) / 2;
	int lcp_lp, lcp_pr, lcp_pc;
	int skip = 0;

	lcp_lp = lcpWithQuery(pattern, str.substr(arr[low]));
	lcp_pc = lcpWithQuery(pattern, str.substr(arr[mid]));
	lcp_pr = lcpWithQuery(pattern, str.substr(arr[high]));

	while (low < high)  {

		skip = min(lcp_lp, lcp_pr);				
		//cout << "skip " << skip << " " << pattern << " " <<  str.substr(arr[low]) << " " << lcp_lp << " " << str.substr(arr[high]) << " " << lcp_pr << endl;
		//cout << pattern << " " << pattern.substr(skip) << str.substr(arr[mid]) << " " << str.substr(arr[mid]+skip) << endl;
		int compareString = str.substr(arr[mid]+skip).compare(pattern.substr(skip)); 

		if (compareString > 0) {
			if (mid == low + 1) {
				//return mid;
				return (str.substr(arr[mid]).find(pattern.substr(0,pattern.length()-1)) == 0 || str.substr(arr[low]).find(pattern.substr(0,pattern.length()-1)) == 0) ? mid : -1;
			} else {
				high = mid;
				lcp_pr = lcp_pc;
			}
		} else if (compareString < 0) {
			if (mid == high - 1) {
				//return high;
				return (str.substr(arr[mid]).find(pattern.substr(0,pattern.length()-1)) == 0 || str.substr(arr[high]).find(pattern.substr(0,pattern.length()-1)) == 0) ? high : -1;
			} else { 
				low = mid;
				lcp_lp = lcp_pc;
			}
		}
		mid = (high + low) /2;
		lcp_pc = lcpWithQuery(pattern, str.substr(arr[mid]));
	}
	return mid;
}

int main(int argc, char* argv[]) {

	string indexFileName, mode, outFileName;
	clock_t start, end;

	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << "<index> <queries> <query mode> <output>" << std::endl;
		return 1;	
	} else {
		indexFileName = argv[1];
		mode = argv[3];
		outFileName = argv[4];
	}

	// read reference string from file
	ifstream file1("string.bin", ios::in | ios::binary);
 	size_t stringLength = 4639675;
 	char* temp = new char[stringLength+1];
 	file1.read(temp, stringLength);
 	temp[stringLength] = '\0';
 	delete [] temp;
 	string str = temp;

	// hard-coded value for length of SA to read in
	const int numFix = 4639676;
	int32_t * SA = new int32_t[numFix];

	// deserialise SA from file
	{
		ifstream in(indexFileName, ios::binary);
		cereal::BinaryInputArchive iarchive(in);
		iarchive( cereal::binary_data( SA, sizeof(int32_t) * numFix) );
	}
	
	// read in query file and iterate for all entries
	string line, name, content;
	string * queryAccession = new string[10000];
	string * queryString = new string[10000];
	ifstream input(argv[2]);
	
	int countQuery = 0;

	while (getline(input, line).good()) {
		if (line.empty() || line[0] == '>') {
			if (!name.empty()) {
				queryAccession[countQuery] = name;
				queryString[countQuery] = content;
				countQuery ++;
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
		queryAccession[countQuery] = name;
		queryString[countQuery] = content;	
	}

	//cout << queryAccession[9999] << " " << queryString[9999] << endl;

	/* Was having difficulty (de)serialising the precomputed prefix table, so
	 * instead, I am implementing it here by creating a mapping from the prefix 
	 * to the start and stop indicies in the SA
	 */
	map<string, int> mapOfPrefixesLowerBound;
	map<string, int> mapOfPrefixesUpperBound;
	int previousIndex = 0;
	string previousString = "";
	string currentString;
	// hard-coded value of k
	int k = 0;

	if (k > 0) {
		for (int i=0; i<4639676; i++) {
			currentString = str.substr(SA[i], k);
			if (currentString.compare(previousString) != 0) {
				previousString = currentString;
				mapOfPrefixesLowerBound.insert(make_pair(currentString, previousIndex));
				mapOfPrefixesUpperBound.insert(make_pair(currentString, i+1));
				previousIndex = i;
			}
		}
	}
				
	//auto lb =  mapOfPrefixesLowerBound.find("AAA"), ub = mapOfPrefixesUpperBound.find("AAA");
	//cout <<  lb->first << " = " << lb->second << ","  << ub->second << endl;
	
	// pass in  each query already loaded into memory
	int lb = 0, ub = str.length() - 1;

	start = clock();
	for (int i=0; i<100; i++) {
	
		int counter = 0;
		string queryL = queryString[i] + "#", queryR = queryString[i] + "{";
		//cout << queryL << endl;		
		
		if (k > 0) {
			string queryPrefix = queryString[i].substr(0,k);
			auto lbSearch =  mapOfPrefixesLowerBound.find(queryPrefix), ubSearch = mapOfPrefixesUpperBound.find(queryPrefix);
			lb = lbSearch->second;
			ub = ubSearch->second;
		}
	
		//cout << queryString[i].substr(0,k) <<" "<< lb << " " << ub << endl;

		int lowerBound, upperBound;			
	
		if (mode == "naive") {
			// naive, two binary searches
			lowerBound = binarySearch(SA, str, queryL, lb, ub); 
			upperBound = binarySearch(SA, str, queryR, lb, ub);
		} else if (mode == "simpaccel") {
			// simple acceleration
			lowerBound = simpleAccel(SA, str, queryL, lb, ub); 
			upperBound = simpleAccel(SA, str, queryR, lb, ub);
		}

		//cout << lowerBound << " " << upperBound << endl;
	
		int pos[(upperBound-lowerBound)+1];

		// position in of match(es) in string and counter for number of matches
		if (lowerBound != -1 && upperBound != -1) {
			for (int i=lowerBound, j=0; i<upperBound; i++, j++) {
				//cout << SA[i] << " ";
				pos[j] = SA[i];
				counter++; 
			}  
			if (upperBound == str.length() - 1) {
				cout << SA[upperBound];
				counter++;
			}
		} else if (lowerBound == -1 && upperBound != -1) {
			//cout << SA[upperBound] << " ";
			pos[0] = SA[upperBound];
			counter++;
		} else if (upperBound == -1 && lowerBound != -1) {
			//cout << SA[lowerBound] << " ";
			pos[0] = SA[lowerBound];
			counter++;
		}
	
		cout << queryAccession[i] << '\t' << counter << '\t';
		for (int i=0; i<counter; i++) {
			cout << pos[i] << '\t';
		} 
		cout << endl;
			
		//cout << i << endl;
	} 
	end = clock();
	printf ("Query time: %0.8f sec \n",((float) end - start)/CLOCKS_PER_SEC);

	return 0;
}
