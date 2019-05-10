// This document provides a brief introduction for the BSA HFP.

# Run bsa_bt_hfp.sh on terminal_1, using cell phone to connect DUT and press 13 to accept Simple
# Pairing:
Bluetooth Application Manager Main menu:
        1 => Abort Discovery
        2 => Discovery
        3 => Discovery test
        4 => Bonding
        5 => Cancel Bonding
        6 => Remove device from security database
        7 => Services Discovery (all services)
        8 => Device Id Discovery
        9 => Set local Device Id
        10 => Get local Device Id
        11 => Stop Bluetooth
        12 => Restart Bluetooth
        13 => Accept Simple Pairing  // <-- press 13 for pairing
        ...

# On terminal_2, need run app_hs on /userdata/bsa/config, and run the app_hs and using the other
# remote phone call cell phone, and press 4 to pick up the call:
cd /userdata/bsa/config
app_hs

Bluetooth Mono Headset Main menu:
    1  => discover
    2  => Connect
    3  => Close
    4  => pick up the call // <-- press 4
    5  => hang up the call
    6  => Play audio file
    7  => Record audio file
    8  => Stop recording audio file
    99  => Quit

# Now, the bluetooth has been connected, we can redirect audio path via
# alsa-utils for testing:
  * local capture and playback to BT: [DUT MICs -> PCM -> BT -> Phone -> Remote phone]
    [if using AMIC]
    arecord -Dhw:0,0 -f S16_LE -c 2 -r 16000 -t raw | aplay -t raw -Dhw:1,0 -f S16_LE -c 2 -r 16000
    [if using PDM MIC]
    arecord -Dplug:6mic_loopback -f S16_LE -c 2 -r 16000 -t raw | aplay -t raw -Dhw:1,0 -f S16_LE -c 2 -r 16000

  * capture from BT and local playback: [Remote phone -> Phone -> BT -> PCM -> DUT Speaker]
    arecord -Dhw:1,0 -f S16_LE -c 2 -r 16000 -t raw | aplay -t raw -Dhw:0,0 -f S16_LE -c 2 -r 16000
