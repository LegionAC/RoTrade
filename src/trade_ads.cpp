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

httplib::Client ad_cli("https://api.rolimons.com");

httplib::Headers ad_headers({
    {"Cookie", ""}
});

auto send_trade(json j, int timer) {
    auto res = ad_cli.Post("/tradeads/v1/createad", ad_headers, j.dump(), "application/json");

    return res;
}

json csv_parse(std::string str, bool id) {
    json values = json::array();
    std::string buffer;

    for (char v : str) {
        if (v != ',') {
            buffer.push_back(std::tolower(v) );
        } else if (v == ' ') {
            continue;
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

    j["offer_item_ids"] = csv_parse(offer, true);

    j["request_item_ids"] = csv_parse(request, true);

    j["request_tags"] = csv_parse(tags, false);

    return j;
}

void ad_loop(std::string timer, json j) {
    while (ad_switch) {
        auto res = send_trade(j, stoi(timer));

        if (res->status == 201) {
            ping_cmd_line("Trade ad sent successfully. Waiting " + timer + " seconds.");
        } else if (res->status == 14) {
            ping_cmd_line("Trade ad limit reached... Waiting " + timer + " seconds.");
        } else {
            ping_cmd_line("Trade ad error: " + std::to_string(res->status) + "\nError code: " + res->body + "\n\n");
        }

        sleep_for(seconds(stoi(timer)));
    }
}

void start_trade_ads() {
    json j = ad_user_query();

    std::string timer;

    timer = query_interface("Please select time duration (seconds): ");

    std::thread loop_thread(ad_loop, timer, j);
    loop_thread.detach();
}