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
        existing = data[proof_item]
        if existing == 0 or item_overpay == 0 or (existing > 0) == (item_overpay > 0):
            item_overpay = (item_overpay + existing) / 2
        else:
            item_overpay = item_overpay

    data[proof_item] = math.ceil(item_overpay)

    with open("proofs.json", "w") as f:
        json.dump(data, f, indent=2)

def calculate_proofs(offer, receive):
    offerInfo, receiveInfo = generate_info(offer, receive)
    total_overpay = overpay(offerInfo, receiveInfo)[0]

    if total_overpay == 0:
        return

    side_items = receive
    side_name = "receive"
    total_overpay = abs(total_overpay)

    side_values = 0
    item_values = {}

    for item in side_items:
        item_data = item_query(item)
        item_value = item_data["Value"] if item_data["Value"] != -1 else item_data["RAP"]
        item_values[item] = item_value
        side_values += item_value

    if side_values == 0:
        return

    for item in side_items:
        item_overpay = total_overpay * (item_values[item] / side_values)
        append_proof(item_overpay, item)
