#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>

using namespace std;

struct Work {
  string name;
  vector<time_t> start_times;
  vector<time_t> end_times;
  double total_hours;
};

vector<Work> works;

string get_file_path() {
  string file_path;
  char *home_dir = getenv("HOME");
  file_path = string(home_dir) + "/.local/share/workman/works.txt";
  struct stat info;
  if (stat(file_path.c_str(), &info) != 0) {
    // Directory does not exist, create it
    mkdir(file_path.substr(0, file_path.find_last_of('/')).c_str(),
          S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  return file_path;
}

void save_works() {
  ofstream file(get_file_path());
  if (file.is_open()) {
    file << works.size() << endl;
    for (Work work : works) {
      file << work.name << endl;
      file << work.total_hours << endl;
      file << work.start_times.size() << endl;
      for (time_t start_time : work.start_times) {
        file << start_time << endl;
      }
      file << work.end_times.size() << endl;
      for (time_t end_time : work.end_times) {
        file << end_time << endl;
      }
    }
    file.close();
  }
}

void load_works() {
  ifstream file(get_file_path());
  if (file.is_open()) {
    works.clear();
    int num_works;
    file >> num_works;
    for (int i = 0; i < num_works; i++) {
      Work work;
      file >> work.name;
      file >> work.total_hours;
      int num_times;
      file >> num_times;
      for (int j = 0; j < num_times; j++) {
        time_t start_time;
        file >> start_time;
        work.start_times.push_back(start_time);
      }
      file >> num_times;
      for (int j = 0; j < num_times; j++) {
        time_t end_time;
        file >> end_time;
        work.end_times.push_back(end_time);
      }
      works.push_back(work);
    }
    file.close();
  }
}

void create_work(string name) {
  for (Work work : works) {
    if (work.name == name) {
      cout << "Work already exists" << endl;
      return;
    }
  }
  Work work;
  work.name = name;
  work.total_hours = 0;
  works.push_back(work);
  save_works();
  cout << "Work created" << endl;
}

time_t read_time(string time_str) {
  time_t result_time = time(nullptr);
  if (!time_str.empty()) {
    tm input_tm;
    strptime(time_str.c_str(), "%H:%M", &input_tm);
    tm *start_time_tm = localtime(&result_time);
    start_time_tm->tm_hour = input_tm.tm_hour;
    start_time_tm->tm_min = input_tm.tm_min;
    result_time = mktime(start_time_tm);
  }
  return result_time;
}

void begin_work(string name, string start_time_str) {
  time_t start_time = read_time(start_time_str);
  for (Work &work : works) {
    if (work.name == name) {
      work.start_times.push_back(start_time);
      cout << "Work begun" << endl;
      save_works();
      return;
    }
  }
  cout << "Work not found" << endl;
}

void end_work(string name, string end_time_str) {
  time_t end_time = read_time(end_time_str);
  for (Work &work : works) {
    if (work.name == name) {
      if (work.start_times.size() == work.end_times.size()) {
        cout << "Work not started" << endl;
        return;
      }
      time_t start_time = work.start_times.back();
      work.end_times.push_back(end_time);
      work.total_hours += difftime(end_time, start_time) / 3600.0;
      cout << "Work ended" << endl;
      save_works();
      return;
    }
  }
  cout << "Work not found" << endl;
}

void delete_work(string name) {
  for (auto it = works.begin(); it != works.end(); ++it) {
    if (it->name == name) {
      works.erase(it);
      save_works();
      cout << "Work \"" << name << "\" deleted." << endl;
      return;
    }
  }
  cerr << "Work \"" << name << "\" not found." << endl;
}

void print_stats() {
  for (Work work : works) {
    cout << work.name << endl;
    cout << "Total hours: " << fixed << setprecision(2) << work.total_hours
         << endl;
    if (work.start_times.empty()) {
      cout << "No working hours" << endl;
    } else {
      time_t first_day = work.start_times.front();
      tm *first_day_tm = localtime(&first_day);
      first_day_tm->tm_hour = 0;
      first_day_tm->tm_min = 0;
      first_day_tm->tm_sec = 0;
      first_day = mktime(first_day_tm);

      time_t last_day = work.end_times.back();
      tm *last_day_tm = localtime(&last_day);
      last_day_tm->tm_hour = 23;
      last_day_tm->tm_min = 59;
      last_day_tm->tm_sec = 59;
      last_day = mktime(last_day_tm);

      double total_hours = 0;
      for (int i = 0; i < (int)work.start_times.size(); i++) {
        time_t start_time = work.start_times[i];
        time_t end_time = work.end_times[i];
        total_hours += difftime(end_time, start_time) / 3600.0;
      }
      cout << "Average hours per day: " << fixed << setprecision(2)
           << total_hours / difftime(last_day, first_day) * 86400 << endl;
      double past_hours = 0;
      for (int i = work.start_times.size() - 1; i >= 0; i--) {
        time_t start_time = work.start_times[i];
        time_t end_time = work.end_times[i];
        if (difftime(time(nullptr), start_time) < 604800) {
          past_hours += difftime(end_time, start_time) / 3600.0;
        } else {
          break;
        }
      }
      cout << "Past 7 days: " << fixed << setprecision(2) << past_hours << endl;
    }
  }
}

int main(int argc, char *argv[]) {
  load_works();
  if (argc < 2) {
    cout << "Usage: workman <command> [arguments]" << endl;
    cout << "Commands:" << endl;
    cout << "  c[reate] <work_name>: Create a type of work" << endl;
    cout << "  s[tats]: Print stats" << endl;
    cout << "  b[egin] <work_name> [start_time in hh:mm]: Begin a work" << endl;
    cout << "  e[nd] <work_name> [end_time in hh:mm]: End a work" << endl;
    cout << "  d[elete] <work_name>: Delete a work" << endl;
    return 0;
  }
  string command = argv[1];
  if (command == "create" || command == "c") {
    if (argc < 3) {
      cout << "Usage: workman c[reate] <work_name>" << endl;
      return 0;
    }
    string name = argv[2];
    create_work(name);
  } else if (command == "begin" || command == "b") {
    if (argc < 3) {
      cout << "Usage: workman b[egin] <work_name> [start_time in hh:mm]"
           << endl;
      return 0;
    }
    string name = argv[2];
    string start_time_str;
    if (argc >= 4) {
      start_time_str = argv[3];
    }
    begin_work(name, start_time_str);
  } else if (command == "end" || command == "e") {
    if (argc < 3) {
      cout << "Usage: workman e[nd] <work_name> [end_time in hh:mm]" << endl;
      return 0;
    }
    string name = argv[2];
    string end_time_str;
    if (argc >= 4) {
      end_time_str = argv[3];
    }
    end_work(name, end_time_str);
  } else if (command == "delete" || command == "d") {
    if (argc < 3) {
      cerr << "Usage: workman d[elete] <work_name>" << endl;
      return 1;
    }
    string name = argv[2];
    delete_work(name);
  } else if (command == "stats" || command == "s") {
    print_stats();
  } else {
    cout << "Unknown command" << endl;
  }
  return 0;
}
