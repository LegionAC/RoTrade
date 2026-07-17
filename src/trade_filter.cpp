#include "feature_utils.h"
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

using json = nlohmann::json;
using namespace std::this_thread;
using namespace std::chrono;

httplib::Client filter_cli("https://trades.roblox.com");

httplib::Headers filter_headers({
    {"Cookie", ""},
    {"X-CSRF-TOKEN", ""}
});

struct query_info {
    std::string cookie;
    std::string baseline;
    std::string cooldown;
};

query_info filter_user_query() {
    query_info info;

    info.cookie = query_interface("Enter your Roblox cookie: ");

    info.baseline = query_interface("\nEnter your maximum decline value (e.g 0.5): ");

    info.cooldown = query_interface("\nEnter the filter cooldown period (how often the program will review trades, in seconds): ");

    return info;
}

void decline_trades(json trades, query_info info) {
    int trades_declined{0};
    int trades_accepted{0};

    for (json v : trades["data"]) {

        std::string trade_id = v["id"].dump();

        auto res = filter_cli.Get("/v2/trades/" + trade_id, filter_headers);

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

        if (eval < std::stod(info.baseline)) {
            auto res = filter_cli.Post("/v1/trades/" + trade_id + "/decline", filter_headers);
            trades_declined += 1;

            if (res->status == 403) {
                std::string csrf = res->get_header_value("x-csrf-token");
                filter_headers.find("X-CSRF-TOKEN")->second = csrf;

                auto res = filter_cli.Post("/v1/trades/" + trade_id + "/decline", filter_headers);
            }
        } else {
            trades_accepted += 1;
        }
    }

    if (trades_declined > 0 || trades_accepted > 0) {
        ping_cmd_line(std::to_string(trades_declined) + " trades declined and " + std::to_string(trades_accepted) + " trades passed.");
    }
}

void filter_loop(query_info info) {
    while (switch_list.filter_switch) {
        auto res = filter_cli.Get("/v1/trades/Inbound", filter_headers);

        if (res->status != 200) {
            switch_list.filter_switch = false;
            std::string err_msg = res->status == 401 ? "Invalid user information or cookie expired. Retry /trade-filter." : "Unknown error occurred. Please screenshot and create a github issue.\n" + res->body;
            ping_cmd_line(err_msg);
            switch_list.filter_switch = false;
            break;
        }

        json trades = json::parse(res->body);

        decline_trades(trades, info);

        sleep_for(seconds(std::stoi(info.cooldown)));
    }
}

void filter_trades() {
    query_info info = filter_user_query();

    filter_headers.find("Cookie")->second = ".ROBLOSECURITY=" + info.cookie;

    std::cout << "Trade filter starting...\n\n";

    std::thread filter_thread(filter_loop, info);
    filter_thread.detach();
}