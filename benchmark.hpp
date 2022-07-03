#pragma once
#include <stdio.h>
#include <string>
#include <string.h>
#include <cassert>
#include <chrono>
#include <vector>
#include <string_view>
namespace benchmark {

    namespace inner {

        #define FOR_ONE_FN_WITH_FILE(FN_NAME) \
            template <typename Fn, typename... Args> \
            bool FN_NAME( \
            int iter_count, const std::string &path_file, const std::string &fn_name, Fn fn, Args ...args \
            ) { \
            std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns; \
            fns.push_back(std::make_pair(std::make_pair(fn_name, fn), args... )); \
            return FN_NAME(iter_count, path_file, fns); \
        }
        #define FOR_ONE_FN(FN_NAME) \
            template <typename Fn, typename... Args> \
            bool FN_NAME(int iter_count, const std::string &fn_name, Fn fn, Args ...args) { \
            std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns; \
            fns.push_back(std::make_pair(std::make_pair(fn_name, fn), args... )); \
            return FN_NAME(iter_count, fns); \
        }
        
        
        class File {
        public:
            File(const std::string_view &file_name) : mpath_file{ file_name } {}
//            ~File() { close(); };

            bool open(const std::string &right) {
                mfile = fopen(mpath_file.c_str(), right.c_str());
                if (!mfile) {
                    log_error();
                    return false;
                }
                return true;
            }

            void close() {
                if (!mfile) {
                    fprintf(stderr, "%s : empty pointer\n", mpath_file.c_str());
                    return;
                }
                else {
                    if (fclose(mfile) != 0) { log_error(); }
                    mfile = nullptr;
                }
            }

            FILE *handle() const { return mfile; }

        private:
            std::string mpath_file;
            FILE *mfile;

            void log_error() {
                fprintf(stderr, "%s : %s\n", mpath_file.c_str(), strerror(errno));
            }
        };

        class Pipe {
        public:
            Pipe(const std::string_view &file_name) : mpath_file{ file_name } {}
//            ~Pipe() { close(); }

            bool open(const std::string &right) {
                mfile = popen(mpath_file.c_str(), right.c_str());
                if (!mfile) {
                    log_error();
                    return false;
                }
                return true;
            }
            
            void close() {
                if (!mfile) {
                    fprintf(stderr, "%s : empty pointer\n", mpath_file.c_str());
                    return;
                }
                else {
                    if (fclose(mfile) != 0) { log_error(); }
                    mfile = nullptr;
                }
            }

            FILE *handle() const { return mfile; }
            
        private:
            std::string mpath_file;
            FILE *mfile;

            void log_error() {
                fprintf(stderr, "%s : %s\n", mpath_file.c_str(), strerror(errno));
            }
        };
        
        struct FnDescription {
            float max = 0.0f, min = 0.0f, sum = 0.0f;
            std::string name;
        };
        template <typename Fn, typename... Args>
        std::vector<FnDescription> benchmark(
            int iter_count, FILE *file, const std::vector<std::pair<std::pair<std::string, Fn>, Args...>> &fns
            ) {
            if (iter_count <= 0) {
                return std::vector<FnDescription>{};
            }

            decltype(std::chrono::steady_clock::now()) startTime, endTime;
            const int FN_COUNT = fns.size();
            std::vector<FnDescription> fns_descr;
            fns_descr.resize(FN_COUNT);

            for (int i = 0; i < iter_count; ++i) {
                for (int fn_count = 0; fn_count < FN_COUNT; ++fn_count) {
                    startTime = std::chrono::steady_clock::now();
                    fns[fn_count].first.second(fns[fn_count].second);
                    endTime = std::chrono::steady_clock::now();

                    auto value = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
                
                    [[unlikely]] if (i == 0) { // start value
                        fns_descr[fn_count].name = fns[fn_count].first.first;
                        fns_descr[fn_count].min = value;
                        fns_descr[fn_count].max = value;
                        fns_descr[fn_count].sum = 0;
                    }

                    if (value > fns_descr[fn_count].max) { fns_descr[fn_count].max = value; }
                    if (value < fns_descr[fn_count].min) { fns_descr[fn_count].min = value; }
                    fns_descr[fn_count].sum += value;

                    if (file) {
                        if (fn_count == 0) {
                            fprintf(file, "%d\t", i);
                        }
                        fprintf(file, "%f\t", value / 1000.0f);
                    }
                }
                if (file) {
                    fprintf(file, "\n");
                }
            }
            return fns_descr;
        }
        bool print_result(int iter_count, const std::vector<FnDescription> &fn_descr, FILE *file = stdout) {
            const int FN_COUNT = fn_descr.size();
            for (int i = 0; i < FN_COUNT; ++i) {
                try {
                    auto fn = fn_descr.at(i);
                    fprintf(
                        file, 
                        "\n*******************\n"
                        "FN: %s\n"
                        "ITERATIONS: %d\n"
                        "MAX: %f ms\n"
                        "MIN: %f ms\n"
                        "AVG: %f ms\n"
                        "*******************\n",
                        fn.name.c_str(), iter_count, fn.max / 1000.0, 
                        fn.min / 1000.0, fn.sum / (iter_count * 1000.0)
                    );
                }
                catch (const std::exception &except) {
                    fprintf(stderr, "error: %s\n", except.what());
                    return false;
                }
            }
            return true;
        }
        
    }  

