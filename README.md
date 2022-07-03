# benchmark
Systems:
    linux

Dependencies:
    gnuplot
instaling Debian and Ubuntu:
    $ sudo apt-get update
    $ sudo apt-get install gnuplot

functions:
    // print summary to console
    bool sum_console(int iter_count, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns);
    bool sum_console(int iter_count, Fn fn, Args ...args);

    // print summary to file 'path_file'
    bool sum_file(
        int iter_count, const std::string &path_file, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns
    );
    bool sum_file(int iter_count, const std::string &fn_name, Fn fn, Args ...args);

    // print points for gnuplot to file 'path_file'
    bool gnuplot_file(
        int iter_count, const std::string &path_file, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns
    );
    bool gnuplot_file(int iter_count, const std::string &fn_name, Fn fn, Args ...args);
    
    // run gnuplot
    bool gnuplot_run(int iter_count, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns);
    bool gnuplot_run(int iter_count, Fn fn, Args ...args);
    
    // save gnuplot results in png image 'path_file'
    bool gnuplot_png(
        int iter_count, const std::string &path_file, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns
    );
    bool gnuplot_png(int iter_count, const std::string &fn_name, Fn fn, Args ...args);
    

example usage:

#include "benchmark.hpp"
#include "example.hpp"
#include <vector>

int main(int argc, char **argv) {
    Student student = { "First", "Second", "FF", 19, 2 };
    std::vector<Student> students(101, student);

    std::vector<std::pair<std::pair<std::string, std::string(*)(const std::vector<Student> &)>, std::vector<Student>>> fns;
    fns.push_back(std::make_pair(std::make_pair("makeHtmlAllOneThread", makeHtmlOneThread), students));
    fns.push_back(std::make_pair(std::make_pair("mokky", mokky), students));
    fns.push_back(std::make_pair(std::make_pair("makeHtmlMultyThread", &makeHtmlMultyThread), students));
    fns.push_back(std::make_pair(std::make_pair("makeHtmlMultyThreadFuture", &makeHtmlMultyThreadFuture), students));

    benchmark::sum_console(101, "makeHtmlAllOneThread", makeHtmlOneThread, students);
    benchmark::sum_console(10, fns);

    benchmark::sum_file(10, "sum_one.txt", "makeHtmlAllOneThread", makeHtmlOneThread, students);
    benchmark::sum_file(10, "sum_all.txt", fns);

    benchmark::gnuplot_file(10, "datas.txt", "makeHtmlAllOneThread", makeHtmlOneThread, students);
    benchmark::gnuplot_file(10, "datas_all.txt", fns);

    benchmark::gnuplot_png(10, "graph.png", "makeHtmlAllOneThread", makeHtmlOneThread, students);
    benchmark::gnuplot_png(10, "graph_all.png", fns);

    benchmark::gnuplot_run(10, "makeHtmlAllOneThread", makeHtmlOneThread, students);
    benchmark::gnuplot_run(10, fns);

    return EXIT_SUCCESS;
} 