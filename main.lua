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
            return float(value.text.replace(",", "").strip()) if value else None
    return -1

def get_stat(soup, name):
    for box in soup.select(".value-stat-box"):
        header = box.select_one(".value-stat-header")
        value = box.select_one(".value-stat-data")

        if header and header.text.strip() == name:
            return value.text.strip() if value else None

    for header in soup.select(".stat-header"):
        if header.text.strip() == name:
            value = header.find_next("h5", class_="stat-data") or header.find_next("div", class_="stat-data")
            return value.text.strip() if value else None

    return -1

def itemQuery(items, item):
    url = rolimons + items[item]
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36",
        "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
    }

    resp = requests.get(url, headers=headers, timeout=10)

    if resp.status_code != 200:
        raise RuntimeError(f"Failed to fetch item page: {resp.status_code}")

    soup = BeautifulSoup(resp.text, "html.parser")

    ADS = get_ADS(soup)
    RAP = int(get_stat(soup, "RAP").replace(",", ""))
    value = int(get_stat(soup, "Value").replace(",", ""))
    demand = demand_dict[get_stat(soup, "Demand")]
    trend = trend_dict[get_stat(soup, "Trend")]
    projected = 1 if "SimpleSVGId" in str(soup) else -1
    rare = 1 if "rare" in str(soup).lower() or "SimpleSVGId" in str(soup) else -1

    return ADS, RAP, value, demand, projected, rare, trend

def overpay(offer, receive):
    offerPay = 0
    receivePay = 0

    for item in range(len(offer)):
        ADS, RAP, value, demand, projected, rare, trend = itemQuery(offer, item)
        if projected == 1 and not value == -1:
            offerPay += value
        else:
            offerPay += RAP
    
    for item in range(len(receive)):
        ADS, RAP, value, demand, projected, rare, trend = itemQuery(offer, item)
        if projected == 1 and not value == -1:
            receivePay += value
        elif projected == 1:
            receivePay += -math.inf
        else:
            receivePay += RAP

    return [receivePay - offerPay, offerPay, receivePay]

def eval_offer(offer, item, offer_num, accept):
    ADS, RAP, value, demand, projected, rare, trend = itemQuery(offer, item)

    if trend == 0:
        accept += (0.15 * (1 - accept)) / offer_num
    elif trend == 1:
        accept += (0.05 * (1 - accept)) / offer_num
    elif trend == 2:
        accept -= (0.1 * (1 - accept)) / offer_num
    elif trend == 3:
        accept -= (0.25 * (1 - accept)) / offer_num
    
    if rare == 1:
        accept -= (0.15 * (1 - accept)) / offer_num

    #if hyped == 1:
        #accept -= (0.25 * (1 - accept)) / offer_num

    return accept

def eval_receive(receive, item, receive_num, accept):
    ADS, RAP, value, demand, projected, rare, trend = itemQuery(receive, item)

    if trend == 0:
        accept -= (0.15 * (1 - accept)) / receive_num
    elif trend == 1:
        accept -= (0.05 * (1 - accept)) / receive_num
    elif trend == 2:
        accept += (0.1 * (1 - accept)) / receive_num
    elif trend == 3:
        accept += (0.25 * (1 - accept)) / receive_num
    
    if rare == 1:
        accept += (0.15 * (1 - accept)) / receive_num

   #if hyped == 1:
        #accept += (0.25 * (1 - accept)) / receive_num

    return accept

def accept_trade(offer, receive):
    offer_num = len(offer)
    receive_num = len(receive)

    accept = 0

    is_overpay = overpay(offer, receive)

    if is_overpay[2] == -math.inf:
        return -math.inf

    if is_overpay[0] > 0:
        accept += is_overpay[2] / (is_overpay[1] + is_overpay[2])
    elif is_overpay[0] < 0:
        accept -= is_overpay[2] / (is_overpay[1] + is_overpay[2])

    for item in range(offer_num):
        accept = eval_offer(offer, item, offer_num, accept)

    for item in range(receive_num):
        accept = eval_receive(receive, item, receive_num, accept)

    return accept

items_to_give = ["10159600649"]
items_to_receive = ["16477149823"]

trade_status = accept_trade(items_to_give, items_to_receive)

if trade_status > 0:
    print("The bot is" , trade_status * 100 , "% sure this is a good trade.")
elif trade_status < 0:
    print("The bot is" , trade_status * 100 * -1 , "% sure this is a bad trade.")
else:
    print("This is an even trade.")
