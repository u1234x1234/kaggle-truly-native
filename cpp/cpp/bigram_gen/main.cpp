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
		words.push_back(make_pair(word, cnt));
	cnt = 0;
	sort(words.begin(), words.end(), [](const pair<string, int> &l, const pair<string, int> &r){return l.second > r.second;});
	unordered_map<string, int> word_id;
	for (int i = 0; i < 5000000; i++)
	{
		string w = words[i].first;
		word_id[w] = cnt;
		cnt++;
	}
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
	vector<string> modes = {"test"};
	unordered_map<long long, int> bigram_id;
	unordered_map<long long, int> bigram_cnt;
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
			map<long long, int> tf;
			for (int i = 1; i < words.size(); i++)
			{
				if (words[i - 1] == -1 || words[i] == -1)
					continue;
				long long bi = ((long long)(words[i - 1]) << 32) + words[i];
				tf[bi]++;
			}
			for (auto &it : tf)
				bigram_cnt[it.first]++;

			cnt++;
			if (cnt % 1000 == 0)
			{
				cout << cnt << " " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start  << " " << bigram_cnt.size() << endl;
				start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
		}
	}
	ofstream out(root_path + "/bigram_id");
	for (auto it : bigram_cnt)
		out << it.first << " " << it.second << "\n";

	return 0;
}
