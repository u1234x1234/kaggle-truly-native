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
#include <functional>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std;
string root_path = "/root/native/";

unordered_map<string, int> word_id;
unordered_map<long long, int> bigram_id;
string word;

void read_word_id()
{
	vector<pair<string, int>> words;
	ifstream in(root_path + "/cnt_na");
	int cnt;
	while(in >> word >> cnt)
		words.push_back(make_pair(word, cnt));
	cnt = 0;
	sort(words.begin(), words.end(), [](const pair<string, int> &l, const pair<string, int> &r){return l.second > r.second;});
	for (int i = 0; i < 5000000; i++)
	{
		string w = words[i].first;
		word_id[w] = cnt;
		cnt++;
	}
	words.clear();
	cout << "unique features: " << word_id.size() << endl;
}
void read_bigram_id()
{
	vector<pair<long long, int>> bigrams;
	ifstream in(root_path + "/bigram_id");
	int cnt;
	long long bi_id;
	while(in >> bi_id >> cnt)
		bigrams.push_back(make_pair(bi_id, cnt));
	cnt = 0;
	sort(bigrams.begin(), bigrams.end(), [](const pair<long long, int> &l, const pair<long long, int> &r){return l.second > r.second;});
	for (int i = 0; i < 1000000; i++)
		bigram_id[bigrams[i].first] = i;
	bigrams.clear();
	cout << "unique bigrams: " << bigram_id.size() << endl;
}

int main()
{
	read_word_id();
	read_bigram_id();

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
		int cnt = 0;
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

		ofstream out_indices(root_path + "/sep_bigram_" + mode + ".indices");
		ofstream out_indptr(root_path + "/sep_bigram_" + mode + ".indptr");
		ofstream out_data(root_path + "/sep_bigram_" + mode + ".data");
		int size = 0;
		out_indptr.write(reinterpret_cast<char*>(&size), sizeof(size));
		for (auto file : files)
		{
			string file_name = file.first;
			ifstream in(root_path + "/ref_na/" + to_string(path_to_folder[file_name]) + "/" + file_name);

			vector<int> words;
			while(in >> word)
			{
				auto f = word_id.find(word);
				int id = -1;
				if (f != word_id.end())
					id = word_id[word];
				words.push_back(id);
			}
			map<int, int> tf;
			for (int i = 1; i < words.size(); i++)
			{
				if (words[i - 1] == -1 || words[i] == -1)
					continue;
				long long bi = ((long long)(words[i - 1]) << 32) + words[i];
				auto f = bigram_id.find(bi);
				if (f == bigram_id.end())
					continue;

				tf[bigram_id[bi]]++;
			}
			for (auto &it : tf)
			{
				int feature_id = it.first;
				float feature_val = it.second;
				out_indices.write(reinterpret_cast<char*>(&feature_id), sizeof(feature_id));
				out_data.write(reinterpret_cast<char*>(&feature_val), sizeof(feature_val));
			}
			size += (tf.size());
			out_indptr.write(reinterpret_cast<char*>(&size), sizeof(size));

			cnt++;
			if (cnt % 1000 == 0)
			{
				cout << cnt << " " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start  << " " << bigram_id.size() << endl;
				start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
		}
	}
	return 0;
}
