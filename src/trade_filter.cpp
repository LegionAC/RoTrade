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

void post(std::string trade_id, std::string action) {
    auto res = trades_api.Post("/v1/trades/" + trade_id + action, filter_headers, params);
    httplib::Headers copy = filter_headers;

    if (res->status == 403) {
        std::string csrf = res->get_header_value("x-csrf-token");
        filter_headers.find("X-CSRF-TOKEN")->second = csrf;

        auto res = trades_api.Post("/v1/trades/" + trade_id + action, filter_headers, params);
    }
}

void filter_action(double eval, filter_info info, std::string trade_id) {
    double baseline = std::stod(info.baseline);

    if (switch_list.filter_decline && !switch_list.filter_counter && eval <= baseline) {
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

        auto current_data_res = roli_api.Get("/items/v2/itemdetails");
        json data = json::parse(current_data_res->body);
        
        double eval = eval_trade(offer_itemIDs, receive_itemIDs, offer_robux, receive_robux, data);

        filter_action(eval, info, trade_id);
    }
}

void filter_loop(filter_info info, int job) {
    std::vector<menu_item>* item = (job == 0) ? &decline_filter_info : (job == 1) ? &accept_filter_info :  &counter_filter_info;
    bool* target_switch = (job == 0) ? &switch_list.filter_decline : (job == 1) ? &switch_list.filter_accept : &switch_list.filter_counter;
    std::string str;
    int target;

    switch (job) {
        case 0: str = "[1] Auto-decline"; target = 1; break;
        case 1: str = "[2] Auto-accept"; target = 2; break;
        case 2: str = "[3] Auto-counter"; target = 3; break;
    };

    while (target_switch) {
        auto res = trades_api.Get("/v1/trades/Inbound?limit=100", filter_headers);
        if (res->status != 200) {
            *target_switch = false;
            ((*item)[3]).text = "Enable";
            trade_filter[target].text = str;
            std::string err_msg = res->status == 401 ? "Invalid cookie. Reset your cookie in Config --> Cookies --> .ROBLOSECURITY" : "Unknown error occurred. Please screenshot and create a github issue." + res->body;
            append_output(err_msg);
            draw();
            break;
        }
        json trades = json::parse(res->body);
        filter(trades, info);
        sleep_for(seconds(std::stoi(info.cooldown)));
    }
}

void filter_trades(filter_info filter_data, int job) {  // job 0 is decline, 1 is accept 2 is counter
    switch_list.filter_switch = true;

    filter_headers.find("Cookie")->second = ".ROBLOSECURITY=" + rblx_cookie;

    output = "Trade filter starting...\n";

    std::thread filter_thread(filter_loop, filter_data, job);
    filter_thread.detach();
}