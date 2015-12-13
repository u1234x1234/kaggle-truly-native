#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <map>
#include <sstream>
#include <cctype>
#include <a.out.h>
#include <functional>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std;

int main()
{
	string root_path = "/root/native/";

	vector<pair<string, int>> words;
	ifstream in(root_path + "/cnt_na");
	string word;
	int cnt;
	while(in >> word >> cnt)
	{
		words.push_back(make_pair(word, cnt));
	}
	cnt = 0;
	sort(words.begin(), words.end(), [](const pair<string, int> &l, const pair<string, int> &r){return l.second > r.second;});

	unordered_map<string, int> word_id;
	unordered_map<int, int> idf;
	int cntq = 0;
	ofstream word_id_out(root_path + "/word_id_na");
	unordered_set<string> site_ext = {"com", "net", "ru", "org", "de", "jp", "uk", "br", "pl", "in", "it", "fr", "info", "au", "nl", "cn", "ir", "es", "biz", "kz", "kr", "ka", "eu", "ua", "za", "co", "gr", "ro", "se"};
	int n = 4;
	for (int i = 0; i < 10000000; i++)
	{
		if (words[i].second < 3)
			continue;
		string w = words[i].first;
		int p = w.find_last_of(".");
		if (p != string::npos)
		{
			string ext = w.substr(p + 1, 5);
			if (site_ext.find(ext) != site_ext.end())
			{
				cntq++;
				word_id[words[i].first] = n;
				idf[n] = log(410000. / words[i].second) / 12.;
				word_id_out
//				cout
				<< n << " " << words[i].first << " " << words[i].second << " " << log(410000. / words[i].second) << "\n";
				n++;
			}
		}
	}
	for (int i = 0; i < 5000000; i++)
	{
		string w = words[i].first;
		if (word_id.find(w) != word_id.end())
			continue;
		word_id[w] = n;
		idf[n] = log(410000. / words[i].second) / 12.;
		word_id_out
//		cout
			<< n << " " << words[i].first << " " << words[i].second << " " << log(410000. / words[i].second) << "\n";
		n++;
	}
	cout << "n: " << n << endl;
	cout << "cntq: " << cntq << endl;
	words.clear();
	cout << "unique features: " << word_id.size() << endl;

	unordered_map<string, int> path_to_folder;
	boost::filesystem::directory_iterator end_itr;
	for(int folder = 0; folder < 6; folder++)
	{
		boost::filesystem::path pth = root_path + "/data/" + to_string(folder);
		if (boost::filesystem::exists(pth))
			for (boost::filesystem::directory_iterator itr(pth); itr != end_itr; ++itr)
			{
				string current_file = itr->path().filename().string();
				path_to_folder[current_file] = folder;
			}
	}
	cout << path_to_folder.size() << endl;

	auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	vector<string> modes = {"train", "test"};
	for (auto mode : modes)
	{
		cnt = 0;
		ifstream file_list;
		if (mode == "train")
			file_list.open(root_path + "train_v2.csv");
		else
			file_list.open(root_path + "sampleSubmission_v2.csv");

		string line;
		getline(file_list, line);
		vector<pair<string, int>> files;
		while(getline(file_list, line))
		{
			vector<string> tokens;
			boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(","));
			files.push_back(make_pair(tokens[0], stoi(tokens[1])));
		}
		cout << "mode: " << mode << ". file num: " << files.size() << endl;

		ofstream out_indices(root_path + "/indices." + mode + "na");
		ofstream out_indptr(root_path + "/indptr." + mode + "na");
		ofstream out_data(root_path + "/data." + mode + "na");
		int size = 0;
		out_indptr.write(reinterpret_cast<char*>(&size), sizeof(size));
		for (auto file : files)
		{
			string file_name = file.first;
			ifstream in(root_path + "/ref_na/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			// hand-crafted features
			int file_size_ref = boost::filesystem::file_size(root_path + "/ref_na/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			int file_size_data = boost::filesystem::file_size(root_path + "/data/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			int token_number = 0;

			map<int, int> tf;
			while(in >> word)
			{
				auto f = word_id.find(word);
				if (f == word_id.end())
					continue;
				int id = word_id[word];
				tf[id]++;
				token_number++;
			}
			auto write_feature = [&out_indices, &out_data](int feature_id, float feature_val)
			{
				out_indices.write(reinterpret_cast<char*>(&feature_id), sizeof(feature_id));
				out_data.write(reinterpret_cast<char*>(&feature_val), sizeof(feature_val));
			};
			write_feature(0, file_size_ref);
			write_feature(1, file_size_data);
			write_feature(2, token_number);
			write_feature(3, tf.size());
			for (auto &it : tf)
			{
				int feature_id = it.first;
				float feature_val = it.second;
				out_indices.write(reinterpret_cast<char*>(&feature_id), sizeof(feature_id));
				out_data.write(reinterpret_cast<char*>(&feature_val), sizeof(feature_val));
			}
			size += (tf.size() + 4);
			out_indptr.write(reinterpret_cast<char*>(&size), sizeof(size));

			cnt++;
			if (cnt % 1000 == 0)
			{
				cout << cnt << " " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start << endl;
				start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
		}
	}
	return 0;
}
