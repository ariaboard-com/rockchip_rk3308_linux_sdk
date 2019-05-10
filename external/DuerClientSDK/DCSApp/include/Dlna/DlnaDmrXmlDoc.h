////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRXMLDOC_H
#define DUEROS_DCS_APP_DLNA_DLNADMRXMLDOC_H

#include "upnp/ixml.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
struct xmlelement {
};
struct xmldoc {
};

class XMLDoc {
public:
    static XMLDoc& get_instance() {
        static XMLDoc m_instance;
        return m_instance;
    }

    struct xmldoc* xmldoc_new(void);

    void xmldoc_free(struct xmldoc* doc);

    char* xmldoc_tostring(struct xmldoc* doc);

    struct xmldoc* xmldoc_parsexml(const char* xml_text);

    struct xmlelement* xmldoc_new_topelement(struct xmldoc* doc,
            const char* elementName,
            const char* xmlns);

    struct xmlelement* xmlelement_new(struct xmldoc* doc, const char* elementName);

    void xmlelement_add_element(struct xmldoc* doc,
                                struct xmlelement* parent,
                                struct xmlelement* child);

    void xmlelement_add_text(struct xmldoc* doc,
                             struct xmlelement* parent,
                             const char* text);

    void xmlelement_set_attribute(struct xmldoc* doc,
                                  struct xmlelement* element,
                                  const char* name,
                                  const char* value);

    void add_value_element(struct xmldoc* doc,
                           struct xmlelement* parent,
                           const char* tagname, const char* value);

    struct xmlelement* find_element_in_doc(struct xmldoc* doc,
                                           const char* key);

    struct xmlelement* find_element_in_element(struct xmlelement* element,
            const char* key);

    char* get_node_value(struct xmlelement* element);

    struct xmlelement* add_attributevalue_element(struct xmldoc* doc,
            struct xmlelement* parent,
            const char* tagname,
            const char* attribute_name,
            const char* value);

    void add_value_element_int(struct xmldoc* doc,
                               struct xmlelement* parent,
                               const char* tagname, int value);

    void add_value_element_long(struct xmldoc* doc,
                                struct xmlelement* parent,
                                const char* tagname, long long value);

private:
    IXML_Document* to_idoc(struct xmldoc* x);

    IXML_Element* to_ielem(struct xmlelement* x);

    struct xmlelement* find_element(IXML_Node* node, const char* key);
};
}
}
#endif //DLNA_DMR_XML_DOC_H
