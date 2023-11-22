#include <gtest/gtest.h>
#include <iostream>
#include <sndfile.h>
#include <string>
#include <subprocess.hpp>
#include <thread>
#include <vector>

using namespace subprocess;

struct wav_head;

class CommandLine : public ::testing::Test {
protected:
    std::atomic_bool sender_has_finished = false;

    void generate_wavfile(const std::string& filename,
                          int freq,
                          float amp,
                          int duration_ms,
                          int samplerate = 44100);
    void send_wavfile(const std::string& sender_endpoint, const std::string& input_file);
    void receive_wavfile(const std::string& receiver_endpoint,
                         const std::string& output_file);
    bool compare_wavfiles(const char* file1, const char* file2);
};
