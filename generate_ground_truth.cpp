#include <fstream>
#include <string>
#include <iostream>

#define PROJECT_NUMBER 5

using namespace std;

struct Project {
	string name;
	int bug_num;
}projects[PROJECT_NUMBER];

const string project_path = "statistic\\statistic\\method\\";
const string default_sdcsv = "Ochiai_coverage_sd.csv";

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

int main(int argc, char const *argv[])
{
	InitProjects();
	ofstream out("gt.csv");
	out << "project\tbug_id\tmethod" << endl;
	for(int i = 0; i < PROJECT_NUMBER; i++) {
		for(int j = 1; j <= projects[i].bug_num; j++) {
			string filename = project_path + projects[i].name + "\\" + projects[i].name + "_" + to_string(j) + "\\" + default_sdcsv;
			ifstream in(filename.c_str());
			if (in.fail()) {
				cout << "Fail to open file " << filename << endl;
				continue;
			}
			string line;
			getline(in, line); // skip header
			while(!getline(in, line).eof()) {
				if (line.empty()) continue;
				int last_ind = line.length();
				if (line[last_ind - 1] == 'T' && line[last_ind - 2] == 'G') {
					int first_sep = line.find_first_of('\t');
					string method = line.substr(0, first_sep);
					int last_sep_in_method = method.find_last_of('#');
					out << projects[i].name << "\t" << j << "\t" << method.substr(0, last_sep_in_method) << endl;
				}
			}
			in.close();
		}
	}
	out.close();
	return 0;
}
