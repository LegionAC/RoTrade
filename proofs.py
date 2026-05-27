import math
from trade_utils import generate_info, overpay, item_query
import json
import os

def append_proof(item_overpay, proof_item):
    if os.path.exists("proofs.json"):
        with open("proofs.json", "r") as f:
            data = json.load(f)
    else:
        data = {}

    if proof_item in data:
        item_overpay = (item_overpay + data[proof_item]) / 2

    data[proof_item] = math.ceil(item_overpay)

    with open("proofs.json", "w") as f:
        json.dump(data, f, indent=2)

def calculate_proofs(offer, receive):
    offerInfo, receiveInfo = generate_info(offer, receive)
    total_overpay = overpay(offerInfo, receiveInfo)[0]

    if total_overpay == 0:
        raise ValueError("Total overpay is zero; cannot compute proof ratios")

    all_items = list(offer) + list(receive)
    total_values = 0
    item_values = {}

    for item in all_items:
        item_data = item_query(item)
        item_value = item_data["Value"] if item_data["Value"] != -1 else item_data["RAP"]
        item_values[item] = item_value
        total_values += item_value

    if total_values == 0:
        raise ValueError("Total item value is zero; cannot compute weighted proof ratios")

    for item in all_items:
        item_overpay = total_overpay * (item_values[item] / total_values)
        append_proof(item_overpay, item)
