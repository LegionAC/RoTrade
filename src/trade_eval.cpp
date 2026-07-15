#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <vector>
#include <string>
#include <iostream>
#include "httplib.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct item_info {
    int rap;
    int value;
    int demand;
    int projected;
    int rare;
};

httplib::Client eval_cli("https://api.rolimons.com");

item_info populate_item_struct(json item) {
    item_info info;

    info.rap = item[2];
    info.value = item[4];
    info.demand = item[5];
    info.projected = item[7];
    info.rare = item[9];

    return info;
}

item_info get_item_info(std::string item_id) {
    auto res = eval_cli.Get("/items/v2/itemdetails");

    json data = json::parse(res->body);

    json target_item = data["items"][item_id];

    item_info info = populate_item_struct(target_item);

    return info;
}

std::vector<item_info> get_item_vector(std::vector<std::string> items) {
    std::vector<item_info> item_list;

    for (std::string v : items) {
        item_info info = get_item_info(v);

        item_list.push_back(info);
    }

    return item_list;
}

int trade_type(int offer_num, int receive_num) {
    if (offer_num < receive_num) {
        return 1; // DOWNGRADE
    } else if (offer_num > receive_num) {
        return -1; // UPGRADE
    } else {
        return 0; // EQUAL
    }
}

int value_sum(std::vector<item_info> offer, int robux) {
    int rap{0};

    rap += robux;

    for (item_info v : offer) {
        rap += v.value;
    }

    return rap;
}

bool has_proj(std::vector<item_info> offer) {
    for (item_info v : offer) {
        if (v.projected == 1) {
            return true;
        }
    }
    return false;
}

double demand_sum(std::vector<item_info> offer) {
    double sum{0};

    for (item_info v : offer) {
        if (v.demand == 0) {
            sum += 0.15;
        } else if (v.demand == 1) {
            sum += 0.1;
        } else if (v.demand == 3) {
            sum -= 0.1;
        } else if (v.demand == 4) {
            sum -=0.25;
        }
    }

    return sum / offer.size();
}

double eval_sum(std::vector<item_info> offer, std::vector<item_info> receive, int trade_type, int offer_sum, int overpay, bool player) {
    double sum{0};
    double baseline = offer_sum * 0.15;

    if (has_proj(receive) && player) {
        return -999999999;
    }

    for (item_info v : offer) {
        if (trade_type == 1 || trade_type == 0) {
            sum -= (double)overpay / offer_sum;
        } else {
            sum += (double)overpay / offer_sum;
        }

        sum += demand_sum(offer);

        sum -= demand_sum(receive);
    }

    return sum / offer.size();
}

double eval_trade(std::vector<std::string> offer_ids, std::vector<std::string> receive_ids, int offer_robux, int receive_robux) {
    std::vector<item_info> offer = get_item_vector(offer_ids);
    std::vector<item_info> receive = get_item_vector(receive_ids);

    int offer_type = trade_type(offer_ids.size(), receive_ids.size());

    int offer_rap = value_sum(offer, offer_robux);
    int receive_rap = value_sum(receive, receive_robux);

    int overpay = offer_rap - receive_rap;

    double offer_sum = eval_sum(offer, receive, offer_type, offer_rap, overpay, true);

    return offer_sum;
}