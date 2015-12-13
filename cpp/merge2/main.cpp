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


int main()
{
	string root_path = "/root/native/";
	string md = "test";
	vector<unsigned long> indptr_site, indptr_na, indptr_space, indptr_hc, indptr_bigram;
	vector<unsigned int> indices_site, indices_na, indices_space, indices_hc, indices_bigram;
	vector<float> data_site, data_na, data_space, data_hc, data_bigram;

	auto read_csr = [&root_path](string prefix, vector<unsigned long> &indptr, vector<unsigned int> &indices, vector<float> &data) -> void
	{
		cout << prefix << " loading.." << endl;
		int tmp_i;
		float tmp_f;
		ifstream in1(root_path + "/" + prefix + ".indptr");
		while(in1.read(reinterpret_cast<char*>(&tmp_i), sizeof(unsigned int)))
			indptr.push_back(tmp_i);
		cout << "indptr loaded: " << indptr.size() << endl;
		ifstream in2(root_path + "/" + prefix + ".indices");
		while(in2.read(reinterpret_cast<char*>(&tmp_i), sizeof(int)))
			indices.push_back(tmp_i);
		cout << "indices loaded " << indices.size() << endl;
		ifstream in3(root_path + "/" + prefix + ".data");
		while(in3.read(reinterpret_cast<char*>(&tmp_f), sizeof(float)))
			data.push_back(tmp_f);
		cout << "data loaded: " << data.size() << endl;
		cout << prefix << " loaded." << endl << endl;
	};
	read_csr("sep_site_" + md, indptr_site, indices_site, data_site);
	read_csr("sep_na_" + md, indptr_na, indices_na, data_na);
	read_csr("sep_space_" + md, indptr_space, indices_space, data_space);
	read_csr("sep_hc_" + md, indptr_hc, indices_hc, data_hc);
	read_csr("sep_bigram_" + md, indptr_bigram, indices_bigram, data_bigram);

	int st_site = 1000;
	int en_site = 1141406;	//1141406
	int st_na = 0;
	int en_na = 0;
	int st_space = 1000;
	int en_space = 300000;
	int st_hc = 0;
	int en_hc = 0;
	int st_bi = 0;
	int en_bi = 1000000;

	string mds = "test";
	ofstream out_indices(root_path + "/sep_joint_" + mds + ".indices");
	ofstream out_indptr(root_path + "/sep_joint_" + mds + ".indptr");
	ofstream out_data(root_path + "/sep_joint_" + mds + ".data");
	int size = 0;
	out_indptr.write(reinterpret_cast<char*>(&size), sizeof(size));
	int st = 0;
	int en = 337024;
	for (size_t i = 0; i < indptr_site.size() - 1; i++)
	{
		int row_size = 0;
		for (int j = indptr_site[i]; j < indptr_site[i + 1]; j++)
		{
			int col = indices_site[j];
			float val = data_site[j];
			if (col < st_site || col >= en_site)
				continue;
			col -= st_site;
			out_indices.write(reinterpret_cast<char*>(&col), sizeof(col));
			out_data.write(reinterpret_cast<char*>(&val), sizeof(val));
			row_size++;
		}
		for (int j = indptr_na[i]; j < indptr_na[i + 1]; j++)
		{
			int col = indices_na[j];
			float val = data_na[j];
			if (col < st_na || col >= en_na)
				continue;
			col -= st_na;
			col += (en_site - st_site);
			out_indices.write(reinterpret_cast<char*>(&col), sizeof(col));
			out_data.write(reinterpret_cast<char*>(&val), sizeof(val));
			row_size++;
		}
		for (int j = indptr_space[i]; j < indptr_space[i + 1]; j++)
		{
			int col = indices_space[j];
			float val = data_space[j];
			if (col < st_space || col >= en_space)
				continue;
			col -= st_space;
			col += (en_site - st_site) + (en_na - st_na);
			out_indices.write(reinterpret_cast<char*>(&col), sizeof(col));
			out_data.write(reinterpret_cast<char*>(&val), sizeof(val));
			row_size++;
		}
		for (int j = indptr_hc[i]; j < indptr_hc[i + 1]; j++)
		{
			int col = indices_hc[j];
			float val = data_hc[j];
			if (col < st_hc || col >= en_hc)
				continue;
			col -= st_hc;
			col += (en_site - st_site) + (en_na - st_na) + (en_space - st_space);
			out_indices.write(reinterpret_cast<char*>(&col), sizeof(col));
			out_data.write(reinterpret_cast<char*>(&val), sizeof(val));
			row_size++;
		}
		for (int j = indptr_bigram[i]; j < indptr_bigram[i + 1]; j++)
		{
			int col = indices_bigram[j];
			float val = data_bigram[j];
			if (col < st_bi || col >= en_bi)
				continue;
			col -= st_bi;
			col += (en_site - st_site) + (en_na - st_na) + (en_space - st_space) + (en_hc - st_hc);
			out_indices.write(reinterpret_cast<char*>(&col), sizeof(col));
			out_data.write(reinterpret_cast<char*>(&val), sizeof(val));
			row_size++;
		}

		size += row_size;
		if (i % 10000 == 0)
			cout << i << " " << size << endl;
		out_indptr.write(reinterpret_cast<char*>(&size), sizeof(size));
	}
	cout << size << endl;

	return 0;
}
