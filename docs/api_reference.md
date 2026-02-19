# Myro API Reference & Examples 📚

This guide provides practical, ready-to-use examples for the most common and advanced features of the **Myro** audio engine.

For the complete class and method documentation, please build the Doxygen documentation (`doxygen Doxyfile`) and open `docs/api/html/index.html`.

---

## 1. Engine Initialization & Teardown

Before calling any Myro functions, you must initialize the engine. This sets up the OpenAL context and the default audio device.

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

    // Cleanup
    myro::audio_engine::shutdown();
    return 0;
}
```

---

## 2. Fast Multi-Threaded Audio Loading

Myro is optimized for loading multiple files concurrently using a thread pool.

```cpp
myro::audio_engine::init();

// Set the number of threads for concurrent loading
myro::audio_engine::set_thread_count(6);

// Define files to load
std::vector<std::filesystem::path> files = {
    "assets/mp3_test.mp3",
    "assets/wav_test.wav",
    "assets/ogg_vorbis_test.ogg"
};

// Load all files instantly returning a vector of shared audio_source pointers
auto sources = myro::audio_engine::multi_load_audio_source(files);

// Play the first source
myro::audio_engine::play(sources[0]);
```

---

## 3. 3D Spatial Audio

Myro is built for 3D environments. It uses the `myro::vec3` struct to position the listener (e.g., the camera) and the sources.

```cpp
auto helicopter = myro::audio_engine::load_audio_source("assets/flac_test.flac");

// Manipulate Audio Source Attributes
helicopter->set_position(myro::vec3(20.0f, 50.0f, 0.0f));
helicopter->set_loop(true);
helicopter->set_spitial(true); // Enables 3D spatial properties
helicopter->set_gain(0.8f);

// Engine playback controls
myro::audio_engine::play(helicopter);

// Later in code...
myro::audio_engine::pause(helicopter);
myro::audio_engine::seek(helicopter, 15.5f); // Seek to 15.5 seconds
```

---

## 4. Audio Filters & Effects Manager

Myro handles audio modification through powerful static managers. You can easily apply or stack filters (like Low-Pass) and effects (like Reverb, Echo, Chorus) directly onto `audio_source` pointers.

```cpp
auto music = myro::audio_engine::load_audio_source("assets/bgm.ogg");

// Apply a Low-Pass Filter (Cuts off high frequencies. gain: 0.8f, gainHF: 0.1f)
myro::audio_filter_manager::apply_low_pass_filter(music, 0.8f, 0.1f);

// Apply an Echo Effect (delay: 0.5f, damping: 0.2f)
myro::audio_effect_manager::apply_echo(music, 0.5f, 0.2f);

myro::audio_engine::play(music);

// To remove them later:
// myro::audio_filter_manager::remove_filter(music, myro::audio_filter::low_pass_filter);
// myro::audio_effect_manager::remove_effect(music, myro::audio_effect::echo);
```

---

## 5. Microphone Capturing & Recording

Myro provides a high-level API for capturing audio from the default input device and directly streaming it to a file.
```cpp
#include <myro/myro.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    myro::audio_engine::init();

    myro::audio_capture mic;
    
    // Initialize the capture device and specify the output file directly
    // Here we record at 44100 Hz with 1 channel (Mono)
    if (!mic.init("output.wav", myro::audio_file_format::wav, 44100, 1)) {
        std::cerr << "Failed to open microphone!" << std::endl;
        return -1;
    }

    std::cout << "Recording for 5 seconds..." << std::endl;
    mic.start();

    // Wait for 5 seconds while it records and writes to the file
    std::this_thread::sleep_for(std::chrono::seconds(5));

    mic.stop();
    mic.deinit(); // Clean up hardware resources
    
    std::cout << "Recording saved successfully." << std::endl;

    myro::audio_engine::shutdown();
    return 0;
}
```

---

## 6. Global Engine & 3D Environment Settings

You can adjust the global properties of the engine to fit your physical world scale.
```cpp
// Change the global Speed of Sound and Doppler scale
myro::audio_engine::set_speed_of_sound(343.3f); // Default is 343.3f (m/s)
myro::audio_engine::set_doppler_factor(1.0f); // Adjust doppler effect intensity

// Check engine state
if (myro::audio_engine::is_active()) {
    std::cout << "Audio Engine is active and processing!" << std::endl;
}
```