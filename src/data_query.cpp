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

item_info populate_item_struct(json item, json item_history, std::string item_id) {
    item_info info;

    info.rap = item[2];
    info.value = item[4];
    info.demand = item[5];
    info.projected = item[7];
    info.rare = item[9];

    if (item[2] == item[4] || info.rare == 1) return info;

    int current_rap = item[2];

    int last_rap = current_rap;

    std::vector<int> median_rap_container;

    std::vector<int> median_diff_container;

    for (int i{0}; i < 14; i++) {
        if (item_history["priceDataPoints"][i]["value"].is_null()) break;

        int rap = item_history["priceDataPoints"][i]["value"];

        median_rap_container.push_back(rap);

        median_diff_container.push_back(rap - last_rap);

        last_rap = rap;
    }

    int rap_median = get_median(median_rap_container);

    int diff_median = std::abs(get_median(median_diff_container));

    web_scraped scraped_info = item_query(item_id);

    int bidaily_sales = static_cast<int>(std::round(scraped_info.ADS * 2));

    int diff = current_rap - rap_median;

    int sales_required = diff_median != 0 ? std::abs(diff) / diff_median : 0;

    int extrapolated = bidaily_sales * diff_median;

    if (current_rap > rap_median) {
        if (scraped_info.best_price > rap_median) {
            info.extra_rap = (bidaily_sales >= sales_required) ? rap_median : rap_median;
        } else {
            info.extra_rap = (bidaily_sales >= sales_required) ? rap_median : current_rap - extrapolated;
        }
    } else {
        info.extra_rap = (bidaily_sales >= sales_required) ? rap_median : current_rap + extrapolated;
    }

    info.ADS = scraped_info.ADS;
    // to-do: measure volatility and measure momentum.
    return info;
}

std::string get_bundle_item_id(std::string item_name, json data) {
    for (auto it : data["items"].items()) {
        std::string current_name = it.value()[0];
        if (current_name == item_name || current_name == item_name + " ") {
            return it.key();
        }
    }

    return "";
}

item_info get_item_info(std::string item_id, json item_data) {
    auto uuid_res = catalog_api.Get("/v1/catalog/items/" + item_id + "/details?itemType=asset");

    bool bundle = uuid_res->status == 400 ? true : false;

    if (bundle) uuid_res = catalog_api.Get("/v1/catalog/items/" + item_id + "/details?itemType=bundle");

    json uuid_data = json::parse(uuid_res->body);

    std::string uuid = uuid_data["collectibleItemId"];

    std::string name = uuid_data["name"];

    auto history_res = roblox_apis.Get("/marketplace-sales/v1/item/" + uuid + "/resale-data");

    json history = json::parse(history_res->body);

    if (bundle) item_id = get_bundle_item_id(name, item_data);

    json target_item = item_data["items"][item_id];

    item_info info = populate_item_struct(target_item, history, item_id);

    return info;
}