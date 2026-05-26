import playwright
import requests
import math
from bs4 import BeautifulSoup

HEADERS = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36",
    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
}

rolimons_item_details = requests.get("https://www.rolimons.com/itemapi/itemdetails", headers=HEADERS, timeout=10)
rolimons = "https://www.rolimons.com/item/"

query = rolimons_item_details.json()

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

# needs to reward value more rather than reward RAP, RAP should be rewarded though less than value.
# should be able to calculate demand itself if need be.
# should be able to, if given data, know what trades to shy away from depending on current market trends e.g faces not doing well.
# possibly make a rap limit (not that trade values with the bot have gotten that high yet) to stop the bot from being able to make catastrophic errors  
# update readability, name attributes rather than just using the numbers

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

def item_query(item):
    url = rolimons + item
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
    }

    resp = requests.get(url, headers=headers, timeout=10)

    if resp.status_code != 200:
        raise RuntimeError(f"Failed to fetch item page: {resp.status_code}")

    soup = BeautifulSoup(resp.text, "html.parser")

    ads = get_ADS(soup)
    rap = int(get_stat(soup, "RAP").replace(",", ""))
    value = int(get_stat(soup, "Value").replace(",", ""))
    demand = demand_dict.get(get_stat(soup, "Demand"), -1)
    trend = trend_dict.get(get_stat(soup, "Trend"), -1)
    api_entry = query.get("items", {}).get(item)
    if api_entry:
        hyped = 1 if len(api_entry) > 8 and api_entry[8] == 1 else -1
        rare = 1 if len(api_entry) > 9 and api_entry[9] == 1 else -1
        projected = 1 if len(api_entry) > 7 and api_entry[7] == 1 else -1
    else:
        projected = 1 if "SimpleSVGId" in str(soup) else -1
        hyped = -1
        rare = -1

    return {"ADS" : ads, "RAP" : rap, "Value" : value, "Demand" : demand, "Projected" : projected, "Rare" : rare, "Trend" : trend, "Hyped": hyped}

def generate_info(offer, receive):
    offerInfo = {}
    receiveInfo = {}

    for item in range(len(offer)):
       data = item_query(offer[item])
       offerInfo[item] = data

    for item in range(len(receive)):
       data = item_query(receive[item])
       receiveInfo[item] = data

    return offerInfo, receiveInfo

def overpay(offerInfo, receiveInfo):
    offerPay = 0
    receivePay = 0

    for item in range(len(offerInfo)):
        currentInfo = offerInfo[item]
        if currentInfo["Projected"] == 1 and not currentInfo["Value"] == -1:
            offerPay += currentInfo["Value"]
        elif currentInfo["Value"] != -1:
            offerPay += currentInfo["Value"]
        else:
            offerPay += currentInfo["RAP"]
    
    for item in range(len(receiveInfo)):
        currentInfo = receiveInfo[item]
        if currentInfo["Projected"] == 1 and not currentInfo["Value"] == -1:
            receivePay += currentInfo["Value"]
        elif currentInfo["Projected"] == 1:
            receivePay += -math.inf
        elif currentInfo["Value"] != -1:
            receivePay += currentInfo["Value"]
        else:
            receivePay += currentInfo["RAP"]

    return [receivePay - offerPay, offerPay, receivePay]

def eval_offer(item, offer_num, accept):
    accept = 0

    if item["Trend"] == 0:
        accept += 0.15 / offer_num
    elif item["Trend"] == 1:
        accept += 0.05 / offer_num
    elif item["Trend"] == 2:
        accept -= 0.1/ offer_num
    elif item["Trend"] == 3:
        accept -= 0.25 / offer_num
    
    if item["Rare"] == 1:
        accept -= 0.15 / offer_num

    if item["Demand"] == 0:
        accept += 0.25
    elif item["Demand"] == 1:
        accept += 0.15
    elif item["Demand"] == 3:
        accept -= 0.15
    elif item["Demand"] == 4:
        accept -= 0.25

    #if hyped == 1:
        #accept -= (0.25 * (1 - accept)) / offer_num

    return accept

def eval_receive(item, receive_num, accept):
    if item["Trend"] == 0:
        accept -= 0.15 / receive_num
    elif item["Trend"] == 1:
        accept -= 0.05 / receive_num
    elif item["Trend"] == 2:
        accept += 0.1 / receive_num
    elif item["Trend"] == 3:
        accept += 0.25 / receive_num
    
    if item["Rare"] == 1:
        accept += 0.15 / receive_num

    if item["Demand"] == 0:
        accept -= 0.25
    elif item["Demand"] == 1:
        accept -= 0.15
    elif item["Demand"] == 3:
        accept += 0.15
    elif item["Demand"] == 4:
        accept += 0.25

   #if hyped == 1:
        #accept += (0.25 * (1 - accept)) / receive_num

    return accept

def accept_trade(offer, receive):
    offer_num = len(offer)
    receive_num = len(receive)

    accept = 0

    offerInfo, receiveInfo = generate_info(offer, receive)

    is_overpay = overpay(offerInfo, receiveInfo)

    if is_overpay[2] == -math.inf:
        return -math.inf

    if is_overpay[0] > 0:
        accept += is_overpay[2] / (is_overpay[1] + is_overpay[2])
    elif is_overpay[0] < 0:
        accept -= is_overpay[1] / (is_overpay[1] + is_overpay[2])

    if offer_num > receive_num and (is_overpay[0] >= (0.15 * is_overpay[1])):
        accept += 0.15
    elif receive_num > offer_num and (is_overpay[0] >= (0.15 * is_overpay[1])):
        accept -= 0.15

    for item in range(offer_num):
        accept = eval_offer(offerInfo[item], offer_num, accept)

    for item in range(receive_num):
        accept = eval_receive(receiveInfo[item], receive_num, accept)

    return accept

items_to_give = ["96103379"]
items_to_receive = ["439945661", "1029025", "4390891467", "1365767"]

trade_status = accept_trade(items_to_give, items_to_receive)

if trade_status > 0:
    print("The bot is" , trade_status * 100 , "% sure this is a good trade.")
elif trade_status < 0:
    print("The bot is" , trade_status * 100 * -1 , "% sure this is a bad trade.")
else:
    print("This is an even trade.")
