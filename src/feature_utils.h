#ifndef feature_utils_H
#define feature_utils_H
#include <vector>
#include <string>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <functional>

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
    bool filter_decline = false;
    bool filter_accept = false;
    bool filter_counter = false;
};

struct filter_info {
    std::string baseline;
    std::string cooldown;
};

struct menu_item {
    std::string text;
    std::function<void()> action;
    std::string orig_text;
    std::string* variable = nullptr;
};

extern double eval_trade(std::vector<std::string> offer_ids, std::vector<std::string> receive_ids, int offer_robux, int receive_robux);
extern void filter_trades(filter_info filter_data, int job);
extern void start_trade_ads();
extern void menu_nav();
extern item_info item_query(std::string item_id);
extern item_info get_item_info(std::string item_id, json item_data);
extern void enable_raw_mode();
extern void disable_raw_mode();
extern void append_output(std::string msg);
extern void draw();
extern void get_info(menu_item& item, std::string& value, std::string prefix = "");
extern void new_focus(std::vector<menu_item>& item);
extern void filter_decline(std::vector<menu_item>& item, int target, filter_info filter_data);
extern void filter_accept(std::vector<menu_item>& item, int target, filter_info filter_data);
extern void ad_poster(std::vector<menu_item>& item, int target);
extern void save_config();
extern void read_config();

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
inline void ping_cmd_line(std::string text) {};
inline filter_info decline_filter_data;
inline filter_info accept_filter_data;
inline filter_info counter_filter_data;
inline std::string ad_cooldown;
inline std::string roli_player_id;
inline std::string offer_ids;
inline std::string receive_ids;
inline std::string offer_robux;
inline std::string receive_robux;
inline std::string poster_offer;
inline std::string poster_receive;
inline std::string poster_cooldown;
inline std::string poster_receive_tags;
inline std::string sender_offer;
inline std::string sender_receive;
inline std::string sender_offer_robux;
inline std::string sender_receive_robux;
inline std::string output;

inline std::vector<menu_item> player_data = {
    {"COOKIES"}, {".ROBLOSECURITY=", [](){get_info(player_data[1], rblx_cookie);}, ".ROBLOSECURITY=", &rblx_cookie}, {"_RoliVerification=", [](){get_info(player_data[2], roli_cookie, "_RoliVerification=");}, "_RoliVerification=", &roli_cookie}, {"Rolimons Player ID=", [](){get_info(player_data[3], roli_player_id);}, "Rolimons Player ID=", &roli_player_id}
};

inline std::vector<menu_item> decline_filter_info = {
    {"FILTER INFO"}, {"Baseline=", [](){get_info(decline_filter_info[1], decline_filter_data.baseline);}, "Baseline=", &decline_filter_data.baseline}, {"Cooldown=", [](){get_info(decline_filter_info[2], decline_filter_data.cooldown);}, "Cooldown=", &decline_filter_data.cooldown}, {"Enable", [](){filter_decline(decline_filter_info, 3, decline_filter_data);}}
};

inline std::vector<menu_item> accept_filter_info = {
    {"FILTER INFO"}, {"Baseline=", [](){get_info(accept_filter_info[1], accept_filter_data.baseline);}, "Baseline=", &accept_filter_data.baseline}, {"Cooldown=", [](){get_info(accept_filter_info[2], accept_filter_data.cooldown);}, "Cooldown=", &accept_filter_data.cooldown}, {"Enable", [](){filter_accept(accept_filter_info, 3, accept_filter_data);}}
};

inline std::vector<menu_item> counter_filter_info;

inline std::vector<menu_item> trade_filter = {
    {"TRADE FILTER"}, {"[1] Auto-decline", [](){new_focus(decline_filter_info);}}, {"[2] Auto-accept", [](){new_focus(accept_filter_info);}}, {"[3] Auto-counter"}
};

