include(FetchContent)

# 1. Kaynak Kodları İndir (Zip olarak)
FetchContent_Declare(
    opusfile
    URL https://github.com/xiph/opusfile/archive/refs/tags/v0.12.zip
)
FetchContent_MakeAvailable(opusfile)

# 2. Kaynak Dosyaları Seç
# (Opusfile'ın src klasöründeki .c dosyalarını topluyoruz)
file(GLOB OPUSFILE_SOURCES "${opusfile_SOURCE_DIR}/src/*.c")

# 3. HTTP ve Gereksizleri Çıkar
# (OpenSSL bağımlılığından kurtulmak için http.c'yi derlemeye dahil etmiyoruz)
list(FILTER OPUSFILE_SOURCES EXCLUDE REGEX "http.c")
list(FILTER OPUSFILE_SOURCES EXCLUDE REGEX "wincerts.c")

# 4. Kütüphaneyi Oluştur
add_library(opusfile STATIC ${OPUSFILE_SOURCES})

# 5. Include Yollarını Tanıt (opusfile.h burada)
target_include_directories(opusfile PUBLIC 
    "${opusfile_SOURCE_DIR}/include"
)

# 6. Define Ayarları
target_compile_definitions(opusfile PRIVATE 
    OP_DISABLE_HTTP          # HTTP kapat
    _CRT_SECURE_NO_WARNINGS  # Uyarıları kapat
)

# 7. Bağımlılıkları Bağla
# (Opusfile, Ogg ve Opus olmadan çalışmaz)
target_link_libraries(opusfile PRIVATE 
    Ogg::ogg 
    Opus::opus
)