    using namespace inner;

    template <typename Fn, typename... Args>
    bool sum_console(int iter_count, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns) {
        std::vector<FnDescription> fn_descr = benchmark(iter_count, nullptr, fns);
        return print_result(iter_count, fn_descr);
    }
    FOR_ONE_FN(sum_console)

    template <typename Fn, typename... Args>
    bool sum_file(
        int iter_count, const std::string &path_file, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns
        ) {
        File file(path_file);
        if (!file.open("w")) {
            return false;
        }       
        std::vector<FnDescription> fn_descr = benchmark(iter_count, nullptr, fns);
        bool isOk = print_result(iter_count, fn_descr, file.handle());
        file.close();
        return isOk;
    }
    FOR_ONE_FN_WITH_FILE(sum_file)
    
    template <typename Fn, typename... Args>
    bool gnuplot_file(
        int iter_count, const std::string &path_file, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns
        ) {
        if (iter_count <= 0) {
            return false;
        }
        
        File file(path_file);
        if (!file.open("w")) {
            return false;
        }
        auto fn_descr = benchmark(iter_count, file.handle(), fns);
        file.close();
        return print_result(iter_count, fn_descr);
    }
    FOR_ONE_FN_WITH_FILE(gnuplot_file)

    namespace inner {

        std::string_view gnuplot_name{ "gnuplot -persist" };
        const char *TEMP_GNUPLOT_DATA_PATH_FILE = "gnuplot_temp.dat";

        const char *graph_settings =
            "set grid\n"
            "set xlabel \"iteration\"\n"
            "set ylabel \"time, ms \"\n"
            "set datafile separator tab\n"
            "set key spacing 1.3\n";
        
        bool create_graph(FILE *pipe, const std::vector<FnDescription> &fn_descr) {
            if (fn_descr.empty()) {
                return false;
            }
            const int FN_COUNT = fn_descr.size();
            // settings
            fprintf(pipe, "%s", graph_settings);
            // plot
            fprintf(
                pipe, "plot \"%s\" using 1:2 with lines linecolor -1 ti \"%s\"", 
                TEMP_GNUPLOT_DATA_PATH_FILE, fn_descr[0].name.c_str()
            );
            for (int i = 1; i < FN_COUNT; ++i) {
                fprintf(
                    pipe, ",\"%s\" using 1:%d with lines linecolor %d ti \"%s\"", 
                    TEMP_GNUPLOT_DATA_PATH_FILE, i + 2, (i - 1) % 10, fn_descr[i].name.c_str()
                    );
            }
            fprintf(pipe, "\n");

            fflush(pipe);
            return true;
        }

        bool run(const std::vector<FnDescription> &fn_descr) {
            Pipe pipe(gnuplot_name);
            if (!pipe.open("w") ) {
                return false;
            }
            return create_graph(pipe.handle(), fn_descr);
        }

        bool create_png(const std::vector<FnDescription> &fn_descr, const std::string &path_file) {
            Pipe pipe(gnuplot_name);
            if (!pipe.open("w") ) {
                return false;
            }
            // png
            fprintf(
                pipe.handle(), "set terminal png"
                " enhanced background rgb 'gray'"
                " size 900, 900\n"
            );
            fprintf(pipe.handle(), "set output '%s'\n", path_file.c_str());
            fflush(pipe.handle());
            return create_graph(pipe.handle(), fn_descr);
        }

    }

    template <typename Fn, typename... Args>
    bool gnuplot_run(int iter_count, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns) {
        if (iter_count <= 0) {
            return false;
        }

        File file(TEMP_GNUPLOT_DATA_PATH_FILE);
        if (!file.open("w")) {
            return false;
        }
        auto fn_descr = benchmark(iter_count, file.handle(), fns);
        file.close();
        if (!print_result(iter_count, fn_descr)) {
            return false;
        }
        return run(fn_descr);
    }
    FOR_ONE_FN(gnuplot_run)
    
    template <typename Fn, typename... Args>
    bool gnuplot_png(
        int iter_count, const std::string &path_file, std::vector<std::pair<std::pair<std::string, Fn>, Args...>> fns
        ) {
        if (iter_count <= 0) {
            return false;
        }

        File file(TEMP_GNUPLOT_DATA_PATH_FILE);
        if (!file.open("w")) {
            return false;
        }
        auto fn_descr = benchmark(iter_count, file.handle(), fns);

        if (!print_result(iter_count, fn_descr)) {
            return false;
        }
        file.close();
        return create_png(fn_descr, path_file);
    }
    FOR_ONE_FN_WITH_FILE(gnuplot_png)

} 