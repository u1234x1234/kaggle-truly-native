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

	unordered_map<string, int> mp[2];
	vector<string> modes = {"train"};
	int cnt = 0;
	for (auto mode : modes)
	{
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

		for (auto file : files)
		{
			string file_name = file.first;
			ifstream in(root_path + "/ref_na/" + to_string(path_to_folder[file_name]) + "/" + file_name);

			string word;
			while(in >> word)
				mp[file.second][word]++;

			cnt++;
			if (cnt % 1000 == 0)
			{
				cout << cnt << " " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start << endl;
				start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
		}
	}
	for (int i = 0; i < 2; i++)
	{
		ofstream out(root_path + "/stat" + to_string(i));
		for (auto &it : mp[i])
			out << it.first << " " << it.second << '\n';
	}

	return 0;
}
