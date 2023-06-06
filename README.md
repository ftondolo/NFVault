# NFVault
A Near-Field Communication (NFC) based authenticator

<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#State of this Project">Prerequisites</a>
      <ul>
        <li><a href="#Goal">Goal</a></li>
        <li><a href="#Current Functionality">Current Functionality</a></li>
        <li><a href="#To Implement">To Implement</a></li>
      </ul>
    </li>
    <li>
      <a href="#Usage">## Usage</a>
      <ul>
        <li><a href="#Theory">Theory</a></li>
      </ul>
      <ul>
        <li><a href="#Practice">Practice</a></li>
      </ul>
    </li>
  </ol>
</details>

## State of this Project
### Goal
I set out to create a small portable device which, upon recognising an NFC, could login to devices or sites in a rapid and secure manner. Most such Arduino-based solutions stored passowrds in plaintext, allowing for easy reading of the authentication data from the microcontroller, not to mention the overall inflexibility of most of these solutions with regards to password management.
### Current Functionality
At present, it is possible to use any number of different tag UIDs to type out any one of multiple passwords (albeit assuming the same general behaviour for LOG IN/OUT). Most importantly however, the passwords are sored encrypted with AES 128-bit encryption and as such are unreadable from the microcontroller code.
### To Implement
Need to allow for multiple log IN/OUT behaviour, permitting for use to authenticate both devices, websites, apps, etc.
## Usage
### Theory
The theory behid the NFVault is quite simple: the arduino uses the tags' UIDs to decrypt the corresponding password cyphertexts stored on the Arduino and then types those passwords out acting as a pseudo-keyboard on the computer it is plugged into.
### Practice
First, to create the cyphertext which the NFVault will store one runs ```AES_config.ino``` on the hardware, after inserting one's password in the code, observing the serial output while reading different tags will output HEX cyphertext to be stored on the final device program (```nfvault.ino```).

Then, you can add these newly generated cyphertextsto to the password 3-D array in the ```nfvault.ino``` script, each row (X) corresponding to a passowrd and each column (Y) to a (Z) 16-byte cyphertext of said password encrypted with a given tag's UID. When reading a tag the Arduino will loop through every password and attempt to decrypt every encryted variation thereof, if a decrypted text is all-ASCII, the corresponding plaintext is typed out! If the tag in question is tapped again, the prescibed log out script is then run. A sleep timeout is implemented but commented out, uncommeting said code will automatically assume a logout after the ```sleep_timeout``` in seconds (customisable).
