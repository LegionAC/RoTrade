import playwright
import requests
import math

rolimons = requests.get("https://www.rolimons.com/itemapi/itemdetails")

query = rolimons.json()

passrate = 0.5

# needs to reward value more rather than reward RAP, RAP should be rewarded though less than value.
# should be able to calculate demand itself if need be.
# should be able to, if given data, know what trades to shy away from depending on current market trends e.g faces not doing well.
# possibly make a rap limit (not that trade values with the bot have gotten that high yet) to stop the bot from being able to make catastrophic errors  
# update readability, name attributes rather than just using the numbers

def overpay(offer, receive):
    offerPay = 0
    receivePay = 0

    for item in range(len(offer)):
        limited = query["items"][offer[item]]
        if limited[7] == 1 and not limited[3] == -1:
            offerPay += limited[3]
        else:
            offerPay += limited[2]
    
    for item in range(len(receive)):
        limited = query["items"][receive[item]]
        if limited[7] == 1 and not limited[3] == -1:
            receivePay += limited[3]
        elif limited[7] == 1:
            receivePay += -math.inf
        else:
            receivePay += limited[2]

    return [receivePay - offerPay, offerPay, receivePay]

def eval_offer(offer, receive, item, offer_num, accept):
    limited = query["items"][offer[item]]
    RAP = limited[2]
    value = limited[3]
    demand = limited[5]
    trend = limited[4]
    hyped = limited[8]
    rare = limited[9]

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

    if hyped == 1:
        accept -= (0.25 * (1 - accept)) / offer_num

    return accept

def eval_receive(offer, receive, item, receive_num, accept):
    limited = query["items"][receive[item]]
    RAP = limited[2]
    value = limited[3]
    demand = limited[5]
    trend = limited[4]
    hyped = limited[8]
    rare = limited[9]

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

    if hyped == 1:
        accept += (0.25 * (1 - accept)) / receive_num

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
        accept = eval_offer(offer, receive, item, offer_num, accept)

    for item in range(receive_num):
        accept = eval_receive(offer,receive, item, receive_num, accept)

    return accept

items_to_give = ["2566018860"]
items_to_receive = ["878904755", "71597060", "158068446", "55917432"]

trade_status = accept_trade(items_to_give, items_to_receive)

if trade_status > 0:
    print("The bot is" , trade_status * 100 , "% sure this is a good trade.")
elif trade_status < 0:
    print("The bot is" , trade_status * 100 * -1 , "% sure this is a bad trade.")
else:
    print("This is an even trade.")
