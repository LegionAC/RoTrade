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
    int median;
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

extern double eval_trade(std::vector<std::string> offer_ids, std::vector<std::string> receive_ids, int offer_robux, int receive_robux, json data);
extern void filter_trades(filter_info filter_data, int job);
extern void start_trade_ads();
extern void menu_nav();
extern item_info item_query(std::string item_id, bool bundle);
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
extern void eval_test();

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
inline std::string eval_offer;
inline std::string eval_receive;
inline std::string eval_offer_robux;
inline std::string eval_receive_robux;
inline std::string output;

inline std::vector<menu_item> player_data = {
    {"COOKIES"}, {"[1] .ROBLOSECURITY=", [](){get_info(player_data[1], rblx_cookie);}, "[1] .ROBLOSECURITY=", &rblx_cookie}, {"[2] _RoliVerification=", [](){get_info(player_data[2], roli_cookie, "[2] _RoliVerification=");}, "[2] _RoliVerification=", &roli_cookie}, {"[3] Rolimons Player ID=", [](){get_info(player_data[3], roli_player_id);}, "[3] Rolimons Player ID=", &roli_player_id}
};

inline std::vector<menu_item> decline_filter_info = {
    {"FILTER INFO"}, {"[1] Baseline=", [](){get_info(decline_filter_info[1], decline_filter_data.baseline);}, "[1] Baseline=", &decline_filter_data.baseline}, {"[2] Cooldown=", [](){get_info(decline_filter_info[2], decline_filter_data.cooldown);}, "[2] Cooldown=", &decline_filter_data.cooldown}, {"[3] Enable", [](){filter_decline(decline_filter_info, 3, decline_filter_data);}}
};

inline std::vector<menu_item> accept_filter_info = {
    {"FILTER INFO"}, {"[1] Baseline=", [](){get_info(accept_filter_info[1], accept_filter_data.baseline);}, "[1] Baseline=", &accept_filter_data.baseline}, {"[2] Cooldown=", [](){get_info(accept_filter_info[2], accept_filter_data.cooldown);}, "[2] Cooldown=", &accept_filter_data.cooldown}, {"[3] Enable", [](){filter_accept(accept_filter_info, 3, accept_filter_data);}}
};

inline std::vector<menu_item> counter_filter_info;

inline std::vector<menu_item> trade_filter = {
    {"TRADE FILTER"}, {"[1] Auto-decline", [](){new_focus(decline_filter_info);}}, {"[2] Auto-accept", [](){new_focus(accept_filter_info);}}, {"[3] Auto-counter"}
};

inline std::vector<menu_item> eval = {
    {"TRADE EVAL"}, {"[1] Offer IDs=", [](){get_info(eval[1], eval_offer);}, "[1] Offer IDs=", &eval_offer}, {"[2] Receive IDs=", [](){get_info(eval[2], eval_receive);}, "[2] Receive IDs=", &eval_receive}, {"[3] Offer Robux=", [](){get_info(eval[3], eval_offer_robux);}, "[3] Offer Robux=", &eval_offer_robux}, {"[4] Receive Robux=", [](){get_info(eval[4], eval_receive_robux);}, "[4] Receive Robux=", &eval_receive_robux}, {"[5] Evaluate Trade", [](){eval_test();}}
};

inline std::vector<menu_item> saved_configs = {
    {"SAVED CONFIGS"}, {"[1] Save Config", [](){save_config();}}, {"[2] Load Config", [](){read_config();}}
};

inline std::vector<menu_item> config = {
    {"CONFIG"}, {"[1] Cookies", [](){new_focus(player_data);}}, {"[2] Trade Evaluation"}, {"[3] Saved Configs", [](){new_focus(saved_configs);}}
};

inline std::vector<menu_item> trade_ad_poster = {
    {"TRADE AD POSTER"}, {"[1] Offer=", [](){get_info(trade_ad_poster[1], poster_offer);}, "[1] Offer=", &poster_offer}, {"[2] Receive=", [](){get_info(trade_ad_poster[2], poster_receive);}, "[2] Receive=", &poster_receive}, {"[3] ReceiveTags=", [](){get_info(trade_ad_poster[3], poster_receive_tags);}, "[3] ReceiveTags=", &poster_receive_tags}, {"[4] Cooldown=", [](){get_info(trade_ad_poster[4], poster_cooldown);}, "[4] Cooldown=", &poster_cooldown}, {"[5] Enable", [](){ad_poster(trade_ad_poster, 5);}}
};

inline std::vector<menu_item> mass_trade_sender = {
    {"MASS TRADE SENDER"}, {"[1] Offer=", [](){get_info(mass_trade_sender[1], sender_offer);}, "[1] Offer=", &sender_offer}, {"[2] Receive=", [](){get_info(mass_trade_sender[2], sender_receive);}, "[2] Receive=", &sender_receive}, {"[3] OfferRobux=", [](){get_info(mass_trade_sender[3], sender_offer_robux);}, "[3] OfferRobux=", &sender_offer_robux}, {"[4] ReceiveRobux=", [](){get_info(mass_trade_sender[4], sender_receive_robux);}, "[4] ReceiveRobux=", &sender_receive_robux}, {"[5] Enable"}
};

inline std::vector<menu_item> trade_menu = {
    {"TRADE MENU"}, {"[1] Trade Sender", [](){new_focus(mass_trade_sender);}}, {"[2] Trade Eval", [](){new_focus(eval);}}
};

inline std::vector<menu_item> main_menu = {
    {"MENU"}, {"[1] Config", [](){new_focus(config);}}, {"[2] Trade Filter", [](){new_focus(trade_filter);}}, {"[3] Trade Ad Poster", [](){new_focus(trade_ad_poster);}}, {"[4] Mass Trade Sender", [](){new_focus(trade_menu);}}
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
    {"counter_cooldown", &counter_filter_data.cooldown},
    {"eval_offer", &eval_offer},
    {"eval_receive", &eval_receive},
    {"eval_offer_robux", &eval_offer_robux},
    {"eval_receive_robux", &eval_receive_robux}
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
    trade_menu,
    main_menu
};

#endif