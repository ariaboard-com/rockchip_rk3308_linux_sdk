////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRSONGMETADATA_H
#define DUEROS_DCS_APP_DLNA_DLNADMRSONGMETADATA_H

#include "DlnaDmrXmlEscape.h"
#include "DlnaDmrXmlDoc.h"

namespace duerOSDcsApp {
namespace dueros_dlna {

struct SongMetaData {
    const char* title;
    const char* artist;
    const char* album;
    const char* genre;
    const char* composer;
};

class SongMetaDataC {
public:
    static SongMetaDataC& get_instance() {
        static SongMetaDataC m_instance;
        return m_instance;
    }

    void songmetadata_init(struct SongMetaData* object);

    void songmetadata_clear(struct SongMetaData* object);

    char* songmetadata_to_didl(const struct SongMetaData* object,
                               const char* original_xml);

    int songmetadata_parse_didl(struct SongMetaData* object, const char* xml);

private:
    char* generate_didl(const char* id,
                        const char* title, const char* artist,
                        const char* album, const char* genre,
                        const char* composer);

    char* replace_range(char* input,
                        const char* tag_start, const char* tag_end,
                        const char* content,
                        int* edit_count);

};
}
}
#endif //DLNA_DMR_SONG_METADATA_H
