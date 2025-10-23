#define DEBUG

#include <pulse/simple.h>
#include <pulse/error.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include <iomanip> // For std::hex, std::setw, std::setfill

// Minimal WAV header
struct WAVHeader {
    char riff[4];             // "RIFF"
    uint32_t chunk_size;
    char wave[4];             // "WAVE"
    char fmt[4];              // "fmt "
    uint32_t fmt_size;
    uint16_t audio_format;    // 1 for PCM
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data[4];             // "data"
    uint32_t data_size;
};

void printBufferAsHex(unsigned char* buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::endl;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <audio.wav>\n";
        return 1;
    }

    const char* filename = argv[1];
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << '\n';
        return 1;
    }

    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

    if (std::strncmp(header.riff, "RIFF", 4) != 0 ||
        std::strncmp(header.wave, "WAVE", 4) != 0 ||
        std::strncmp(header.fmt, "fmt ", 4) != 0 ||
        header.audio_format != 1) {
        std::cerr << "Unsupported or invalid WAV file.\n";
        return 1;
    }

    // PulseAudio sample spec
    pa_sample_spec ss;
    ss.format = (header.bits_per_sample == 16) ? PA_SAMPLE_S16LE : PA_SAMPLE_U8;
    ss.rate = header.sample_rate;
    ss.channels = header.num_channels;

    int error;
    pa_simple* s = pa_simple_new(
        nullptr,             // Use default server
        "PulseAudio Player", // Application name
        PA_STREAM_PLAYBACK,
        nullptr,             // Default device
        "Playback",          // Stream description
        &ss,                 // Sample format
        nullptr,             // Default channel map
        nullptr,             // Default buffering attributes
        &error
    );

    if (!s) {
        std::cerr << "pa_simple_new() failed: " << pa_strerror(error) << '\n';
        return 1;
    }

    const size_t buffer_size = 4096;
    char buffer[buffer_size];

    // Playback loop
    while (file.read(buffer, buffer_size) || file.gcount() > 0) {
#ifdef DEBUG
	printBufferAsHex((unsigned char*)buffer, sizeof(buffer));
	break;
#endif
        if (pa_simple_write(s, buffer, file.gcount(), &error) < 0) {
            std::cerr << "pa_simple_write() failed: " << pa_strerror(error) << '\n';
            pa_simple_free(s);
            return 1;
        }
    }

    // Wait for playback to complete
    if (pa_simple_drain(s, &error) < 0) {
        std::cerr << "pa_simple_drain() failed: " << pa_strerror(error) << '\n';
    }

    pa_simple_free(s);
    std::cout << "Playback finished.\n";

    return 0;
}

