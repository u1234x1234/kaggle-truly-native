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
unordered_map<string, int> word_id_site, word_id_na, word_id_space;
void read_site()
{
	vector<pair<string, int>> words;
	ifstream in(root_path + "/cnt_na");
	string word;
	int cnt;
	while(in >> word >> cnt)
		words.push_back(make_pair(word, cnt));
	cnt = 0;
	sort(words.begin(), words.end(), [](const pair<string, int> &l, const pair<string, int> &r){return l.second > r.second;});

	ofstream word_id_out(root_path + "/word_id_sep_site");
	unordered_set<string> site_ext = {"com", "net", "ru", "org", "de", "jp", "uk", "br", "pl", "in", "it", "fr", "info", "au", "nl", "cn", "ir", "es", "biz", "kz", "kr", "ka", "eu", "ua", "za", "co", "gr", "ro", "se"};
	int n = 0;
	for (int i = 0; i < words.size(); i++)
	{
		string w = words[i].first;
		int p = w.find_last_of(".");
		if (p != string::npos)
		{
			string ext = w.substr(p + 1, 5);
			if (site_ext.find(ext) != site_ext.end())
			{
				word_id_site[words[i].first] = n;
				word_id_out << n << " " << words[i].first << " " << words[i].second << "\n";
				n++;
			}
		}
	}
	cout << "site readed. unique features: " << word_id_site.size() << endl;
}
void read_na()
{
	vector<pair<string, int>> words;
	ifstream in(root_path + "/cnt_na");
	string word;
	int cnt;
	while(in >> word >> cnt)
		words.push_back(make_pair(word, cnt));
	cnt = 0;
	sort(words.begin(), words.end(), [](const pair<string, int> &l, const pair<string, int> &r){return l.second > r.second;});
	ofstream word_id_out(root_path + "/word_id_sep_na");
	int n = 0;
	for (int i = 0; i < 10000000; i++)
	{
		string w = words[i].first;
		if (word_id_site.find(w) != word_id_site.end())
			continue;
		word_id_na[w] = n;
		word_id_out	<< n << " " << words[i].first << " " << words[i].second << "\n";
		n++;
	}
	cout << "na readed. unique features: " << word_id_na.size() << endl;
}
void read_space()
{
	vector<pair<string, int>> words;
	ifstream in(root_path + "/cnt_space");
	string word;
	int cnt;
	while(in >> word >> cnt)
		words.push_back(make_pair(word, cnt));
	cnt = 0;
	sort(words.begin(), words.end(), [](const pair<string, int> &l, const pair<string, int> &r){return l.second > r.second;});
	ofstream word_id_out(root_path + "/word_id_sep_space");
	int n = 0;
	for (int i = 0; i < 5000000; i++)
	{
		string w = words[i].first;
		if (word_id_na.find(w) != word_id_na.end())
			continue;
		if (word_id_site.find(w) != word_id_site.end())
			continue;

		word_id_space[w] = n;
		word_id_out	<< n << " " << words[i].first << " " << words[i].second << "\n";
		n++;
	}
	cout << "space readed. unique features: " << word_id_space.size() << endl;
}


