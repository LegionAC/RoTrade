import requests
import math
from bs4 import BeautifulSoup

HEADERS = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36",
    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
    "Accept-Language": "en-US,en;q=0.5",
}

rolimons = "https://www.rolimons.com/item/"

passrate = 0.5

demand_dict = {
    "None": -1,
    "Terrible": 0,
    "Low": 1,
    "Normal": 2,
    "High": 3,
    "Amazing": 4,
}

trend_dict = {
    "None": -1,
    "Lowering": 0,
    "Unstable": 1,
    "Stable": 2,
    "Raising": 3,
    "Fluctuating": 4,
}

custom_demand_map = {
    "20573078": 4,
}


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
        value = header.find_next("h5", class_="stat-data") or header.find_next("div", class_="stat-data")
        if header.text.strip() == name and value and not value.text.strip() == "Not Assigned":
            return value.text.strip()

    return "-1"


def item_query(item):
    url = rolimons + item
    resp = requests.get(url, headers=HEADERS, timeout=10)

    if resp.status_code != 200:
        raise RuntimeError(f"Failed to fetch item page: {resp.status_code}")

    soup = BeautifulSoup(resp.text, "html.parser")

    ads = get_ADS(soup)
    rap_raw = get_stat(soup, "RAP")
    value_raw = get_stat(soup, "Value")
    rap = int(rap_raw.replace(",", "")) if rap_raw != "-1" else -1
    value = int(value_raw.replace(",", "")) if value_raw != "-1" else -1
    demand = demand_dict.get(get_stat(soup, "Demand"), -1)
    trend = trend_dict.get(get_stat(soup, "Trend"), -1)

    if item in custom_demand_map:
        demand = custom_demand_map[item]

    api_entry = query.get("items", {}).get(item)
    if api_entry:
        projected = 1 if len(api_entry) > 7 and api_entry[7] == 1 else -1
        hyped = 1 if len(api_entry) > 8 and api_entry[8] == 1 else -1
        rare = 1 if len(api_entry) > 9 and api_entry[9] == 1 else -1
    else:
        projected = 1 if "SimpleSVGId" in str(soup) else -1
        hyped = -1
        rare = -1

    return {
        "ADS": ads,
        "RAP": rap,
        "Value": value,
        "Demand": demand,
        "Projected": projected,
        "Rare": rare,
        "Trend": trend,
        "Hyped": hyped,
    }

def generate_info(offer, receive):
    offerInfo = {}
    receiveInfo = {}

    for index, item in enumerate(offer):
        offerInfo[index] = item_query(item)

    for index, item in enumerate(receive):
        receiveInfo[index] = item_query(item)

    return offerInfo, receiveInfo


def overpay(offerInfo, receiveInfo):
    offerPay = 0
    receivePay = 0

    for item in range(len(offerInfo)):
        currentInfo = offerInfo[item]
        if currentInfo["Projected"] == 1 and currentInfo["Value"] != -1:
            offerPay += currentInfo["Value"]
        elif currentInfo["Value"] != -1:
            offerPay += currentInfo["Value"]
        else:
            offerPay += currentInfo["RAP"]

    for item in range(len(receiveInfo)):
        currentInfo = receiveInfo[item]
        if currentInfo["Projected"] == 1 and currentInfo["Value"] != -1:
            receivePay += currentInfo["Value"]
        elif currentInfo["Projected"] == 1:
            receivePay += -math.inf
        elif currentInfo["Value"] != -1:
            receivePay += currentInfo["Value"]
        else:
            receivePay += currentInfo["RAP"]

    return [receivePay - offerPay, offerPay, receivePay]
