#include <alsa/asoundlib.h>
#include <iostream>
#include <fstream>

#define PCM_DEVICE "default"

// Minimal WAV header for PCM
struct WAVHeader {
    char riff_header[4]; // "RIFF"
    uint32_t wav_size;
    char wave_header[4]; // "WAVE"
    char fmt_header[4];  // "fmt "
    uint32_t fmt_chunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_header[4]; // "data"
    uint32_t data_bytes;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <audio.wav>" << std::endl;
        return 1;
    }

    const char* filename = argv[1];

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 1;
    }

    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

    if (std::string(header.riff_header, 4) != "RIFF" ||
        std::string(header.wave_header, 4) != "WAVE") {
        std::cerr << "Invalid WAV file format." << std::endl;
        return 1;
    }

    // ALSA setup
    snd_pcm_t* pcm_handle;
    snd_pcm_hw_params_t* params;
    snd_pcm_uframes_t frames;

    int pcm;

    pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    if (pcm < 0) {
        std::cerr << "ERROR: Can't open PCM device. " << snd_strerror(pcm) << std::endl;
        return 1;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    // Set format based on WAV file
    snd_pcm_format_t format;
    if (header.bits_per_sample == 16) {
        format = SND_PCM_FORMAT_S16_LE;
    } else if (header.bits_per_sample == 8) {
        format = SND_PCM_FORMAT_U8;
    } else {
        std::cerr << "Unsupported bit depth: " << header.bits_per_sample << std::endl;
        return 1;
    }

    snd_pcm_hw_params_set_format(pcm_handle, params, format);
    snd_pcm_hw_params_set_channels(pcm_handle, params, header.num_channels);
    snd_pcm_hw_params_set_rate(pcm_handle, params, header.sample_rate, 0);
    snd_pcm_hw_params(pcm_handle, params);

    snd_pcm_hw_params_get_period_size(params, &frames, 0);
    int buffer_size = frames * header.block_align;
    char* buffer = new char[buffer_size];

    // Read and play audio
    while (file.read(buffer, buffer_size)) {
        int err = snd_pcm_writei(pcm_handle, buffer, frames);
        if (err == -EPIPE) {
            snd_pcm_prepare(pcm_handle);
        } else if (err < 0) {
            std::cerr << "Write error: " << snd_strerror(err) << std::endl;
        }
    }

    // Final drain
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    delete[] buffer;
    file.close();

    std::cout << "Playback finished." << std::endl;
    return 0;
}

