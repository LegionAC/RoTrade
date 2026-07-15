#ifndef feature_utils_H
#define feature_utils_H
#include <vector>
#include <string>

struct item_info {
    double ADS;
    int best_price;
};

extern double eval_trade(std::vector<std::string> offer_ids, std::vector<std::string> receive_ids, int offer_robux, int receive_robux);
extern void filter_trades();
extern void start_trade_ads();
extern void cli();
extern void ping_cmd_line(std::string msg);
extern std::string query_interface(std::string msg);
extern item_info item_query(std::string item_id);

inline bool global_proc_switch = true;
inline bool ad_switch = false;
inline bool filter_switch = false;
inline bool cmd_wait;
inline std::string str_buffer;

#endif

#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif