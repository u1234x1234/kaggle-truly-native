#include <iostream>
#include <fstream>
#include <string>
#include <future>
#include <cstdlib>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <cctype>
#include <functional>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "../xgboost/wrapper/xgboost_wrapper.h"

using namespace std;

int main(int argc, char *argv[])
{
	string root_path = "/root/native/";
	string mode(argv[1]);
	vector<unsigned long> indptr;
	vector<unsigned int> indices;
	vector<float> data;
	int tmp_i;
	float tmp_f;
	ifstream in1(root_path + "/" + mode + ".indptr");
	while(in1.read(reinterpret_cast<char*>(&tmp_i), sizeof(unsigned int)))
		indptr.push_back(tmp_i);
	cout << "indptr loaded: " << indptr.size() << endl;
	cout << "indptr " << indptr[0] << " " << indptr[indptr.size() - 1] << endl;
	ifstream in2(root_path + "/" + mode + ".indices");
	while(in2.read(reinterpret_cast<char*>(&tmp_i), sizeof(int)))
		indices.push_back(tmp_i);
	cout << "indices loaded " << indices.size() << endl;
	cout << "ind: " << indices[0] << endl;
	ifstream in3(root_path + "/" + mode + ".data");
	while(in3.read(reinterpret_cast<char*>(&tmp_f), sizeof(float)))
		data.push_back(tmp_f);
	cout << "data loaded: " << data.size() << endl;
	cout << "data: " << data[0] << endl;

	DMatrixHandle *out = new DMatrixHandle();
	XGDMatrixCreateFromCSR(indptr.data(), indices.data(), data.data(), indptr.size(), data.size(), out);
	cout << "dmat created" << endl;
	XGDMatrixSaveBinary(*out, (root_path + "/" + mode + ".dmat").c_str(), 0);
	cout << "saved" << endl;

	return 0;
}
