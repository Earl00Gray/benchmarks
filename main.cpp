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

//    benchmark::sum_console(101, "makeHtmlAllOneThread", makeHtmlOneThread, students);
//    benchmark::sum_console(10, fns);

//    benchmark::sum_file(10, "sum_one.txt", "makeHtmlAllOneThread", makeHtmlOneThread, students);
//    benchmark::sum_file(10, "sum_all.txt", fns);

//    benchmark::gnuplot_file(10, "datas.txt", "makeHtmlAllOneThread", makeHtmlOneThread, students);
//    benchmark::gnuplot_file(10, "datas_all.txt", fns);

//    benchmark::gnuplot_png(10, "graph.png", "makeHtmlAllOneThread", makeHtmlOneThread, students);
    benchmark::gnuplot_png(10, "graph_all.png", fns);

//    benchmark::gnuplot_run(10, "makeHtmlAllOneThread", makeHtmlOneThread, students);
//    benchmark::gnuplot_run(10, fns);

    
    std::pair<std::pair<std::string, std::string(*)(const std::vector<Student> &)>, std::vector<Student>> fn 
        = { { "makeHtmlAllOneThread", makeHtmlOneThread }, students };

    return EXIT_SUCCESS;
} 