#ifndef feature_utils_H
#define feature_utils_H
#include <vector>
#include <string>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct item_info {
    int rap;
    int value;
    int demand;
    int projected;
    int rare;
    int trend;
};

struct switches {
    bool ad_switch = false;
    bool filter_switch = false;
};

extern double eval_trade(std::vector<std::string> offer_ids, std::vector<std::string> receive_ids, int offer_robux, int receive_robux);
extern void filter_trades();
extern void start_trade_ads();
extern void cli();
extern void ping_cmd_line(std::string msg);
extern std::string query_interface(std::string msg);
extern item_info item_query(std::string item_id);
extern item_info get_item_info(std::string item_id, json item_data);
extern int query_trades_remaining();

inline switches switch_list;
inline bool cmd_wait;
inline std::string str_buffer;
inline httplib::Client roli_api("https://api.rolimons.com");
inline httplib::Client catalog_api("https://catalog.roblox.com");
inline httplib::Client roblox_apis("https://apis.roblox.com");
inline httplib::Client inventory_api("https://inventory.roblox.com");
inline httplib::Client trades_api("https://trades.roblox.com");
inline std::string rblx_cookie;
inline std::string roli_cookie;


#endif

#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif