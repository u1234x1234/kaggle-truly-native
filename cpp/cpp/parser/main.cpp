#include <iostream>
#include <fstream>
#include <string>
#include <future>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <cctype>
#include <a.out.h>
#include <functional>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std;

int main()
{
	string root_path = "/root/native/";

	vector<string> files;
	boost::filesystem::directory_iterator end_itr;
	for(int folder = 0; folder < 6; folder++)
	{
		boost::filesystem::path pth = root_path + "/data/" + to_string(folder);
		if (boost::filesystem::exists(pth))
			for (boost::filesystem::directory_iterator itr(pth); itr != end_itr; ++itr)
				files.push_back(itr->path().string());
	}
	cout << files.size() << endl;

	auto proc = [](const vector<string> &files) -> unordered_map<string, int>
	{
		auto token_process = [](string &a) -> void
		{
			for (int i = 0; i < a.size(); i++)
				a[i] = tolower(a[i]);
		};
		auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		unordered_map<string, int> word_count;
		int cnt = 0;
		for (auto file : files)
		{
			string new_file = file;
			boost::replace_all(new_file, "/data/", "/ref_na/");
			ifstream in(file);
			ofstream out(new_file);

			vector<string> tokens;
			stringstream ss;
			ss << in.rdbuf();
			string line = ss.str();
			boost::split(tokens, line, !boost::is_alnum() && !boost::is_any_of("."));
//			boost::split(tokens, line, boost::is_space());
			unordered_set<string> un;
			for (size_t i = 0; i < tokens.size(); i++)
			{
				if (tokens[i].size() < 2)
					continue;
				token_process(tokens[i]);
//				cout << tokens[i] << "\n";
				out << tokens[i] << " ";
				if (un.find(tokens[i]) == un.end())
				{
					word_count[tokens[i]]++;
					un.insert(tokens[i]);
				}
			}
//			cout << endl << endl;
//			cin.get();

			out.close();
			in.close();
			cnt++;
			if (cnt % 1000 == 0)
			{
				cout << cnt << " " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start << endl;
				start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
		}
		return word_count;
	};
	int thread_cnt = 4;
	vector<vector<string>> files_per_thread(thread_cnt);
	for (int i = 0; i < files.size(); i++)
		files_per_thread[i % thread_cnt].push_back(files[i]);
	vector<future<unordered_map<string, int>>> futures;
	for (int i = 0; i < thread_cnt; i++)
		futures.push_back(async(std::launch::async, proc, files_per_thread[i]));
	unordered_map<string, int> word_count;
	for (int i = 0; i < thread_cnt; i++)
	{
		auto tt = futures[i].get();
		for (auto it : tt)
			word_count[it.first] += it.second;
	}
	ofstream out(root_path + "/cnt_na");
	for (auto it : word_count)
		out << it.first << " " << it.second << "\n";


	return 0;
}
