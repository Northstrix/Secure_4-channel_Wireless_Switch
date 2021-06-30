# Secure_4-channel_Wireless_Switch

The security of this device based on four pillars:
1) Attacker doesn't know the MAC address of the receiver;
2) Attacker doesn't know the keys;
3) Each IV is encrypted with its own key;
4) Attacker doesn't know the IVs.

Now, let me explain what it means:
1) There's no way to send any instructions to the device if an attacker doesn't know where to send it;
2) Even if an attacker will discover the MAC address, an attacker will still need three keys and three IVs.
3) Even if one key gets compromised, an attacker will still have to guess the other two.
4) Even if an attacker will obtain the MAC address of the receiver and all three keys. An attacker will still need to guess all three IVs. There are 10 to the power of 24 (1,000,000,000,000,000,000,000,000) possible combinations, and let's be honest, this, is a big number.

Features:

* Encrypted communication channel
* Invulnerability to the replay attacks
* Counter of the unsuccessful attempts to interact with the device
* Each IV encrypted with its own key
* Using SPIFFS to store the IVs
* Possibility to enormously expand the number of channels by connecting up to 253 Arduinos
* Easy-to-use

You can find the tutorial here: https://www.instructables.com/Secure-4-channel-Wireless-Switch-for-Smart-Home-Wi/
