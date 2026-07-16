#include <iostream>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <string>
#include <gumbo.h>
#include "feature_utils.h"

httplib::Client parse_cli("https://www.rolimons.com");

web_scraped info;

GumboNode* find_sibling_node(GumboNode* child, GumboVector* children) {
    for (int i{0}; i < 2; i++) {
        GumboNode* sibling = static_cast<GumboNode*>(children->data[i]);

        if (sibling != child) return sibling;
    }

    return nullptr;
}

std::string get_text_from_node(GumboNode* node) {
    GumboVector* children = &node->v.element.children;

    for (int i{0}; i < children->length; i++) {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);

        if (child->type == GUMBO_NODE_TEXT) {
            return child->v.text.text;
        }
    }

    return "empty";
}

void data_query(GumboNode* child, GumboVector* children) {
    if (children->length != 2) return;

    GumboNode* sibling = find_sibling_node(child, children);

    if (!sibling || sibling->type != GUMBO_NODE_ELEMENT || child->type != GUMBO_NODE_ELEMENT) return;

    std::string sibling_str = get_text_from_node(sibling);
    std::string child_str = get_text_from_node(child);

    if (sibling_str == "Avg Daily Sales") {
        info.ADS = std::stod(child_str);
    }

    GumboVector* attributes = &sibling->v.element.attributes;

    GumboAttribute* cls = gumbo_get_attribute(attributes, "class");

    if (sibling_str == "Best Price" && cls && std::string(cls->value) == "value-stat-header") {
        child_str.erase(std::remove(child_str.begin(), child_str.end(), ','), child_str.end());
        info.best_price = std::stoi(child_str); 
    }
}

void walk(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) return;

    GumboVector* children = &node->v.element.children;

    for (int i{0}; i < children->length; i++) {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        data_query(child, children);
        walk(child);
    }
}

void parse_doc(std::string item_id) {
    auto res = parse_cli.Get("/item/" + item_id);

    GumboOutput* output = gumbo_parse(res->body.c_str());

    walk(output->root);
    
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

web_scraped item_query(std::string item_id) {
    parse_doc(item_id);

    return info;
}