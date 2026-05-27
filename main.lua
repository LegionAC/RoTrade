import playwright
import math
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

    is_overpay = overpay(offerInfo, receiveInfo)

    if is_overpay[2] == -math.inf:
        return -math.inf

    if offer_num > receive_num and (is_overpay[0] >= (0.15 * is_overpay[1])):
        accept -= ((is_overpay[1] - is_overpay[0]) / is_overpay[0])
    elif offer_num > receive_num and (is_overpay[0] < (0.15 * is_overpay[1])):
        accept += is_overpay[0] / is_overpay[1]
    elif receive_num > offer_num and (is_overpay[0] > (0.15 * is_overpay[1])):
        accept += ((is_overpay[1] - is_overpay[0]) / is_overpay[0])
    elif receive_num > offer_num and (is_overpay[0] < (0.15 * is_overpay[1])):
        accept += is_overpay[0] / is_overpay[1]
    elif receive_num == offer_num:
        accept += is_overpay[0] / is_overpay[1] * 2


    for item in range(offer_num):
        accept = eval_offer(offerInfo[item], offer_num, accept)

    for item in range(receive_num):
        accept = eval_receive(receiveInfo[item], receive_num, accept)

    return accept

items_to_give = ["1235488"]
items_to_receive = ["110673146052704", "1191152570", "293318274", "1029025"]

trade_status = accept_trade(items_to_give, items_to_receive)

if trade_status > 0:
    print("The bot is" , trade_status * 100 , "% sure this is a good trade.")
elif trade_status < 0:
    print("The bot is" , trade_status * 100 * -1 , "% sure this is a bad trade.")
else:
    print("This is an even trade.")

calculate_proofs(items_to_give, items_to_receive, "4255053867")
