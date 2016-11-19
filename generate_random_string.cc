#include <iostream>
#include <string>

#include <common/string_util.h>

#include "sdk/bfs.h"

using namespace baidu::bfs;

class Random {
private:
    uint32_t seed_;
public:
    explicit Random(uint32_t s) : seed_(s & 0x7fffffffu) { }
    uint32_t Next() {
        static const uint32_t M = 2147483647L;
        static const uint64_t A = 16807;
        uint64_t product = seed_ * A;
        seed_ = static_cast<uint32_t>((product >> 31) + (product & M));
        if (seed_ > M) {
            seed_ -= M;
        }
        return seed_;
    }
    uint32_t Uniform(int n) { return Next() % n; }
};

Random** rand_;
FS* fs_;

void RandomString(std::string* out, int size) {
    out->resize(size);
    for (int i = 0; i < size; i++) {
        //(*out)[i] = static_cast<char>(' ' + rand_[random() % 10]->Next());
        (*out)[i] = static_cast<char>('a' + (rand_[random() % 10]->Next()) % 26);
    }
}

std::string GenerateFileName(int32_t file_num) {
    char* pod_name = getenv("GALAXY_POD_ID");
    if (!pod_name) {
        std::cerr << "fail to get pod name" << std::endl;
        abort();
    }
    std::string pname(pod_name);
    size_t pos = pname.size() - 1;
    while (pos > 0 && pname[pos] != '_') {
        pos--;
    }
    std::string pod_id(pname.begin() + pos + 1, pname.end());

    static std::string prefix = "/test/";
    return prefix + pod_id + "_" + baidu::common::NumToString(file_num);
}

void WriteFile(const std::string& file_name) {
    int32_t line_num = 1024 * 50;
    File *file = NULL;
    if (OK != fs_->OpenFile(file_name.c_str(),
                            O_WRONLY | O_TRUNC,
                            664, &file, WriteOptions())) {
        std::cerr << "OpenFile failed " << file_name << std::endl;
        return;
    }
    for (int i = 0; i < line_num; i++) {
        std::string content;
        RandomString(&content, 10 * (1 << 10));
        content += "\n";
        uint32_t len = file->Write(content.c_str(), content.size());
        if (len != content.size()) {
            std::cerr << "WriteFile failed " << file_name << std::endl;
            break;
        }
    }
    file->Close();
    delete file;
    std::cout << "FinishFile" << file_name << std::endl;
}

int main()
{
    //if (!FS::OpenFileSystem("nj02-bfs0.nj02:8828", &fs_, FSOptions())) {
    if (!FS::OpenFileSystem("localhost:8827", &fs_, FSOptions())) {
        std::cerr << "OpenFileSystem fail" << std::endl;
        return -1;
    }
    rand_ = new Random*[10];
    for (int i = 0; i < 10; i++) {
        rand_[i] = new Random(10 + i);
    }
    int32_t file_num = 0;
    while (file_num < 2 * 500) {
        srand(time(NULL));
        std::string file_name = GenerateFileName(file_num);
        WriteFile(file_name);
        file_num++;
    }
}
