# MosquittoESP
wss mqtt
wrapping together the excelent work of 
- Nick O'Leary (PubSubClient): https://github.com/knolleary
- Markus Sattler (arduinoWebSockets wss): https://github.com/Links2004/arduinoWebSockets

I had to modify both as they implemented both 'loop'...

I started to add a Client interface to arduinoWebSockets and did some rewiring of PubSubClient to take into account the asynchronous aspect of sending / receiving data...

Still some cosmetic to make it easier to maintain