int main()
{
	read_site();
	read_na();
	read_space();

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
	int cnt;
	string word;
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

		ofstream out_indices_site(root_path + "/sep_site_" + mode + ".indices");
		ofstream out_indptr_site(root_path + "/sep_site_" + mode + ".indptr");
		ofstream out_data_site(root_path + "/sep_site_" + mode + ".data");

		ofstream out_indices_na(root_path + "/sep_na_" + mode + ".indices");
		ofstream out_indptr_na(root_path + "/sep_na_" + mode + ".indptr");
		ofstream out_data_na(root_path + "/sep_na_" + mode + ".data");

		ofstream out_indices_space(root_path + "/sep_space_" + mode + ".indices");
		ofstream out_indptr_space(root_path + "/sep_space_" + mode + ".indptr");
		ofstream out_data_space(root_path + "/sep_space_" + mode + ".data");

		ofstream out_indices_hc(root_path + "/sep_hc_" + mode + ".indices");
		ofstream out_indptr_hc(root_path + "/sep_hc_" + mode + ".indptr");
		ofstream out_data_hc(root_path + "/sep_hc_" + mode + ".data");

		int size_site = 0, size_na = 0, size_space = 0, size_hc = 0;
		out_indptr_site.write(reinterpret_cast<char*>(&size_site), sizeof(size_site));
		out_indptr_na.write(reinterpret_cast<char*>(&size_na), sizeof(size_na));
		out_indptr_space.write(reinterpret_cast<char*>(&size_space), sizeof(size_space));
		out_indptr_hc.write(reinterpret_cast<char*>(&size_hc), sizeof(size_hc));
		for (auto file : files)
		{
			string file_name = file.first;
			ifstream in_na(root_path + "/ref_na/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			// hand-crafted features
			int file_size_ref_na = boost::filesystem::file_size(root_path + "/ref_na/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			int file_size_ref_space = boost::filesystem::file_size(root_path + "/ref_space/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			int file_size_data = boost::filesystem::file_size(root_path + "/data/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			int token_number_na = 0, token_number_space = 0;

			map<int, int> tf_site, tf_na, tf_space;
			while(in_na >> word)
			{
				token_number_na++;
				auto f_site = word_id_site.find(word);
				if (f_site != word_id_site.end())
				{
					int id = word_id_site[word];
					tf_site[id]++;
					continue;
				}
				auto f_na = word_id_na.find(word);
				if (f_na != word_id_na.end())
				{
					int id = word_id_na[word];
					tf_na[id]++;
				}
			}
			ifstream in_space(root_path + "/ref_space/" + to_string(path_to_folder[file_name]) + "/" + file_name);
			while(in_space >> word)
			{
				token_number_space++;
				auto f_space = word_id_space.find(word);
				if (f_space != word_id_space.end())
				{
					int id = word_id_space[word];
					tf_space[id]++;
				}
			}

			auto write_feature = [&out_indices_hc, &out_data_hc](int feature_id, float feature_val)
			{
				out_indices_hc.write(reinterpret_cast<char*>(&feature_id), sizeof(feature_id));
				out_data_hc.write(reinterpret_cast<char*>(&feature_val), sizeof(feature_val));
			};
			write_feature(0, file_size_ref_na);
			write_feature(1, file_size_ref_space);
			write_feature(2, file_size_data);
			write_feature(3, token_number_na);
			write_feature(4, token_number_space);
			write_feature(5, tf_site.size());
			write_feature(6, tf_na.size());
			write_feature(7, tf_space.size());

			for (auto &it : tf_site)
			{
				int feature_id = it.first;
				float feature_val = it.second;
				out_indices_site.write(reinterpret_cast<char*>(&feature_id), sizeof(feature_id));
				out_data_site.write(reinterpret_cast<char*>(&feature_val), sizeof(feature_val));
			}
			for (auto &it : tf_na)
			{
				int feature_id = it.first;
				float feature_val = it.second;
				out_indices_na.write(reinterpret_cast<char*>(&feature_id), sizeof(feature_id));
				out_data_na.write(reinterpret_cast<char*>(&feature_val), sizeof(feature_val));
			}
			for (auto &it : tf_space)
			{
				int feature_id = it.first;
				float feature_val = it.second;
				out_indices_space.write(reinterpret_cast<char*>(&feature_id), sizeof(feature_id));
				out_data_space.write(reinterpret_cast<char*>(&feature_val), sizeof(feature_val));
			}

			size_site += (tf_site.size());
			size_na += (tf_na.size());
			size_space += (tf_space.size());
			size_hc += 8;

			out_indptr_site.write(reinterpret_cast<char*>(&size_site), sizeof(size_site));
			out_indptr_na.write(reinterpret_cast<char*>(&size_na), sizeof(size_na));
			out_indptr_space.write(reinterpret_cast<char*>(&size_space), sizeof(size_space));
			out_indptr_hc.write(reinterpret_cast<char*>(&size_hc), sizeof(size_hc));

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
