# Myro 🎵
[![Windows CI](https://github.com/tynes0/Myro/actions/workflows/build-windows.yml/badge.svg)](https://github.com/tynes0/Myro/actions/workflows/build-windows.yml)
[![Linux CI](https://github.com/tynes0/Myro/actions/workflows/build-linux.yml/badge.svg)](https://github.com/tynes0/Myro/actions/workflows/build-linux.yml)
[![macOS CI](https://github.com/tynes0/Myro/actions/workflows/build-macos.yml/badge.svg)](https://github.com/tynes0/Myro/actions/workflows/build-macos.yml)
> A Modern, High-Performance, and Cross-Platform C++20 Audio Engine. **Myro** is an advanced audio engine and processing library built for modern C++ applications. Powered by an `OpenAL Soft` backend, it provides a seamless and high-level API for 3D spatial audio, audio effects, and multi-format decoding/encoding.

It features a completely automated build system via CMake `FetchContent`, meaning **zero manual dependency management**. Just clone and build.

## ✨ Features

* **Modern C++20 Architecture:** Built with standard library features, thread-pools, and smart memory buffers (`myro::raw_buffer`).
* **3D Spatial Audio:** Fully supports 3D positioning, listeners, velocity, and distance models.
* **Rich Format Support:** Automatically decodes and encodes popular audio formats without requiring external codec installations.
* **Audio Effects & Filters:** Built-in support for real-time audio manipulation.
* **Audio Capturing:** Record audio from microphone inputs easily.
* **Zero Dependency Setup:** All third-party libraries (OpenAL, Vorbis, FLAC, etc.) are fetched and built automatically via CMake.

## 🎧 Supported Audio Formats

| Format | Decoding (Playback) | Encoding (Export) | Powered By |
| :--- | :---: | :---: | :--- |
| **WAV** | ✅ | ✅ | *dr_wav (miniaudio)* |
| **MP3** | ✅ | ✅ | *minimp3, lame* |
| **OGG / Vorbis**| ✅ | ✅ | *libogg, libvorbis* |
| **FLAC** | ✅ | ✅ | *libFLAC* |
| **Opus** | ✅ | ✅ | *libopus, opusfile* |
| **Speex** | ✅ | ✅ | *libspeex* |

---

## 🚀 Getting Started

### Prerequisites
* A C++20 compatible compiler (MSVC, Clang, or GCC)
* [CMake](https://cmake.org/download/) (3.15 or higher)
* *Optional: Visual Studio 2022 (Highly recommended for Windows)*

### Building the Project

Myro takes care of all its dependencies automatically. 

1. **Clone the repository:**
```bash
git clone [https://github.com/tynes0/Myro.git](https://github.com/tynes0/Myro.git)
cd Myro
```

2. **Generate the build files & Compile:**
```bash
cd scripts
.\BuildEngine.bat or .\BuildEngine.sh
.\GenerateProject.bat or .\GenerateProject.sh
```
*(If you are using Visual Studio, simply open the `.sln` file generated in the `build` folder. `Myro-Examples` is already set as the default startup project!)*

---

## 📖 Quick Start & API Overview

Myro is designed to be extremely intuitive. Here is how you initialize the engine and play a sound.

### 1. Initialization and Playback

```cpp
#include <myro/myro.h>
#include <iostream>

int main() {
    // Initialize the Audio Engine
    myro::audio_engine::init();

    // Load an audio source directly through the engine
    // The format is automatically handled.
    auto source = myro::audio_engine::load_audio_source("assets/mp3_test.mp3");

    // Play the source via the engine
    myro::audio_engine::play(source);

    while (myro::audio_engine::state_of(source) == myro::audio_state::playing)
    {
        // .....
    }

    // Cleanup
    myro::audio_engine::shutdown();
    return 0;
}
```

### 2. 3D Spatial Audio
```cpp
myro::listener::set_position(myro::vec3(0.0f, 0.0f, 0.0f));


std::shared_ptr<myro::audio_source> fire_sfx = myro::audio_engine::load_audio_source("assets/fire.ogg");

// Place the fire 10 units to the right
fire_sfx->set_position(myro::vec3(10.0f, 0.0f, 0.0f));
fire_sfx->set_loop(true);
myro::audio_engine::play(fire_sfx);
```

---

## 🏗️ Project Structure

The project is highly modular. Under the hood, the Visual Studio solution is cleanly organized:

* **`Myro`**: The core static library.
  * `src/audio/`: Core engine, sources, listeners, and effects.
  * `src/audio/loaders/`: Format-specific decoding logic.
  * `src/audio/encoders/`: Format-specific encoding logic.
  * `src/core/`: Threading, logging, and buffer management.
  * `src/math/`: Internal math library (`vec2`, `vec3`) for spatial calculations.
* **`Myro-Examples`**: The sandbox application for testing and demonstrating features.
* **`Vendor`**: Automatically managed 3rd-party dependencies (OpenAL, FLAC, Speex, Vorbis, etc.). Cleanly grouped in the IDE.

## 📜 License
This project is licensed under the terms of the included LICENSE file. (See [LICENSE](LICENSE) for details).