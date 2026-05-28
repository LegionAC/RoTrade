import playwright
import math
import os
import json
from proofs import calculate_proofs
from trade_utils import item_query, generate_info, overpay

passrate = 0.5

demand_dict = {
    "None" : -1,
    "Terrible" : 0,
    "Low" : 1,
    "Normal" : 2,
    "High" : 3,
    "Amazing" : 4 
}

trend_dict = {
    "None" : -1,
    "Lowering" : 0,
    "Unstable" : 1,
    "Stable" : 2,
    "Raising" : 3,
    "Fluctuating" : 4
}

custom_demand_map = {
    "20573078" : 4
}

# needs to reward value more rather than reward RAP, RAP should be rewarded though less than value.
# should be able to calculate demand itself if need be.
# should be able to, if given data, know what trades to shy away from depending on current market trends e.g faces not doing well.
# possibly make a rap limit (not that trade values with the bot have gotten that high yet) to stop the bot from being able to make catastrophic errors  
# update readability, name attributes rather than just using the numbers
# make it possible to determine demand if not assigned
# submit trades through use of the roblox api
# make it so it's possible to feed proofs to the bot to determine what items fetch what overpay and compile an accurate evaluation
# only get overpay for proofs

def get_ADS(soup):
    for card in soup.select(".card"):
        title = card.select_one(".card-title")
        value = card.select_one("h5.stat-data")

        if title and "ADS" in title.text:
            return float(value.text.replace(",", "").strip()) if value else "-1"
    return "-1"

def get_stat(soup, name):
    for box in soup.select(".value-stat-box"):
        header = box.select_one(".value-stat-header")
        value = box.select_one(".value-stat-data")

        if header and header.text.strip() == name and not value.text.strip() == "Not Assigned":
            return value.text.strip() if value else "-1"

    for header in soup.select(".stat-header"):
        if header.text.strip() == name and not value.text.strip() == "Not Assigned":
            value = header.find_next("h5", class_="stat-data") or header.find_next("div", class_="stat-data")
            return value.text.strip() if value else "-1"

    return "-1"

def eval_offer(item, offer_num, accept):
    if item["Trend"] == 0:
        accept += (accept * 0.15) / offer_num
    elif item["Trend"] == 1:
        accept += (accept * 0.05) / offer_num
    elif item["Trend"] == 2:
        accept -= (accept * 0.1) / offer_num
    elif item["Trend"] == 3:
        accept -= (accept * 0.35) / offer_num
    
    if item["Rare"] == 1:
        accept -= (accept * 0.15) / offer_num

    if item["Demand"] == 0:
        accept += (accept * 0.25) / offer_num
    elif item["Demand"] == 1:
        accept += (accept * 0.15) / offer_num
    elif item["Demand"] == 3:
        accept -= (accept * 0.15) / offer_num
    elif item["Demand"] == 4:
        accept -= (accept * 0.35) / offer_num

    return accept

def eval_receive(item, receive_num, accept):
    if item["Trend"] == 0:
        accept -= (accept * 0.15) / receive_num
    elif item["Trend"] == 1:
        accept -= (accept * 0.05) / receive_num
    elif item["Trend"] == 2:
        accept += (accept * 0.1) / receive_num
    elif item["Trend"] == 3:
        accept += (accept * 0.35) / receive_num
    
    if item["Rare"] == 1:
        accept += (accept * 0.15) / receive_num

    if item["Demand"] == 0:
        accept -= (accept * 0.25) / receive_num
    elif item["Demand"] == 1:
        accept -= (accept * 0.15) / receive_num
    elif item["Demand"] == 3:
        accept += (accept * 0.15) / receive_num
    elif item["Demand"] == 4:
        accept += (accept * 0.35) / receive_num

    return accept

def accept_trade(offer, receive):
    offer_num = len(offer)
    receive_num = len(receive)

    accept = 0

    offerInfo, receiveInfo = generate_info(offer, receive)

    proofs_data = {}
    if os.path.exists("proofs.json"):
        with open("proofs.json", "r") as f:
            proofs_data = json.load(f)

    is_overpay = overpay(offerInfo, receiveInfo)

    offer_proof_bonus = sum(proofs_data.get(item_id, 0) for item_id in offer)
    receive_proof_bonus = sum(proofs_data.get(item_id, 0) for item_id in receive)
    is_overpay = [
        is_overpay[0] + receive_proof_bonus - offer_proof_bonus,
        is_overpay[1] + offer_proof_bonus,
        is_overpay[2] + receive_proof_bonus,
    ]

    if is_overpay[2] == -math.inf:
        return -math.inf

    def get_overpay_threshold(items_list):
        weighted_threshold = 0
        total_value = 0
        for item_id in items_list:
            item_data = item_query(item_id)
            item_value = item_data["Value"] if item_data["Value"] != -1 else item_data["RAP"]
            if item_id in proofs_data:
                item_threshold = abs(proofs_data[item_id]) / item_value
            else:
                item_threshold = 0.15
            weighted_threshold += item_threshold * item_value
            total_value += item_value
        return weighted_threshold / total_value if total_value > 0 else 0.15

    if is_overpay[0] >= 0:
        threshold = get_overpay_threshold(receive)
    else:
        threshold = get_overpay_threshold(offer)

    if offer_num >= receive_num:
        base_value = is_overpay[1]
    else:
        base_value = is_overpay[2]

    expected_overpay = threshold * base_value

    def overpay_score(actual, expected):
        expected = max(expected, 1)
        delta = actual - expected
        ratio = delta / expected
        return math.copysign(abs(ratio) ** 2, ratio)

    accept += overpay_score(is_overpay[0], expected_overpay)

    for item in range(offer_num):
        accept = eval_offer(offerInfo[item], offer_num, accept)

    for item in range(receive_num):
        accept = eval_receive(receiveInfo[item], receive_num, accept)

    return max(min(accept, 1), -1)

items_to_give = ["102605392", "7636350", "63253701", "3798239844"]
items_to_receive = ["14463095", "4390891467", "19027209"]

trade_status = accept_trade(items_to_give, items_to_receive)

if trade_status > 0:
    print("The bot is" , trade_status * 100 , "% sure this is a good trade.")
elif trade_status < 0:
    print("The bot is" , trade_status * 100 * -1 , "% sure this is a bad trade.")
else:
    print("This is an even trade.")

#calculate_proofs(items_to_give, items_to_receive)
