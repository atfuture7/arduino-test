# for ALSA development
apt install libasound2-dev

# for PulseAudio development 
apt install libpulse-dev
# install PulseAudio
apt install pulseaudio


# to make examples in arduino-audio-tools runnable 
apt install alsa-utils
apt install libasound2-dev
apt install pulseaudio-utils

# to find package 
add-apt-repository universe multiverse
apt update
apt upgrade -y
apt search pulseaudio-alsa

