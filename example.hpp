#pragma once
#include <string>
#include <vector>
#include <thread>
#include <future>

struct Student {
    std::string fname, sname;
    std::string facilite;
    int age;
    int cource;
};

constexpr int GCOLUMN_COUNT = 5;
    
std::string mokky(const std::vector<Student> &datas) { return std::string{}; }

std::string makeHtmlOneThread(const std::vector<Student> &datas) {
    const int ROW_COUNT = datas.size();
    std::string table_body, tempr;

    for (int i = 0; i < ROW_COUNT; ++i) {
        for (int j = 0; j < GCOLUMN_COUNT; ++j) {
            tempr += "<td>" + datas[i].sname + "</td>";
        }
        table_body += "<tr>" + tempr + "</tr>";
        int size = tempr.size();
        tempr.clear();
        tempr.reserve(size);
    }
    return "<table>" + table_body + "</table>";
}

std::string makeHtmlTask(int row_start, int row_end, const std::vector<Student> &datas) {
    std::string result, tempr;
    for (int i = row_start; i < row_end; ++i) {
        for (int j = 0; j < GCOLUMN_COUNT; ++j) {
            tempr += "<td>" + datas[i].sname + "</td>";
        }
        result += "<tr>" + tempr + "</tr>";
        int size = tempr.size();
        tempr.clear();
        tempr.reserve(size);
    }
    return result;
}

std::string makeHtmlMultyThread(const std::vector<Student> &datas) {
    const int ROW_COUNT = datas.size();
    const int THREAD_COUNT = std::thread::hardware_concurrency() - 1;
    const int ROW_PER_THREAD = ROW_COUNT / THREAD_COUNT;

    std::thread threads[THREAD_COUNT];
    std::string results[THREAD_COUNT];
    for (int i = 0, row = 0; i < THREAD_COUNT; row += ROW_PER_THREAD, ++i) {
        (i == THREAD_COUNT - 1)
            ? threads[i] = std::thread([&datas, row, ROW_COUNT, &results, i]() { 
                results[i] = makeHtmlTask(row, ROW_COUNT, datas); })
            : threads[i] = std::thread([&datas, row, ROW_PER_THREAD, &results, i]() { 
                results[i] = makeHtmlTask(row, row + ROW_PER_THREAD, datas); });
    }
    for (int i = 0; i < THREAD_COUNT; ++i) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    std::string table_body;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        table_body += results[i];
    }
    return "<table>" + table_body + "</table>";
}

std::string makeHtmlMultyThreadFuture(const std::vector<Student> &datas) {
    const int ROW_COUNT = datas.size();
    const int THREAD_COUNT = std::thread::hardware_concurrency() - 1;
    const int ROW_PER_THREAD = ROW_COUNT / THREAD_COUNT;

    std::future<std::string> futures[THREAD_COUNT];
    for (int i = 0, row = 0; i < THREAD_COUNT; row += ROW_PER_THREAD, ++i) {
        (i == THREAD_COUNT - 1)
            ? futures[i] = std::async(std::launch::async, [&datas, row, ROW_COUNT]() 
                            { return makeHtmlTask(row, ROW_COUNT, datas); })
            : futures[i] = std::async(std::launch::async, [&datas, row, ROW_PER_THREAD]() 
                            { return makeHtmlTask(row, row + ROW_PER_THREAD, datas); });
    }

    std::string table_body;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        try {
            table_body += futures[i].get();
        }
        catch (const std::exception &except) {
            fprintf(stderr, "%s \n", except.what());
            return std::string{};
        }
    }

    return "<table>" + table_body + "</table>";
} 