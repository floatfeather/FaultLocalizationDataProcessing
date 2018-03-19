#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <map>
#include <iostream>
#include <cmath>

using namespace std;

#define ALGORITHM_NUMBER 5
#define PROJECT_NUMBER 5

struct Project {
	string name;
	int bug_num;
}projects[PROJECT_NUMBER];

const string project_path = "sd_data\\";
const string algorithms[ALGORITHM_NUMBER] = {"Barinel", "DStar", "Ochiai", "Op2", "Tarantula"};
const string sdcsv = "_coverage_sd.csv";
const string result_path = "result\\";
const string result_file_suffix = "_predicate_effect.csv";
map<string, map<int, vector<string>>> bug_map;

void InitProjects() {
	projects[0].name = "math";
	projects[0].bug_num = 106;
	projects[1].name = "chart";
	projects[1].bug_num = 26;
	projects[2].name = "lang";
	projects[2].bug_num = 65;
	projects[3].name = "time";
	projects[3].bug_num = 27;
	projects[4].name = "closure";
	projects[4].bug_num = 0;
}

void SplitCSVLine(const string& line, vector<string>* result) {
	int pos = 0;
	while(true) {
		int next = line.find_first_of('\t', pos);
		if (next == string::npos) {
			result->push_back(line.substr(pos));
			break;
		} else {
			result->push_back(line.substr(pos, next - pos));
			pos = next + 1;
		}
	}
}

void InitBugMap() {
	ifstream in("gt.csv");
	string line;
	getline(in, line); // skip header
	while(!getline(in, line).eof()) {
		if (line.empty()) continue;
		vector<string> results;
		SplitCSVLine(line, &results);
		bug_map[results[0]][stoi(results[1])].push_back(results[2]);
	}
	in.close();
}

int main(int argc, char const *argv[]) {
	InitProjects();
	InitBugMap();
	for(int i = 0; i < PROJECT_NUMBER; i++) {
		string temp1 = result_path + projects[i].name + "\\";
		CreateDirectory(temp1.c_str(), NULL);
		for(int j = 1; j <= projects[i].bug_num; j++) {
			string temp2 = temp1 + projects[i].name + "_" + to_string(j) + "\\";
			CreateDirectory(temp2.c_str(), NULL);
			vector<string>& bugs = bug_map[projects[i].name][j];
			for(int k = 0; k < ALGORITHM_NUMBER; k++) {
				string result_file_path = temp2 + algorithms[k] + result_file_suffix;
				string filename = project_path + projects[i].name + "\\" + projects[i].name + "_" + to_string(j) + "\\" + algorithms[k] + sdcsv;
				ofstream out(result_file_path.c_str());
				ifstream in(filename.c_str());
				if (in.fail()) {
					cout << "Fail to open " << filename << endl;
					continue;
				}
				string line;
				getline(in, line); // skipheader
				out << "is_bug\tis_useful\tis_equal\tline\toriginal_score\tsd_score\tpredicate" << endl;
				while(!getline(in, line).eof()) {
					vector<string> results;
					SplitCSVLine(line, &results);
					string method = results[0];
					int last_sep_in_method = method.find_last_of('#');
					method = method.substr(0, last_sep_in_method);
					double spectrum_score = stod(results[1]);
					double sd_score = stod(results[2]);
					string predicate = results[4];
					if (predicate.empty()) {
						continue;
					}
					bool is_bug = false;
					for(string bug : bugs) {
						if (method == bug) {
							is_bug = true;
							break;
						}
					}
					bool is_useful = (is_bug && sd_score > spectrum_score) || (!is_bug && sd_score < spectrum_score);
					bool is_equal = fabs(sd_score - spectrum_score) < 1e-6;
					out << is_bug << "\t" << is_useful << "\t" << is_equal << "\t" << results[0] << "\t" << spectrum_score << "\t" << sd_score << "\t" << predicate << endl; 
				}
				out.close();
				in.close();
			}
		} 
	}
}
