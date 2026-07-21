#include "feature_utils.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <cctype>

using json = nlohmann::json;
using namespace std::this_thread;
using namespace std::chrono;

httplib::Headers ad_headers({
    {"Cookie", ""}
});

auto send_trade(json j, int timer) {
    auto res = roli_api.Post("/tradeads/v1/createad", ad_headers, j.dump(), "application/json");

    return res;
}

json json_csv_parse(std::string str, bool id, bool vector) {
    std::string buffer;

    json values = json::array();

    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

    for (char v : str) {
        if (v != ',') {
            buffer.push_back(std::tolower(v) );
        } else if (!buffer.empty()) {
            if (id && buffer != "0") {
                values.push_back(std::stoi(buffer));
            } else if (buffer != "0") {
                values.push_back(buffer);
            }
            buffer = ""; 
        }
    }

    if (!buffer.empty() && buffer != "0") {
        if (id) {
            values.push_back(std::stoi(buffer));
        } else {
            values.push_back(buffer);
        }
    }

    return values;
}

json ad_user_query() {
    json j;

    std::string cookie;

    std::string id;
    std::string offer;
    std::string request;
    std::string tags;

    cmd_wait = true;

    id = query_interface("Enter your Rolimons player id: ");
    std::cout << "\nFor the following entries, separate your values with commas (csv) and input 0 to represent no value.";
    offer = query_interface("\nEnter your offer ids: ");
    request = query_interface("\nEnter your request ids: ");
    tags = query_interface("\nEnter your request tags: ");
    cookie = query_interface("\nEnter your Rolimons RoliVerification cookie: ");

    cmd_wait = false;

    ad_headers.find("Cookie")->second = "_RoliVerification=" + cookie;

    j["player_id"] = std::stoi(id);

    j["offer_item_ids"] = json_csv_parse(offer, true, false);

    j["request_item_ids"] = json_csv_parse(request, true, false);

    j["request_tags"] = json_csv_parse(tags, false, false);

    return j;
}

void ad_loop(std::string timer, json j) {
    while (switch_list.ad_switch) {
        auto res = send_trade(j, stoi(timer));

        if (res->status == 201) {
            ping_cmd_line("Trade ad sent successfully. Waiting " + timer + " seconds.");
        } else if (res->status == 400) {
            ping_cmd_line("Trade ad limit reached... Waiting " + timer + " seconds.");
        } else {
            ping_cmd_line("Trade ad error: " + std::to_string(res->status) + "\nError code: " + res->body);
            switch_list.ad_switch = false;
            break;
        }

        sleep_for(seconds(stoi(timer)));
    }
}

void start_trade_ads() {
    if (switch_list.ad_switch) {
        std::cout << "\nTrade ads already running. Use /disable-ads.\n\n";
        return;
    }
    switch_list.ad_switch = true;

    json j = ad_user_query();

    std::string timer;

    timer = query_interface("Please select time duration (seconds): ");

    std::thread loop_thread(ad_loop, timer, j);
    loop_thread.detach();
}