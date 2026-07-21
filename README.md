Updated version of original python-written RoTrade, now with new features and written in C++.

-- Version 3.0

## FEATURES

<ins>Automated Trade Ad Poster:</ins>
- Automatically sends trades on ``Rolimons.com`` at user configured intervals.
- For standalone use or instructions, please see --> ``https://github.com/LegionAC/Automated-Rolimons-Trade-Ad-Creator``
<ins>Trade Filter:</ins>
- Tired of lowball trades? Tired of people thinking you're stupid enough to get proj flipped? If the standard Roblox trade filter isn't cutting it, you can use the trade filter utility.
- The RoTrade trade filter automatically declines trades deemed inconsiderable through the user configured trade evaluation.
- The user chooses a baseline value (such as 0.2, 0.0 is a theoretical even trade) , if the trade falls short of that value in the trade eval, the trade is automatically declined.
- The trade is also declined if the other user is offering projected items.
- If trades do pass the trade eval, they are not accepted automatically. The tool is simply made to streamline the process of looking through trades, and ignoring trades which aren't worth considering.

<ins>BELOW IS AN EXAMPLE INPUT TO THE TRADE FILTER</ins>

<img width="778" height="179" alt="image" src="https://github.com/user-attachments/assets/15360ca9-6196-4e3f-a564-5d18295dcbcc" />

<ins>Trade Evaluation:</ins>
- RoTrade uses a pre-configured evaluation algorithm to determine trade quality.
- Trade eval is used on other, and planned future features. 

<ins>Command Line Usage:</ins>
- The command line holds a variety of different commands, such as enabling trade ads, the trade filter, or querying/disabling enabled utilities.

<ins>BELOW IS A LIST OF ALL POSSIBLE COMMANDS</ins>
<img width="831" height="207" alt="image" src="https://github.com/user-attachments/assets/79a19917-20a4-49a0-b79e-36e2b80ac211" />

<ins>PLANNED FEATURES:</ins>
- Auto counter trade filter add-on.
- Auto accept trade filter add-on.
- Mass trade sender.
- Automated mass trade sending.
- Rolimons deals page monitor. (Purchases item if deal meets user configured requirements)