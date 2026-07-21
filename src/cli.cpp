#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "feature_utils.h"
#include <thread>
#include <mutex>
#include <cctype>

std::vector<std::string> cmd_line = {"/help", "/disable-all", "/auto-ads", "/disable-ads", "/trade-filter", "/disable-filter", "/utils-running", "/trade-eval"}; // /auto-trade is a planned feature.

std::mutex mutex;

void help_cmd() {
    std::cout << "cmd line: \n" << "/help: prints list of valid commands.\n";
    std::cout << "/disable-all disables all currently running RoTrade processes.\n";
    std::cout << "/auto-ads: sends trades automatically on Rolimons.\n";
    std::cout << "/disable-ads: disables automated ads.\n";
    std::cout << "/trade-filter: automatically declines trades which are low quality or involve projected items.\n";
    std::cout << "/disable-filter: disables trade filter.\n";
    std::cout << "/utils-running: returns all running utilities.\n";
    std::cout << "/trade-eval: returns a decimal score of a given trade.\n\n";
}

std::vector<std::string> vector_csv_parse(std::string input) {
    std::vector<std::string> container;
    std::string buffer;

    input.erase(std::remove(input.begin(), input.end(), ' '), input.end());

    for (int i{0}; i < input.size(); i++) {
        if (!std::isdigit(input[i]) && input[i] != ',') continue;
        char v = input[i];
        if (v != ',') buffer.push_back(v);
        if (v == ',' || i == input.size() - 1) { 
            container.push_back(buffer);
            buffer.clear();
        }
    }

    return container;
}

void trade_eval() {
    std::string offer_input = query_interface("\nEnter your offer (csv): ");
    std::string receive_input = query_interface("\nEnter your recipient offer (csv): ");
    std::string offer_robux_input = query_interface("\nEnter your offered robux: ");
    std::string receive_robux_input = query_interface("\nEnter your recipient robux: ");

    int offer_robux = std::stoi(offer_robux_input);
    int receive_robux = std::stoi(receive_robux_input);

    std::vector<std::string> offer = vector_csv_parse(offer_input);
    std::vector<std::string> receive = vector_csv_parse(receive_input);

    double eval = eval_trade(offer, receive, offer_robux, receive_robux);

    std::cout << "Trade eval: " << eval;
}

void utils_running() {
    bool util_printed;

    if (!switch_list.ad_switch && !switch_list.filter_switch) {
        std::cout << "nothing running.";
    }

    if (switch_list.ad_switch) {
        std::cout << "auto-ads running";
        util_printed = true;
    }

    if (switch_list.filter_switch) {
        if (util_printed) std::cout << ", ";
        std::cout << "trade-filter running";
        util_printed = true;
    }

    std::cout << "\n\n";
}

int cmd_search(std::string user_input) {
    auto res = find(cmd_line.begin(), cmd_line.end(), user_input);
    
    if (res == cmd_line.end()) {
        return -1;
    } else if (user_input == "/help") {
        help_cmd();
    } else if (user_input == "/auto-ads") {
        start_trade_ads();
    } else if (user_input == "/trade-filter") {
        filter_trades();
    } else if (user_input == "/disable-all") {
        switch_list.ad_switch = false;
        switch_list.filter_switch = false;
        std::cout << "all processes disabled.\n\n";
    } else if (user_input == "/disable-ads") {
        switch_list.ad_switch = false;

        std::cout << "trade ads disabled.\n\n";
    } else if (user_input == "/disable-filter") {
        switch_list.filter_switch = false;

        std::cout << "filter disabled.\n\n";
    } else if (user_input == "/utils-running") {
        utils_running();
    } else if (user_input == "/trade-eval") {
        trade_eval();
    }

    return 0;
}

void ping_cmd_line(std::string msg) {
    std::cout << "\n\n" << msg << "\n\n";

    mutex.lock();
    std::cout << str_buffer << std::flush;
    mutex.unlock();
}

std::string query_interface(std::string msg) {
    cmd_wait = true;

    mutex.lock();
    str_buffer = msg;
    mutex.unlock();

    std::string str;

    std::cout << msg;

    std::getline(std::cin, str);

    mutex.lock();
    str_buffer = "enter a cmd: ";
    mutex.unlock();

    cmd_wait = false;

    return str;
}

void cli() {
    std::string user_input = query_interface("enter a cmd: ");

    int res = cmd_search(user_input);

    if (res != 0) {
        std::cout << "\ncmd not found.\n\n";
    } else {
        std::cout << "\n\ncmd success.\n\n";
    }

    cli();
}