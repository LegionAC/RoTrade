#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "feature_utils.h"
#include <thread>
#include <cctype>
#include <termios.h>
#include <functional>
#include <sys/ioctl.h>
#include <algorithm>
#include <fstream>

/*
    to do list:
    make menu
    make like trade counter
    big mass trade sender
    good

    remove racist command line
    wait i should push changes first brb
*/

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

std::vector<menu_item>* focus;
int menu_index = 1;

std::vector<std::vector<menu_item>*> queue;
std::vector<int> queue_index;

std::string build_str;

void get_info(menu_item& item, std::string& value, std::string prefix) {
    disable_raw_mode();
    std::cout << "Input your value here: " << std::flush;
    std::cout << output << std::flush;
    std::string input;
    std::getline(std::cin, input);
    item.text = item.orig_text + input;
    value = prefix + input;
    output = "Written successfully";
    enable_raw_mode();
}

void new_focus(std::vector<menu_item>& item) {
    queue.push_back(focus);
    queue_index.push_back(menu_index);
    menu_index = 1;
    focus = &item;
}

void filter_decline(std::vector<menu_item>& item, int target, filter_info filter_data) {
    if (switch_list.filter_decline) {
        switch_list.filter_decline = false;
        trade_filter[1].text = "[1] Auto-decline";
        item[target].text = "Enable";
    } else {
        switch_list.filter_decline = true;
        trade_filter[1].text = "[1] Auto-decline [RUNNING]";
        filter_trades(filter_data, 0);
        item[target].text = "Disable";
    }
}

void ad_poster(std::vector<menu_item>& item, int target) {
    if (switch_list.ad_switch) {
        switch_list.ad_switch = false;
        main_menu[4].text = "[3] Trade Ad Poster";
        item[target].text = "Enable";
    } else {
        switch_list.ad_switch = true;
        start_trade_ads();
        item[target].text = "Disable";
        main_menu[4].text = "[3] Trade Ad Poster [RUNNING]";
    }
}

void filter_accept(std::vector<menu_item>& item, int target, filter_info filter_data) {
    if (switch_list.filter_accept) {
        switch_list.filter_accept = false;
        trade_filter[2].text = "[2] Auto-accept";
        item[target].text = "Enable";
    } else {
        switch_list.filter_accept = true;
        trade_filter[2].text = "[2] Auto-accept [RUNNING]";
        filter_trades(filter_data, 1);
        item[target].text = "Disable";
    }
}

void append_output(std::string msg) {
    output = output + msg + "\n";
}

void save_config() {
    std::ofstream config("config.txt");
    std::string str;

    for (auto v : config_ptrs) {
        str += v.first + "=" + *v.second + "\n";
    }

    config << str;

    output = "Config saved.";
}

void read_config() {
    std::ifstream config("config.txt");
    std::string str;
    int index = 0;

    while(std::getline (config, str)) {
        size_t pos = str.find('=');
        if (pos != std::string::npos){
            str = str.substr(pos + 1);
        } else {
            str = "";
        }
        *config_ptrs[index].second = str;
        index += 1;
    }

    for (auto& v : menu_objs) {
        auto& item = v.get();
        for (auto& v : item) {
            if (!v.variable || v.variable->empty()) continue;
            if (v.orig_text == "_RoliVerification=") {
                v.text = *v.variable;
                continue;
            }
            v.text = v.orig_text + *v.variable;
        }
    }

    output = "Config loaded successfully.";
}

void draw() {
    std::cout << "\033[2J\033[H";

    for (int i{0}; i < focus->size(); i++) {
        std::string text = (*focus)[i].text;
        if (i != menu_index) {
            std::cout << text << "\n";
        } else {
            std::cout << "\033[7m" << text << "\033[0m\n";
        }
    }

    std::cout << "\n" << output << std::flush;
}

char get_key() {
    char input;
    read(STDIN_FILENO, &input, 1);
    if (input == '\033') {
        int bytes;
        ioctl(STDIN_FILENO, FIONREAD, &bytes);

        if (bytes >= 2) {
            char seq[2];
            read(STDIN_FILENO, &seq[0], 1);
            read(STDIN_FILENO, &seq[1], 1);
            if (seq[0] == '[') {
                return seq[1];
            }
        }

        return '\033';
    }

    return input;
}

void menu_nav() {
    focus = &main_menu;
    while (true) {
        draw();

        char input = get_key();
        if (input == 'A') {
            if (menu_index != 1) menu_index -= 1;
            output = "";
        } else if (input == 'B') {
            if (menu_index != focus->size() - 1) menu_index += 1;
            output = "";
        } else if (input == '\r' || input == '\n') {
            output = "";
            draw();
            if (!(*focus)[menu_index].action) {
                output = "Feature is a work in progress, not available currently.";
                continue;
            }
            (*focus)[menu_index].action();
        } else if (input == '\033') {
            output = "";
            if (!queue.empty()) {
                focus = queue[queue.size() - 1];
                menu_index = queue_index[queue_index.size() - 1];
                queue.erase(queue.begin() + (queue.size() - 1));
                queue_index.erase(queue_index.begin() + (queue_index.size() - 1));
            }
        }
    }
}