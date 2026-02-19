cmake_minimum_required(VERSION 3.15)

set(CMAKE_POLICY_VERSION_MINIMUM "3.5")
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# --- VS KLASÖRLEMEYİ AÇ ---
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakePredefined")

project(Myro VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# --- Vendor ve Alt Projeler ---
add_subdirectory(vendor)

# Alias Tanımları
if(TARGET ogg AND NOT TARGET Ogg::ogg)
    add_library(Ogg::ogg ALIAS ogg)
endif()
if(TARGET opus AND NOT TARGET Opus::opus)
    add_library(Opus::opus ALIAS opus)
endif()
if(TARGET vorbis AND NOT TARGET Vorbis::vorbis)
    add_library(Vorbis::vorbis ALIAS vorbis)
endif()
if(TARGET vorbisenc AND NOT TARGET Vorbis::vorbisenc)
    add_library(Vorbis::vorbisenc ALIAS vorbisenc)
endif()

# --- Ana Projeler ---
add_subdirectory(Myro)
add_subdirectory(Myro-Examples)

# ========================================================
# AKILLI TEMİZLİK ROBOTU (Alt Klasörleme Desteği İle)
# ========================================================

function(fix_all_folders_recursive current_dir)
    get_property(targets DIRECTORY "${current_dir}" PROPERTY BUILDSYSTEM_TARGETS)
    
    foreach(t ${targets})
        if(NOT "${t}" STREQUAL "Myro" AND NOT "${t}" STREQUAL "Myro-Examples")
            get_target_property(current_folder ${t} FOLDER)
            
            # Eğer hedef ALL_BUILD gibi CMake özel hedefi değilse ayır:
            if(NOT "${current_folder}" STREQUAL "CMakePredefined")
                
                # İsimleri küçük harfe çevirip tarıyoruz
                string(TOLOWER "${t}" lower_t)
                set(subfolder "Vendor/Misc") # Ne olduğu bilinmeyenler buraya düşer
                
                # Hedefin ismine göre uygun Vendor alt klasörünü seç
                if(lower_t MATCHES "^(openal.*|common|ex-common|objects|bsincgen|make_hrm)$")
                    set(subfolder "Vendor/OpenAL")
                elseif(lower_t MATCHES "^ogg$")
                    set(subfolder "Vendor/Ogg")
                elseif(lower_t MATCHES "^vorbis.*")
                    set(subfolder "Vendor/Vorbis")
                elseif(lower_t MATCHES "^opus.*")
                    set(subfolder "Vendor/Opus")
                elseif(lower_t MATCHES "^(flac.*|getopt_static|grabbag_static|replaygain.*|utf8_static)$")
                    set(subfolder "Vendor/FLAC")
                elseif(lower_t MATCHES "^speex$")
                    set(subfolder "Vendor/Speex")
                elseif(lower_t MATCHES "^(miniaudio|minimp3|coco|dtlog|frenum)$")
                    set(subfolder "Vendor/SmallLibs")
                endif()
                
                # Klasörü ata
                set_target_properties(${t} PROPERTIES FOLDER "${subfolder}")
            endif()
        else()
            # Ana projeleri kendi isimleriyle klasörle
            set_target_properties(${t} PROPERTIES FOLDER "${t}")
        endif()
    endforeach()

    # Alt dizinleri gez
    get_property(subdirs DIRECTORY "${current_dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirs})
        fix_all_folders_recursive("${subdir}")
    endforeach()
endfunction()

# Robotu Çalıştır
fix_all_folders_recursive("${CMAKE_BINARY_DIR}")