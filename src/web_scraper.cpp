#include <iostream>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <string>
#include <gumbo.h>
#include "feature_utils.h"

httplib::Client parse_cli("https://www.rolimons.com");

item_info info;

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

    GumboVector* attributes = &sibling->v.element.attributes;

    GumboAttribute* cls = gumbo_get_attribute(attributes, "class");

    if (sibling_str == "RAP" && cls && std::string(cls->value) == "value-stat-header") {
        child_str.erase(std::remove(child_str.begin(), child_str.end(), ' '), child_str.end());
        info.rap = std::stoi(child_str);
    } else if (sibling_str == "Value" && cls && std::string(cls->value) == "value-stat-header") {
        child_str.erase(std::remove(child_str.begin(), child_str.end(), ' '), child_str.end());
        if (child_str != "NotAssigned") {
            info.value = std::stoi(child_str);
        } else {
            info.value = -1;
        }
    } else if (sibling_str == "Demand" && cls && std::string(cls->value) == "value-stat-header") {
        if (child_str == "Amazing") {
            info.demand = 4;
        } else if (child_str == "High") {
            info.demand = 3;
        } else if (child_str == "Normal") {
            info.demand = 2;
        } else if (child_str == "Low") {
            info.demand = 1;
        } else if (child_str == "Terrible") {
            info.demand = 0;
        } else if (child_str == "NotAssigned") {
            info.demand = -1;
        }
    } else if (sibling_str == "Trend" && cls && std::string(cls->value) == "card-subtitle mt-1 text-muted stat-header stat-header") {
        if (child_str == "Fluctuating") {
            info.trend = 4;
        } else if (child_str == "Raising") {
            info.trend = 3;
        } else if (child_str == "Stable") {
            info.trend = 2;
        } else if (child_str == "Unstable") {
            info.trend = 1;
        } else if (child_str == "Lowering") {
            info.trend = 0;
        } else if (child_str == "Not    Assigned") {
            info.trend = -1;
        }
    }

    if (child_str == "Projected") {
        info.projected = 1;
    } else if (child_str == "Rare") {
        info.rare = 1;
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
    auto res = parse_cli.Get("/bundle/" + item_id);

    GumboOutput* output = gumbo_parse(res->body.c_str());

    walk(output->root);
    
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

item_info item_query(std::string item_id) {
    parse_doc(item_id);

    if (info.value == -1) info.value = info.rap;

    return info;
}