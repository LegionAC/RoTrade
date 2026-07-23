#include "feature_utils.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

using json = nlohmann::json;
using namespace std::this_thread;
using namespace std::chrono;

int trades_declined{0};
int trades_passed{0};
int trades_countered{0};

httplib::Headers filter_headers({
    {"Cookie", rblx_cookie},
    {"X-CSRF-TOKEN", ""}
});

httplib::Params params;

filter_info filter_user_query() {
    filter_info info;

    info.baseline = query_interface("Enter your maximum decline value (e.g 0.5): ");

    info.cooldown = query_interface("\nEnter the filter cooldown period (how often the program will review trades, in seconds): ");

    return info;
}

void post(std::string trade_id, std::string action) {
    auto res = trades_api.Post("/v1/trades/" + trade_id + action, filter_headers, params);

    if (res->status == 403) {
        std::string csrf = res->get_header_value("x-csrf-token");
        filter_headers.find("X-CSRF-TOKEN")->second = csrf;

        auto res = trades_api.Post("/v1/trades/" + trade_id + action, filter_headers, params);
    }
}

void filter_action(double eval, filter_info info, std::string trade_id) {
    double baseline = std::stod(info.baseline);

    if (switch_list.filter_decline && !switch_list.filter_counter && eval < baseline) {
        trades_declined += 1;
        post(trade_id, "/decline");
    }

    if (switch_list.filter_accept && eval >= baseline) {
        trades_passed += 1;
        post(trade_id, "/accept");
    }
}

void filter(json trades, filter_info info) {
    for (json v : trades["data"]) {

        std::string trade_id = v["id"].dump();

        auto res = trades_api.Get("/v2/trades/" + trade_id, filter_headers);

        json details = json::parse(res->body);

        std::vector<std::string> offer_itemIDs;
        std::vector<std::string> receive_itemIDs;

        for (json items : details["participantAOffer"]["items"]) {
            offer_itemIDs.push_back(items["itemTarget"]["targetId"].get<std::string>());
        }

        for (json items : details["participantBOffer"]["items"]) {
            receive_itemIDs.push_back(items["itemTarget"]["targetId"].get<std::string>());
        }

        int offer_robux = details["participantAOffer"]["robux"];
        int receive_robux = details["participantBOffer"]["robux"];

        double eval = eval_trade(offer_itemIDs, receive_itemIDs, offer_robux, receive_robux);

        filter_action(eval, info, trade_id);
    }

    std::string pass_msg = switch_list.filter_accept ? " trades accepted, " : " trades passed, ";
    
    ping_cmd_line(std::to_string(trades_declined) + " trades declined, " + std::to_string(trades_passed) + pass_msg + std::to_string(trades_countered) + " trades countered.");

    trades_declined = 0;
    trades_passed = 0;
    trades_countered = 0;
}

void filter_loop(filter_info info) {
    while (switch_list.filter_accept || switch_list.filter_decline || switch_list.filter_counter) {
        auto res = trades_api.Get("/v1/trades/Inbound?limit=100", filter_headers);

        if (res->status != 200) {
            switch_list.filter_switch = false;
            std::string err_msg = res->status == 401 ? "Invalid cookie. Retry /trade-filter." : "Unknown error occurred. Please screenshot and create a github issue.\n" + res->body;
            ping_cmd_line(err_msg);
            switch_list.filter_switch = false;
            break;
        }

        json trades = json::parse(res->body);

        filter(trades, info);
        sleep_for(seconds(std::stoi(info.cooldown)));
    }
}

void filter_trades() {
    if (switch_list.filter_switch == true) {
        std::cout << "\nTrade filter already running. Use /disable-filter.\n\n";
        return;
    }
    switch_list.filter_switch = true;

    filter_headers.find("Cookie")->second = ".ROBLOSECURITY=" + rblx_cookie;

    std::cout << "Trade filter starting...\n";

    std::thread filter_thread(filter_loop, filter_data);
    filter_thread.detach();
}