inline std::vector<menu_item> eval = {
    {"TRADE EVAL"}, {"[1] Offer IDs="}, {"[2] Receive IDs="}, {"[3] Offer Robux="}, {"[4] Receive Robux"}
};

inline std::vector<menu_item> saved_configs = {
    {"SAVED CONFIGS"}, {"[1] Save Config", [](){save_config();}}, {"[2] Load Config", [](){read_config();}}
};

inline std::vector<menu_item> config = {
    {"CONFIG"}, {"[1] Cookies", [](){new_focus(player_data);}}, {"[2] Trade Evaluation"}, {"[3] Saved Configs", [](){new_focus(saved_configs);}}
};

inline std::vector<menu_item> trade_ad_poster = {
    {"TRADE AD POSTER"}, {"Offer=", [](){get_info(trade_ad_poster[1], poster_offer);}, "Offer=", &poster_offer}, {"Receive=", [](){get_info(trade_ad_poster[2], poster_receive);}, "Receive=", &poster_receive}, {"ReceiveTags=", [](){get_info(trade_ad_poster[3], poster_receive_tags);}, "ReceiveTags=", &poster_receive_tags}, {"Cooldown=", [](){get_info(trade_ad_poster[4], poster_cooldown);}, "Cooldown=", &poster_cooldown}, {"Enable", [](){ad_poster(trade_ad_poster, 5);}}
};

inline std::vector<menu_item> mass_trade_sender = {
    {"MASS TRADE SENDER"}, {"Offer=", [](){get_info(mass_trade_sender[1], sender_offer);}, "Offer=", &sender_offer}, {"Receive=", [](){get_info(mass_trade_sender[2], sender_receive);}, "Receive=", &sender_receive}, {"OfferRobux=", [](){get_info(mass_trade_sender[3], sender_offer_robux);}, "OfferRobux=", &sender_offer_robux}, {"ReceiveRobux=", [](){get_info(mass_trade_sender[4], sender_receive_robux);}, "ReceiveRobux=", &sender_receive_robux}, {"Enable"}
};

inline std::vector<menu_item> main_menu = {
    {"MENU"}, {"[1] Config", [](){new_focus(config);}}, {"[2] Trade Filter", [](){new_focus(trade_filter);}}, {"[3] Trade Ad Poster", [](){new_focus(trade_ad_poster);}}, {"[4] Mass Trade Sender", [](){new_focus(mass_trade_sender);}}
};

inline std::vector<std::pair<std::string, std::string*>> config_ptrs = {
    {"rblx_cookie", &rblx_cookie},
    {"roli_cookie", &roli_cookie},
    {"ad_cooldown", &ad_cooldown},
    {"roli_player_id", &roli_player_id},
    {"offer_ids", &offer_ids},
    {"receive_ids", &receive_ids},
    {"offer_robux", &offer_robux},
    {"receive_robux", &receive_robux},
    {"poster_offer", &poster_offer},
    {"poster_receive", &poster_receive},
    {"poster_cooldown", &poster_cooldown},
    {"poster_receive_tags", &poster_receive_tags},
    {"sender_offer", &sender_offer},
    {"sender_receive", &sender_receive},
    {"sender_offer_robux", &sender_offer_robux},
    {"sender_receive_robux", &sender_receive_robux},
    {"decline_baseline", &decline_filter_data.baseline},
    {"decline_cooldown", &decline_filter_data.cooldown},
    {"accept_baseline", &accept_filter_data.baseline},
    {"accept_cooldown", &accept_filter_data.cooldown},
    {"counter_baseline", &counter_filter_data.baseline},
    {"counter_cooldown", &counter_filter_data.cooldown}
};

inline std::vector<std::reference_wrapper<std::vector<menu_item>>> menu_objs = {
    player_data,
    decline_filter_info,
    accept_filter_info,
    counter_filter_info,
    trade_filter,
    eval,
    saved_configs,
    config,
    trade_ad_poster,
    mass_trade_sender,
    main_menu
};

#endif