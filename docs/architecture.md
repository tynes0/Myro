# Myro Architecture & Design 🏗️

This document outlines the high-level architecture of the **Myro** audio engine. Myro is designed with a strict emphasis on modularity, zero-cost abstractions, and thread safety using modern C++20 features.

## 1. High-Level Overview

Myro is divided into four main layers:

1. **Frontend API (Public Interfaces):** The classes that the end-user interacts with (`audio_engine`, `audio_source`, `listener`, etc.).
2. **Codec Subsystem (Loaders & Encoders):** A highly extensible system for decoding and encoding various audio formats.
3. **Core Subsystem:** Utility classes handling memory management (`raw_buffer`), threading (`thread_pool`), and logging.
4. **Backend (Hardware Abstraction):** The internal wrapper around the actual audio driver (currently `OpenAL Soft`).

---

## 2. Directory Structure & Modules

The `src/` directory is cleanly separated by responsibilities:

```text
Myro/src/
├── audio/               # Public API & Audio Entities
│   ├── detail/          # Backend implementations (OpenAL) & internal states
│   ├── loaders/         # Decoders (WAV, MP3, FLAC, Ogg, Opus, Speex)
│   └── encoders/        # Encoders (WAV, FLAC)
├── core/                # Memory, Concurrency, and Logging tools
├── math/                # Internal math library (vec2, vec3) for spatial audio
└── util/                # String/Stream formatters
```

---

## 3. The Core Components

### 3.1. Hardware Abstraction (The Backend)
Myro uses the **Pimpl (Pointer to Implementation)** idiom and hidden detail namespaces to completely isolate the user from OpenAL headers.
* `myro::audio_engine::init()` internally spins up the `myro::detail::openal_backend`.
* OpenAL context, device management, and extension loading (`ALC_ENUMERATION_EXT`, etc.) happen exclusively inside `openal_backend.cpp`.
* This design allows Myro to potentially support other backends (like XAudio2 or CoreAudio) in the future without changing the public API.

### 3.2. Memory Management (`myro::raw_buffer`)
Audio decoding often requires moving large chunks of raw PCM data around. To prevent memory leaks and minimize deep copies, Myro relies heavily on `myro::raw_buffer`.
* It acts as a smart wrapper around standard C++ memory containers.
* Decoders output a `raw_buffer`, and `audio_source` consumes it directly.

### 3.3. Concurrency (`myro::thread_pool`)
Modern games require non-blocking audio operations. Myro includes a custom, lightweight `thread_pool` built with C++20 `<thread>`, `<mutex>`, and `<condition_variable>`.
* Used internally for asynchronous audio streaming (preventing main-thread stutters during heavy disk I/O on large `.flac` or `.ogg` files).

---

## 4. The Codec Subsystem

Myro does not rely on a monolithic audio decoding library (like FFmpeg or libsndfile) which are often bloated and hard to compile. Instead, it uses a modular "Loader" approach:

* **Loaders (`myro::audio_engine::load_audio_source`):** When the user calls `load_audio_source()`, Myro inspects the file extension or file signature and routes it to the appropriate specific loader (`wav_loader`, `mp3_loader`, `flac_loader`, etc.).
* **Encoders (`myro::IEncoder`):** Myro defines a common `IEncoder` interface. Classes like `wav_encoder` and `flac_encoder` inherit from this, allowing users to capture microphone input or modify audio and save it back to disk.

---

## 5. Entities & 3D Spatial Audio

### `myro::listener`
Represents the user's ears (or the camera in a 3D game). There is only one active listener per context. It relies on the `myro::vec3` math struct for `position`, `velocity`, and `orientation`.

### `myro::audio_source`
Represents the sound emitter. A source takes audio data (`AL uint buffer` in the backend) and plays it in 3D space.
Features implemented:
* Pitch and Gain control.
* Distance attenuation models (Inverse, Linear, Exponential).
* Looping mechanisms.

### `myro::audio_effect` & `myro::audio_filter`
These wrap OpenAL's EFX extensions. Filters (like Low-Pass or High-Pass) are applied directly to `audio_source` objects, modifying the sound dynamically (e.g., muffling a sound when the player walks behind a wall).

---

## 6. Error Handling & Logging

Myro avoids C++ Exceptions (`throw`/`try-catch`) for audio processing loop efficiency.
Instead, it uses standard return codes and a thread-safe custom logger (`myro::log`).
For file I/O operations (like `fopen`, `fwrite`), the engine wraps standard C library calls and uses `std::error_code` with `std::system_category()` to ensure thread-safe error reporting.