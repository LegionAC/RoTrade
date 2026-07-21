#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "feature_utils.h"
#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>
#include <vector>

using json = nlohmann::json;

int get_median(std::vector<int> container) {
    std::sort(container.begin(), container.end());

    double size = container.size();

    double median = size / 2;

    if (median != std::floor(size)) {
        return (container[median + 0.5] + container[median - 0.5]) / 2;
    } else {
        return container[median];
    }

    return 0;
}

item_info populate_item_struct(json item, std::string item_id) {
    item_info info;

    info.rap = item[2];
    info.value = item[4];
    info.demand = item[5];
    info.trend = item[6];
    info.projected = item[7];
    info.rare = item[9];

    item_info scraped_info =  item_query(item_id);

    // to-do: measure volatility and measure momentum.
    return info;
}

std::string get_bundle_item_id(std::string item_name, json data) {
    for (auto it : data["items"].items()) {
        auto current_name = it.value()[0];

        if (current_name == item_name || current_name == item_name + " ") {
            return it.key();
        }
    }

    return "";
}

item_info get_item_info(std::string item_id, json item_data) {
    json target_item = item_data["items"][item_id];

    if (target_item.is_null()) {
        auto bundle_info = catalog_api.Get("/v1/bundles/" + item_id + "/details");
        json data = json::parse(bundle_info->body);
        std::string name = data["name"];

        std::string roli_item_id = get_bundle_item_id(name, item_data);

        if (roli_item_id.empty()) return item_query(item_id);

        target_item = item_data["items"][roli_item_id];
    }

    item_info info = populate_item_struct(target_item, item_id);
    return info;
}