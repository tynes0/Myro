#include "speex_loader.h"


#include "ogg_loader.h"

#include "../detail/audio_data.h"
#include "../detail/detail.h"
#include "../detail/openal_backend.h"

#include <speex/speex.h>
#include <speex/speex_header.h>
#include <speex/speex_callbacks.h>
#include <ogg/ogg.h>

namespace myro
{
    void speex_loader::init()
    {
    }

    void speex_loader::shutdown()
    {
    }

    raw_buffer speex_loader::load(const std::filesystem::path& filepath)
    {
        if (ogg_loader::is_ogg_container(filepath))
            return load_ogg_speex(filepath);


        MYRO_UNIMPLAMENTED();

        raw_buffer final;
        return final;
    }

    raw_buffer speex_loader::load_ogg_speex(const std::filesystem::path& filepath)
    {
        MYRO_UNUSED(filepath);
        MYRO_UNIMPLAMENTED();

        raw_buffer final;
        return final;
    }
}