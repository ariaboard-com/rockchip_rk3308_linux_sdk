////
// Created by eddy on 18-1-30.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Dlna/DlnaDmrSongMetaData.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
static const char kDidlHeader[] = "<DIDL-Lite "
                                  "xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" "
                                  "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
                                  "xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\">";
static const char kDidlFooter[] = "</DIDL-Lite>";

void SongMetaDataC::songmetadata_init(struct SongMetaData* value) {
    memset(value, 0, sizeof(struct SongMetaData));
}

void SongMetaDataC::songmetadata_clear(struct SongMetaData* value) {
    free((char*) value->title);
    value->title = NULL;
    free((char*) value->artist);
    value->artist = NULL;
    free((char*) value->album);
    value->album = NULL;
    free((char*) value->genre);
    value->genre = NULL;
}

int SongMetaDataC::songmetadata_parse_didl(struct SongMetaData* object, const char* xml) {
    struct xmldoc* doc = XMLDoc::get_instance().xmldoc_parsexml(xml);

    if (doc == NULL) {
        return 0;
    }

    struct xmlelement* didl_node = XMLDoc::get_instance().find_element_in_doc(doc, "DIDL-Lite");

    if (didl_node == NULL) {
        return 0;
    }

    struct xmlelement* item_node = XMLDoc::get_instance().find_element_in_element(didl_node,
                                   "item");

    if (item_node == NULL) {
        return 0;
    }

    struct xmlelement* value_node = NULL;

    value_node = XMLDoc::get_instance().find_element_in_element(item_node, "dc:title");

    if (value_node) {
        object->title = XMLDoc::get_instance().get_node_value(value_node);
    }

    value_node = XMLDoc::get_instance().find_element_in_element(item_node, "upnp:artist");

    if (value_node) {
        object->artist = XMLDoc::get_instance().get_node_value(value_node);
    }

    value_node = XMLDoc::get_instance().find_element_in_element(item_node, "upnp:album");

    if (value_node) {
        object->album = XMLDoc::get_instance().get_node_value(value_node);
    }

    value_node = XMLDoc::get_instance().find_element_in_element(item_node, "upnp:genre");

    if (value_node) {
        object->genre = XMLDoc::get_instance().get_node_value(value_node);
    }

    XMLDoc::get_instance().xmldoc_free(doc);
    return 1;
}


char* SongMetaDataC::songmetadata_to_didl(const struct SongMetaData* object,
        const char* original_xml) {
    static unsigned int xml_id = 42;
    char unique_id[4 + 8 + 1] = {0};
    snprintf(unique_id, sizeof(unique_id), "gmr-%08x", xml_id++);

    char* result = NULL;
    char* title = NULL; 
	char* artist = NULL;
	char* album = NULL;
	char* genre = NULL;
	char* composer = NULL;
    title = object->title ? XMLEscape::get_instance().xmlescape(object->title, 0) : NULL;
    artist = object->artist ? XMLEscape::get_instance().xmlescape(object->artist, 0) : NULL;
    album = object->album ? XMLEscape::get_instance().xmlescape(object->album, 0) : NULL;
    genre = object->genre ? XMLEscape::get_instance().xmlescape(object->genre, 0) : NULL;
    composer = object->composer ? XMLEscape::get_instance().xmlescape(object->composer, 0) : NULL;

    if (original_xml == NULL || strlen(original_xml) == 0) {
        result = generate_didl(unique_id, title, artist, album,
                               genre, composer);
    } else {
        int edits = 0;
        result = strdup(original_xml);
        result = replace_range(result, "<dc:title>", "</dc:title>",
                               title, &edits);
        result = replace_range(result,
                               "<upnp:artist>", "</upnp:artist>",
                               artist, &edits);
        result = replace_range(result, "<upnp:album>", "</upnp:album>",
                               album, &edits);
        result = replace_range(result, "<upnp:genre>", "</upnp:genre>",
                               genre, &edits);
        result = replace_range(result,
                               "<upnp:creator>", "</upnp:creator>",
                               composer, &edits);

        if (edits) {
            result = replace_range(result, "id=\"", "\"", unique_id,
                                   &edits);
        }
    }

    free(title);
    free(artist);
    free(album);
    free(genre);
    free(composer);
    return result;
}

char* SongMetaDataC::generate_didl(const char* id,
                                   const char* title, const char* artist,
                                   const char* album, const char* genre,
                                   const char* composer) {
    char* result = NULL;
    int ret = asprintf(&result, "%s\n<item id=\"%s\">\n"
                       "\t<dc:title>%s</dc:title>\n"
                       "\t<upnp:artist>%s</upnp:artist>\n"
                       "\t<upnp:album>%s</upnp:album>\n"
                       "\t<upnp:genre>%s</upnp:genre>\n"
                       "\t<upnp:creator>%s</upnp:creator>\n"
                       "</item>\n%s",
                       kDidlHeader, id,
                       title ? title : "", artist ? artist : "",
                       album ? album : "", genre ? genre : "",
                       composer ? composer : "",
                       kDidlFooter);
    return ret >= 0 ? result : NULL;
}

char* SongMetaDataC::replace_range(char* input,
                                   const char* tag_start, const char* tag_end,
                                   const char* content,
                                   int* edit_count) {
    if (content == NULL) {
        return input;
    }

    const int total_len = strlen(input);
    const char* start_pos = strstr(input, tag_start);

    if (start_pos == NULL) {
        return input;
    }

    start_pos += strlen(tag_start);
    const char* end_pos = strstr(start_pos, tag_end);

    if (end_pos == NULL) {
        return input;
    }

    int old_content_len = end_pos - start_pos;
    int new_content_len = strlen(content);
    char* result = NULL;

    if (old_content_len != new_content_len) {
        result = (char*) malloc(total_len
                                + new_content_len - old_content_len + 1);
        strncpy(result, input, start_pos - input);
        strncpy(result + (start_pos - input), content, new_content_len);
        strcpy(result + (start_pos - input) + new_content_len, end_pos);
        free(input);
        ++*edit_count;
    } else {
        if (strncmp(start_pos, content, new_content_len) != 0) {
            const int offset = start_pos - input;
            strncpy(input + offset, content, new_content_len);
            ++*edit_count;
        }

        result = input;
    }

    return result;
}

}
}