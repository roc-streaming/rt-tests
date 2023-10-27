#include "command_line.h"

using namespace subprocess;

struct wav_head {
    char riff_tag[4] = { 'R', 'I', 'F', 'F' };
    int32_t overall_size;
    char wave_tag[4] = { 'W', 'A', 'V', 'E' };
    char fmt_tag[4] = { 'f', 'm', 't', ' ' };
    int32_t fmt_length = 16;
    int16_t format_type = 1;
    int16_t channels = 2;
    int32_t samplerate;
    int32_t byterate;
    int16_t block_align;
    int16_t bits_per_sample = 16;
    char data_tag[4] = { 'd', 'a', 't', 'a' };
    int32_t data_size;
};

void CommandLine::generate_wavfile(
    const std::string& filename, int freq, float amp, int duration_ms, int samplerate) {
    const int num_samples = duration_ms * samplerate / 1000;
    int32_t data_size = num_samples * 2 * 2; // x2 for stereo, x2 for 16-bit
    int32_t overall_size = data_size + sizeof(wav_head) - 8;

    wav_head header;
    header.samplerate = samplerate;
    header.byterate = samplerate * header.bits_per_sample / 8 * header.channels;
    header.block_align = header.bits_per_sample / 8 * header.channels;
    header.data_size = data_size;
    header.overall_size = overall_size;

    FILE* file = fopen(filename.c_str(), "wb");

    if (!file) {
        printf("Could not open %s for writing\n", filename.c_str());
        return;
    }

    fwrite(&header, sizeof(header), 1, file);

    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / samplerate;
        int16_t sample = static_cast<int16_t>(amp * 32767.0 * sin(2.0 * M_PI * freq * t));
        // Write for left channel
        fwrite(&sample, sizeof(int16_t), 1, file);
        // Write for right channel
        fwrite(&sample, sizeof(int16_t), 1, file);
    }

    fclose(file);
}

void CommandLine::send_wavfile(const std::string& sender_endpoint,
                               const std::string& input_file) {
    std::string send_command =
        "roc-send -vv -i file:./" + input_file + " -s " + sender_endpoint;
    try {
        Popen process(send_command, output { PIPE }, error { PIPE });
        auto res = process.communicate().first.buf.data();
        std::cout << "Wav file sent successfully!\n" << res << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error sending wav file: " << e.what() << std::endl;
    }
}

void CommandLine::receive_wavfile(const std::string& receiver_endpoint,
                                  const std::string& output_file) {
    std::string receive_command =
        "roc-recv -vv -s " + receiver_endpoint + " -o file:./" + output_file;
    try {
        Popen process(receive_command, output { PIPE }, error { PIPE });
        std::cout << "Successfully open!" << std::endl;

        while (!sender_has_finished) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        process.kill();
        auto res = process.communicate().first.buf.data();
        std::cout << "Wav file received successfully!\n" << res << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error receiving wav file: " << e.what() << std::endl;
    }
}

bool CommandLine::compare_wavfiles(const char* file1, const char* file2) {
    SF_INFO sfinfo1, sfinfo2;
    SNDFILE* sndfile1 = sf_open(file1, SFM_READ, &sfinfo1);
    SNDFILE* sndfile2 = sf_open(file2, SFM_READ, &sfinfo2);

    // Check whether the file is successfully opened
    if (!sndfile1 || !sndfile2) {
        sf_close(sndfile1);
        sf_close(sndfile2);
        std::cout << "Failed to open the file" << std::endl;
        return false;
    }

    // Compare file attributes
    if (sfinfo1.channels != sfinfo2.channels
        || sfinfo1.samplerate != sfinfo2.samplerate) {
        sf_close(sndfile1);
        sf_close(sndfile2);
        std::cout << "File attributes do not match" << std::endl;
        return false;
    }

    // Compare audio data
    const int buffer_size = 4096;
    double buffer1[buffer_size];
    double buffer2[buffer_size];
    sf_count_t frames_read1, frames_read2;

    while (true) {
        frames_read1 = sf_readf_double(sndfile1, buffer1, buffer_size);
        frames_read2 = sf_readf_double(sndfile2, buffer2, buffer_size);

        if (frames_read1 != frames_read2) {
            sf_close(sndfile1);
            sf_close(sndfile2);
            std::cout << "File data do not match" << std::endl;
            return false;
        }

        if (frames_read1 == 0) {
            break;
        }

        for (sf_count_t i = 0; i < frames_read1; i++) {
            if (buffer1[i] != buffer2[i]) {
                sf_close(sndfile1);
                sf_close(sndfile2);
                std::cout << "File data do not match" << std::endl;
                return false;
            }
        }
    }

    sf_close(sndfile1);
    sf_close(sndfile2);
    return true;
}

TEST_F(CommandLine, SendReceiveWavFile) {
    std::string filename = "test.wav";
    generate_wavfile(filename, 440, 0.5, 1000);

    std::string sender_endpoint = "rtp://127.0.0.1:50127";
    std::string receiver_endpoint = "rtp://127.0.0.1:50127";
    std::string received_file = "received.wav";

    // Reset the sender_has_finished flag before using
    sender_has_finished = false;

    // Start the receiver in a separate thread first
    std::thread receiver_thread([this, &receiver_endpoint, &received_file]() {
        this->receive_wavfile(receiver_endpoint, received_file);
    });

    // Then start the sender
    std::thread sender_thread([this, &sender_endpoint, &filename]() {
        this->send_wavfile(sender_endpoint, filename);
        this->sender_has_finished = true; // indicate that the sender has finished
    });

    // Wait for both threads to finish
    receiver_thread.join();
    sender_thread.join();

    EXPECT_TRUE(compare_wavfiles(filename.c_str(), received_file.c_str()))
        << "The contents of the original file and the received file do not match!";
}
