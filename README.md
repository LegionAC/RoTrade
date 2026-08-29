Updated version of original python-written RoTrade, now with new features and written in C++.

-- Version 4.0

## FEATURES

<ins>**Automated Trade Ad Poster:**</ins>
- Automatically sends trades on ``Rolimons.com`` at user configured intervals.
- Configure your trade ad in ``MENU --> TRADE AD POSTER``

<ins>**Trade Filter:**</ins>
- Tired of lowball trades? Tired of people thinking you're stupid enough to get proj flipped? If the standard Roblox trade filter isn't cutting it, you can use the trade filter utility.

- There are three types of trade filters, auto-decline, auto-accept, and auto-counter.

- For each trade filter, the user may establish a baseline value in which said filter wil hinge its decision upon. 0.0 is a theoretical even trade.

<ins>Auto-Decline:</ins>
- If the trade evaluation falls below the baseline, the trade is automatically declined.
- If the evaluation meets the baseline requirement, the trade is ignored and left for the user to accept themselves.
- Useful to ignore poor trades which slip through the standard Roblox trade filter, which uses a flat RAP calculation to decline trades.

<ins>Auto-Accept:</ins>
- If the trade evaluation meets the baseline requirement, the trade is automatically accepted.
- Trade evaluations which fall below the baseline requirement are ignored.
- Useful if the user wants an entirely RoTrade dependent experience.

<ins>Auto-Counter:</ins>
- If the trade evaluation falls below the baseline requirement, the bot attempts to counter with a trade that meets the baseline or above.
- Trades which meet the baseline are ignored.
- Possibly more profitable than the passive method of sending trade ads and solely accept-filtering trades, as it sends out high quality trades.

<ins>**Trade Evaluation:**</ins>
- RoTrade uses a pre-configured evaluation algorithm to determine trade quality.
- Test the trade algorithm in ``MENU --> MASS TRADE SENDER --> TRADE EVAL``

<ins>MENU PREVIEW</ins>

<img width="890" height="337" alt="image" src="https://github.com/user-attachments/assets/eae251e4-ae3a-4f63-b5fa-610e972e5007" />


<ins>PLANNED FEATURES:</ins>
- Auto counter trade filter add-on.
- Mass trade sender.
- Automated mass trade sending.
- Rolimons deals page monitor. (Purchases item if deal meets user configured requirements)
- Trade evaluation restrictions (The trade evaluation algorithm will take restrictions set by you into consideration. A default trade restriction is that faces require at least 20% overpay to be considered viable)
