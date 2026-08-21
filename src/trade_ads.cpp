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
    {"Cookie", roli_cookie}
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
                values.push_back(std::stoll(buffer));
            } else if (buffer != "0") {
                values.push_back(buffer);
            }
            buffer = ""; 
        }
    }

    if (!buffer.empty() && buffer != "0") {
        if (id) {
            values.push_back(std::stoll(buffer));
        } else {
            values.push_back(buffer);
        }
    }

    return values;
}

json ad_user_query() {
    json j;

    j["player_id"] = std::stoll(roli_player_id);

    j["offer_item_ids"] = json_csv_parse(poster_offer, true, false);

    j["request_item_ids"] = json_csv_parse(poster_receive, true, false);

    j["request_tags"] = json_csv_parse(poster_receive_tags, false, false);

    return j;
}

void ad_loop(json j) {
    ad_headers.find("Cookie")->second = roli_cookie;
    while (switch_list.ad_switch) {
        auto res = send_trade(j, stoi(poster_cooldown));

        if (res->status == 201) {
            append_output("Trade ad sent successfully. Waiting " + poster_cooldown + " seconds.");
        } else if (res->status == 400) {
            append_output("Trade ad limit reached... Waiting " + poster_cooldown + " seconds.");
        } else {
            trade_ad_poster[5].text = "Enable";
            main_menu[3].text = "[3] Trade Ad Poster";
            append_output("Trade ad error: " + std::to_string(res->status) + "\nError code: " + res->body);
            switch_list.ad_switch = false;
            break;
        }

        draw();

        sleep_for(seconds(stoi(poster_cooldown)));
    }
}

void start_trade_ads() {
    switch_list.ad_switch = true;

    json j = ad_user_query();

    std::thread loop_thread(ad_loop, j);
    loop_thread.detach();
}