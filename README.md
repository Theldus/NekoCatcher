# NekoCatcher <img alt="Neko Catcher" src="https://i.imgur.com/TRIlRBS.png" width="50">
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

TL;DR: NekoCatcher helps you to find your soulmate ;-).

## Neko what?
Have you ever thought about how bad these dating apps are? You use your GPS,
put a picture, a description and expect someone to be interested in you...
don't get me wrong the idea is great, but we have a problem: range

By range I do not mean GPS location .. but the amount of people you can reach
with the app since the first requirement of all is: have the app.

For dozens of reasons your ideal girlfriend may not use this app, either because she
does not believe it works, have an old cell phone, have already deleted the app,
etc... whatever the reason, having the app is the key piece.

## So what it does?

To put it simple: NekoCatcher uses the ESP8266 (any model) to create an WiFi Hotspot
that simulates a register page from a public WiFi, like Airport, Shopping, FastFood...
does. This page asks the user for 3 things: name, birtdate and gender. If male, the
server redirects to a fake error page, if female we redirect to a 'proposal' page.

This 'proposal' page is meant to describe you, your qualities and ultimately present
some form of contact to get to you. It's up to you to change the page and leave it the
way you want.

In addition, a special link with authentication is available (/dumpnekos) as a way for
you to check how many and which people have filled out the form, although this does not
mean that these people will contact you. You also can download the current list as a
.csv file and clean the list from the ESP memory.

## Project + in action

NekoCatcher was made using Arduino IDE in ESP8266 NodeMCU v3 but is expected to work
in any version. All the html pages was minified through the [HTML Minifier](http://kangax.github.io/html-minifier/)
project and are stored directly in the source code as a giant constant string. The
index page was also gzipped (through the gzip command) to decreases the overhead
of redirecting everything to there.

Please note that WiFi is open and therefore has no security enabled, nor does the
webserver have http(s) enabled. I chose this to 'relieve' the ESP and not overload
it too much.

<p align="center">
	<img align="center" src="https://i.imgur.com/PoOSRC4.gif" alt="NekoCatcher in action">
	<br>
	<i>NekoCatcher in action</i>
</p>

## Disclaimer

This project is intended to be useful but without any guarantee or fitness for particular purposes,
I do not take any responsibility for damages and outcomes that can arise due the usage of this
project.

This is more like a toy project than a serious application but if you get someone with it, please,
tell me, I would be very glad to be a cupid of a couple, ;-).

----
Last, but not least, I'm really looking for a GF... so if anyone has an interest... :D
