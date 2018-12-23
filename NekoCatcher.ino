/*
 * MIT License
 *
 * Copyright (c) 2018 Davidson Francis <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#define MAX_NEKO_AMOUNT  20
#define NEKO_NAME_LENGTH 31
#define NEKO_AGE_LENGTH  11

/* ============== CHANGE THIS ============== */
#define www_user "changeme"
#define www_passwd "changeme"
/* ========================================= */

#define DNS_PORT 53
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

/* error.html. */
static const String page_male = "<!doctypehtml><meta content=\"text/html; charset=utf-8\"http-equiv=Content-Type><title>Resource not available</title><h1>Resource not available</h1><p>The server is receiving too many requests, please try again in a few hours<hr><address>Apache/2.4.29 (Ubuntu) Server at nekonetworks Port 80</address>";

/* neko.html. */
static const String page_female = "<!doctypehtml><meta charset=utf-8><title>Neko Networks</title><style>body{background-image:url(\"data:image/svg+xml;utf8,<svg enable-background='new 0 0 50 50' version='1.1' viewBox='0 0 50 50' xml:space='preserve' xmlns='http://www.w3.org/2000/svg'><path d='M24.85,10.126c2.018-4.783,6.628-8.125,11.99-8.125c7.223,0,12.425,6.179,13.079,13.543  c0,0,0.353,1.828-0.424,5.119c-1.058,4.482-3.545,8.464-6.898,11.503L24.85,48L7.402,32.165c-3.353-3.038-5.84-7.021-6.898-11.503  c-0.777-3.291-0.424-5.119-0.424-5.119C0.734,8.179,5.936,2,13.159,2C18.522,2,22.832,5.343,24.85,10.126z' fill='#D75A4A' opacity='0.1'/></svg>\");background-repeat:repeat}p{text-align:center;font-family:'Helvetica Neue',Helvetica,Arial,sans-serif}ul{display:table;margin:0 auto;font-family:'Helvetica Neue',Helvetica,Arial,sans-serif}.title{font-size:200%;color:red}</style><div class=main><p class=title>Neko Networks<p>This is a demo page, please describe yourself here to let the girl know some characteristics of you<p>My best qualities are:<ul><li>Quality 1<li>Quality 2<li>Quality n<li>...</ul><p>If you are interested or would like to know more about me, please contact me: WhatsApp/Telegram/Facebook/Instagram/E-mail... or whatever</div>";

/* dump.html. */
static const String dump_top = "<!DOCTYPE html><html><head><meta charset=\"utf-8\" /><title>Neko Networks</title><style>body { font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif; background:rgb(44, 51, 63); margin:15px; text-align: center;}p {text-align: center;color: #4caf50;font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif;}button { width: 100%; background: #4caf50; color: #fff; font-size: 1em; font-weight: bold; border: 0; padding: 20px 0; outline: none; cursor: pointer; transition: all 0.2s ease-in-out; margin: 10px 0 0; } button:hover { background: #66bb6a; }.main {margin: 0 auto;max-width: 440px;}.title {font-size: 200%;font-weight: bold;}.counter {text-align: left !important;}#nekos {font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;border-collapse: collapse;width: 440px;}#nekos td, #nekos th {border: 1px solid #2c333f;padding: 8px;}#nekos tr:nth-child(even){ background-color: #f2f2f2; }#nekos tr:nth-child(odd){ background-color: white; }#nekos th {padding-top: 12px;padding-bottom: 12px;text-align: center;background-color: #4caf50;color: white;}</style></head><body><div class=\"main\"><p class=\"title\">Neko Networks</p>";
static const String dump_bottom = "</table><button onclick=\"window.open('http://192.168.1.1/getnekos','_blank');\">Save and Reset</button></div></body></html>";

/**
 * index.html
 *
 * Wtf is this? relax... this is just the gzipped version of the
 * minified/index.html... why? I'm assuming that the most part of
 * accesses will be provided from a smartphone, and, since our DNS
 * Server resolves any domain to our IP... the ESP will receives a
 * lot of HTTP requests from every app that the user have installed..
 * so we need to serve the index.html as soon as possible.
 *
 * You can get this by running:
 *        gzip -c html/minified/index.html | xxd -i | sed 's/  /\t/g'
 * paste the output here, and ggwp.
 */
const char index_page[] = 
{
	0x1f, 0x8b, 0x08, 0x08, 0x92, 0xdd, 0x1f, 0x5c, 0x00, 0x03, 0x69, 0x6e,
	0x64, 0x65, 0x78, 0x2e, 0x68, 0x74, 0x6d, 0x6c, 0x00, 0x9d, 0x56, 0xdd,
	0x6e, 0xac, 0x36, 0x10, 0x7e, 0x15, 0xf7, 0x1c, 0x55, 0x27, 0x91, 0x16,
	0x30, 0x3f, 0x4b, 0x08, 0x0b, 0x48, 0xfd, 0x51, 0x7a, 0x7a, 0x91, 0xa3,
	0xaa, 0x77, 0xbd, 0xaa, 0xbc, 0xc6, 0x80, 0x15, 0x83, 0xa9, 0x6d, 0xb2,
	0x49, 0x56, 0xfb, 0x40, 0x7d, 0x8d, 0x3e, 0x59, 0xc7, 0x98, 0xcd, 0x92,
	0x9c, 0x56, 0x95, 0x2a, 0x6b, 0xcd, 0xcc, 0x60, 0xcf, 0xcc, 0x37, 0xf3,
	0xd9, 0x6c, 0xf1, 0x4d, 0x2d, 0xa9, 0x79, 0x1e, 0x59, 0x67, 0x7a, 0x51,
	0x15, 0x3d, 0x33, 0x04, 0xd1, 0x8e, 0x28, 0xcd, 0x4c, 0x39, 0x99, 0xc6,
	0xcb, 0xaa, 0xc2, 0x70, 0x23, 0x58, 0xf5, 0x2b, 0x6b, 0xb9, 0x36, 0x4c,
	0x15, 0x81, 0xd3, 0x0b, 0x6d, 0x9e, 0xe1, 0xb1, 0x97, 0xf5, 0xf3, 0xb1,
	0x91, 0x83, 0xf1, 0x1a, 0xd2, 0x73, 0xf1, 0x9c, 0x7f, 0xfa, 0xcc, 0xc4,
	0x23, 0x33, 0x9c, 0x12, 0xf4, 0x85, 0x4d, 0xec, 0xd3, 0xe6, 0x55, 0xdf,
	0x7c, 0xa7, 0x38, 0x11, 0x1b, 0x4d, 0x06, 0xed, 0x69, 0xa6, 0x78, 0xb3,
	0xdb, 0x13, 0xfa, 0xd0, 0x2a, 0x39, 0x0d, 0x75, 0xfe, 0x31, 0xa2, 0x71,
	0x1c, 0x37, 0xbb, 0x9e, 0xa8, 0x96, 0x0f, 0x79, 0xb8, 0x1d, 0x9f, 0x76,
	0x86, 0x3d, 0x19, 0x8f, 0x08, 0xde, 0x0e, 0x39, 0x65, 0x03, 0xc4, 0x3e,
	0x75, 0xa1, 0x8b, 0xa5, 0xf9, 0x0b, 0xcb, 0x43, 0x3f, 0x61, 0xfd, 0x79,
	0x03, 0x3e, 0x75, 0xea, 0x78, 0x96, 0x51, 0x84, 0x61, 0xfb, 0x5e, 0xaa,
	0x9a, 0xa9, 0x7c, 0x90, 0x03, 0xdb, 0x75, 0x8c, 0xb7, 0x9d, 0xc9, 0x43,
	0x6b, 0x5e, 0x05, 0x6d, 0xb0, 0x1d, 0x27, 0x41, 0xf6, 0x4c, 0x1c, 0x6b,
	0xae, 0x47, 0x41, 0x9e, 0xf3, 0xbd, 0x90, 0xf4, 0x61, 0x77, 0x89, 0xe3,
	0x67, 0x10, 0x66, 0x24, 0x75, 0xcd, 0x87, 0x36, 0xb7, 0x9e, 0x11, 0x46,
	0x36, 0xbd, 0x79, 0xc5, 0xc1, 0x39, 0xbe, 0xc1, 0x78, 0x9d, 0xae, 0x60,
	0x8d, 0x71, 0x5e, 0xf3, 0x86, 0x2b, 0x6d, 0x3c, 0xda, 0x71, 0x51, 0x1f,
	0x17, 0x27, 0x9e, 0x91, 0x23, 0x24, 0xcc, 0x87, 0x71, 0x32, 0x1b, 0xcd,
	0x04, 0xa3, 0xe6, 0x78, 0xe0, 0xb5, 0xe9, 0xf2, 0x10, 0xe3, 0x6f, 0x97,
	0xbc, 0x3d, 0x45, 0x6a, 0x3e, 0xe9, 0x3c, 0xb9, 0x20, 0xd1, 0x52, 0xf0,
	0x1a, 0x01, 0x06, 0xf4, 0x91, 0x66, 0x76, 0xbc, 0xc5, 0xd2, 0x34, 0x3b,
	0x39, 0x19, 0xc1, 0x07, 0x96, 0xe3, 0xd7, 0x7c, 0x31, 0x0a, 0x5d, 0x2d,
	0x9e, 0x2c, 0x18, 0x6b, 0x59, 0xdc, 0x83, 0x65, 0x67, 0x14, 0xf4, 0x82,
	0x1b, 0x2e, 0x87, 0x9c, 0x08, 0x81, 0xfc, 0x48, 0x23, 0x46, 0x34, 0xf3,
	0xf8, 0xe0, 0x81, 0xa7, 0xdd, 0xff, 0x6d, 0xeb, 0x52, 0xec, 0x04, 0x9f,
	0x8b, 0xe4, 0xca, 0x78, 0xcb, 0x7a, 0x87, 0x39, 0x6f, 0x24, 0x9d, 0xf4,
	0xf1, 0x7d, 0xf2, 0x4b, 0x62, 0x54, 0x0a, 0xa9, 0x80, 0x0f, 0x61, 0x96,
	0xcd, 0x46, 0x48, 0xbc, 0x23, 0xb5, 0x3c, 0x00, 0x14, 0x3b, 0x62, 0x80,
	0xaf, 0xda, 0x3d, 0xb9, 0x8a, 0x37, 0x21, 0x8e, 0x36, 0x51, 0x98, 0x6c,
	0xfc, 0xf8, 0xfa, 0xe4, 0xf7, 0x84, 0x0f, 0x17, 0x02, 0x90, 0xc9, 0x48,
	0xa0, 0xc6, 0x93, 0xe7, 0xea, 0x9a, 0xd8, 0x5c, 0xdc, 0x1a, 0xe4, 0xef,
	0x01, 0x75, 0x7d, 0x5c, 0x11, 0xed, 0xe4, 0x0b, 0x09, 0xb2, 0x2d, 0x09,
	0xf2, 0x1b, 0xa9, 0x7a, 0xaf, 0xe1, 0x4c, 0xd4, 0x7a, 0xb3, 0xb6, 0x77,
	0x8c, 0x40, 0x76, 0xc7, 0x35, 0x0f, 0xd6, 0xfb, 0xf6, 0x93, 0x31, 0x72,
	0x78, 0xd3, 0xc6, 0x15, 0xbc, 0x84, 0x92, 0x66, 0x8b, 0x77, 0x0b, 0x34,
	0x0b, 0x76, 0x45, 0x63, 0x60, 0xd7, 0x7b, 0x2a, 0x2d, 0x0d, 0xc7, 0xef,
	0x68, 0xf7, 0x8e, 0x1a, 0xb6, 0x1c, 0x40, 0x0f, 0xfb, 0x5b, 0x75, 0x9e,
	0x4e, 0x4a, 0x43, 0x94, 0x51, 0x72, 0x7b, 0x64, 0xfe, 0xab, 0xc7, 0xe7,
	0x32, 0x38, 0x5a, 0xe3, 0xaf, 0x21, 0xe5, 0x9d, 0x7c, 0x04, 0xdc, 0x6b,
	0x34, 0x69, 0xba, 0xdf, 0xa7, 0x64, 0xb5, 0xf4, 0xdf, 0x5a, 0x79, 0xa1,
	0xf0, 0x09, 0xea, 0x2a, 0x21, 0x9d, 0xe3, 0xbb, 0x63, 0xb5, 0x54, 0x64,
	0x7b, 0xb3, 0xa5, 0xe9, 0xf6, 0x0d, 0xda, 0x53, 0x11, 0xb8, 0x1b, 0xa6,
	0xa8, 0xf9, 0x23, 0xa2, 0x82, 0x68, 0x5d, 0xda, 0xfe, 0xad, 0xf5, 0xb9,
	0x93, 0x70, 0x13, 0x3d, 0xb6, 0xc8, 0x71, 0xae, 0x4c, 0x32, 0x34, 0xb7,
	0xc0, 0x0a, 0x4f, 0xbd, 0x18, 0x74, 0xd9, 0x19, 0x33, 0xe6, 0x41, 0x70,
	0x38, 0x1c, 0xfc, 0x43, 0xec, 0x4b, 0xd5, 0x06, 0x11, 0xc6, 0x38, 0x80,
	0x3d, 0x55, 0x31, 0x12, 0xd3, 0xa1, 0xba, 0xfc, 0x70, 0x0f, 0xc8, 0xbb,
	0x24, 0x7b, 0x4c, 0xb2, 0xcf, 0xf8, 0xa5, 0x5f, 0x2b, 0x1f, 0x1a, 0x2e,
	0x44, 0x69, 0xaf, 0x10, 0x14, 0xac, 0xd6, 0x47, 0x28, 0xcc, 0x44, 0x82,
	0x12, 0x7a, 0xeb, 0xdf, 0x26, 0x9e, 0x9d, 0x50, 0x94, 0xfa, 0x38, 0x75,
	0x62, 0x9c, 0x22, 0x2c, 0x12, 0x2f, 0xf9, 0x21, 0x8e, 0xfd, 0x2c, 0x45,
	0x5b, 0x3b, 0x85, 0x89, 0x1f, 0x2e, 0xa2, 0xdd, 0xfc, 0xd2, 0x83, 0x16,
	0xa6, 0x22, 0x45, 0x30, 0xbc, 0x94, 0x7a, 0xb1, 0x1f, 0xdb, 0x5f, 0xe4,
	0x65, 0x7e, 0x9a, 0x39, 0x29, 0x8c, 0x10, 0x64, 0xe3, 0x65, 0x9e, 0x0b,
	0xb5, 0xf5, 0xb7, 0x91, 0x67, 0x27, 0xeb, 0x2b, 0xc9, 0x9c, 0x18, 0x61,
	0x17, 0x8a, 0x7a, 0x37, 0xfe, 0x4d, 0xe4, 0xa6, 0x08, 0xfb, 0x51, 0xba,
	0x88, 0x19, 0x3a, 0x63, 0xb0, 0x7d, 0xb1, 0x18, 0x1c, 0xf2, 0x00, 0x8a,
	0xb8, 0xae, 0xe4, 0x6b, 0x2b, 0xd7, 0x46, 0x47, 0xf9, 0xaa, 0xe8, 0xc2,
	0xd5, 0xed, 0x0f, 0xca, 0xb2, 0xbb, 0x83, 0x57, 0xf6, 0xb4, 0x20, 0x42,
	0x2d, 0xbd, 0x4a, 0xb5, 0xac, 0x59, 0xbb, 0x58, 0x9d, 0xa6, 0xaa, 0x98,
	0x2f, 0x44, 0x04, 0xa6, 0xb2, 0x99, 0x84, 0xf8, 0x7d, 0x20, 0x3d, 0xab,
	0xbe, 0xc0, 0x54, 0x04, 0xf3, 0x9b, 0x0a, 0x15, 0xf3, 0xf5, 0x80, 0x78,
	0x7d, 0x59, 0x80, 0xec, 0xb4, 0x52, 0x15, 0xfb, 0x63, 0xe2, 0x8a, 0xd5,
	0xf3, 0x09, 0xa7, 0x64, 0xe4, 0x06, 0x6e, 0xdc, 0x17, 0x56, 0x4a, 0x40,
	0x37, 0x9b, 0xa4, 0x52, 0x70, 0x9f, 0xce, 0x3a, 0x9c, 0x7f, 0xc1, 0x86,
	0x16, 0xd8, 0x10, 0x63, 0xa4, 0x47, 0x26, 0x04, 0xed, 0x18, 0x7d, 0x28,
	0x1b, 0x22, 0x34, 0x83, 0x68, 0x97, 0x7c, 0xf6, 0x5c, 0x99, 0xae, 0x26,
	0x86, 0x55, 0xdf, 0x9f, 0xa5, 0x7f, 0x48, 0xea, 0x75, 0x95, 0x4b, 0xea,
	0xa2, 0xbe, 0x26, 0x65, 0x3f, 0xa4, 0xe5, 0xec, 0x67, 0xed, 0xbd, 0x65,
	0x83, 0xad, 0xe3, 0x4f, 0xf3, 0xe3, 0xe2, 0xd7, 0xdd, 0xfc, 0xd6, 0xb1,
	0x5b, 0xe0, 0xbc, 0x2e, 0xf2, 0xd9, 0x65, 0x55, 0xc8, 0xd1, 0x56, 0xf7,
	0xfc, 0x44, 0x8f, 0x44, 0x4c, 0xac, 0xbc, 0xaf, 0xee, 0x89, 0x60, 0x6f,
	0x6d, 0x77, 0xd5, 0x1d, 0xeb, 0xad, 0x35, 0x70, 0x9e, 0xcf, 0x7d, 0x72,
	0xe7, 0xb8, 0xfa, 0x45, 0x49, 0xca, 0x58, 0x5d, 0x04, 0x8b, 0x5e, 0x04,
	0xb6, 0x39, 0x5f, 0x53, 0xc1, 0x1d, 0xd4, 0xea, 0xaf, 0x3f, 0x0b, 0x4d,
	0x15, 0x1f, 0x4d, 0x05, 0x7f, 0x10, 0xa6, 0x1e, 0xbe, 0xbf, 0xfe, 0x41,
	0x71, 0xc3, 0xae, 0xae, 0x06, 0x76, 0x40, 0x3f, 0x02, 0xc6, 0x6b, 0xbf,
	0x65, 0xe6, 0x0e, 0x3a, 0xf3, 0x1b, 0x23, 0xea, 0xea, 0xfa, 0x1a, 0xe2,
	0xba, 0x0d, 0xf0, 0x85, 0x78, 0x90, 0x30, 0x99, 0x83, 0x54, 0x0f, 0x7a,
	0x83, 0x7e, 0x1e, 0xa8, 0xbf, 0x84, 0x99, 0xe7, 0xbf, 0x01, 0x30, 0x0f,
	0xe6, 0x8a, 0x74, 0x08, 0x00, 0x00
};

/**
 * Neko struture, ;-)
 */
struct __attribute__((packed)) neko
{
	char name[NEKO_NAME_LENGTH];
	char age[NEKO_AGE_LENGTH];
	char gender;
};

/* Nekos. */
struct neko neko_log[MAX_NEKO_AMOUNT] = {0};
int neko_counter = 0;

/**
 * Register page, this is where we decide to save or
 * not our neko.
 */
void registerNeko()
{
	if (webServer.hasArg("full_name") && webServer.hasArg("birthdate")
		&& webServer.hasArg("gender"))
	{
		char gender = webServer.arg("gender").charAt(0);
		
		if (gender == 'M')
		{
			webServer.send(200, "text/html", page_male);
		}
		else if (gender == 'F')
		{
			int len = 0;
			String name = webServer.arg("full_name");
			String birt = webServer.arg("birthdate");
			
			/* We have a neko :O, let us save first. */
			
			/* Name. */
			if (name.length() > NEKO_NAME_LENGTH - 1)
				len = NEKO_NAME_LENGTH - 1;
			else
				len = name.length();
			
			strncpy(neko_log[neko_counter].name, name.c_str(), len);
			neko_log[neko_counter].name[len] = '\0';

			/* Birthday. */
			if (birt.length() > NEKO_AGE_LENGTH - 1)
				len = NEKO_AGE_LENGTH - 1;
			else
				len = birt.length();			
			
			strncpy(neko_log[neko_counter].age, webServer.arg("birthdate").c_str(), len);
			neko_log[neko_counter].age[len] = '\0';

			/* Gender. */
			neko_log[neko_counter].gender = gender;

			/* Update counter. */
			neko_counter = (neko_counter + 1) % MAX_NEKO_AMOUNT;

			/* Now, let us send our special page to her. */
			webServer.send(200, "text/html", page_female);
		}
	}
	else
		webServer.send(200, "text/html", page_male);
}

/**
 * Dump all the nekos read until now...
 */
void dumpNekos()
{
	/**
	 * I know, this is pretty useless without any type of cryptography,
	 * but I hope it helps a little bit, just to keep away the curious.
	 */
	if (!webServer.authenticate(www_user, www_passwd))
		return webServer.requestAuthentication();
	
	String page = dump_top;

	/* Neko counter. */
	page += "<p class=\"counter\"> ";
	page += neko_counter;
	page += " Nekos found</p>";

	/* Table header. */
	page += "<table id=\"nekos\">";
	page += "<tr><th>Neko</th><th>Birthdate</th><th>Gender</th></tr>";

	/* Table. */
	for (int i = 0; i < neko_counter; i++)
	{
		page += "<tr>";
		
		page += "<td>";
		page += neko_log[i].name;
		page += "</td>";
		
		page += "<td>";
		page += neko_log[i].age;
		page += "</td>";
		
		page += "<td>";
		page += neko_log[i].gender;
		page += "</td>";
		
		page += "</tr>";
	}

	page += dump_bottom;

	/* Send. */
	webServer.send(200, "text/html", page);
}

/**
 * Download all the nekos obtained uuntil now.
 */
void getNekos()
{
	/**
	 * I know, this is pretty useless without any type of cryptography,
	 * but I hope it helps a little bit, just to keep away the curious.
	 */
	if (!webServer.authenticate(www_user, www_passwd))
		return webServer.requestAuthentication();
	
	String response_header = "";
	String response_file = "";

	/* Our file.csv. */
	response_file = "neko,birthdate,gender\n";
	for (int i = 0; i < neko_counter; i++)
	{
		response_file += neko_log[i].name;
		response_file += ",";
		response_file += neko_log[i].age;
		response_file += ",";
		response_file += neko_log[i].gender;
		response_file += "\n";
	}

	response_header  = "HTTP/1.0 200 OK\r\n";
	response_header += "Content-Disposition:attachment;filename=nekos.csv\r\n";
	response_header += "Content-type: application/octet-stream\r\n";
	response_header += "Content-Length: ";
	response_header += response_file.length();
	response_header += "\r\n\r\n";
	response_header += response_file;

	/* Clear our data until now. */
	memset(&neko_log, 0, sizeof(struct neko) * MAX_NEKO_AMOUNT);
	neko_counter = 0;

	/* Sends the content. */
	webServer.sendContent(response_header);
}

/**
 * Setup the DNS, WebServer
 */
void setup()
{
	WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP("Neko Networks - Free Wifi");

	/* Start DNS server for a specific domain name. */
	dnsServer.start(DNS_PORT, "*", apIP);

	/* Simple HTTP server to see that DNS server is working. */
	webServer.onNotFound([]()
	{
			String response_header = "";
			response_header  = "HTTP/1.1 200 OK\r\n";
			response_header += "Content-Encoding: gzip\r\n";
			response_header += "Content-type: text/html\r\n";
			response_header += "Connection: close\r\n";
			response_header += "Content-Length: ";
			response_header += sizeof(index_page);
			response_header += "\r\n\r\n";
			
			webServer.sendContent(response_header);
			webServer.sendContent_P(index_page, sizeof(index_page));
	});

	/* Binds. */
	webServer.on("/register", registerNeko);
	webServer.on("/dumpnekos", dumpNekos);
	webServer.on("/getnekos", getNekos);

	webServer.begin();
}

/**
 * Main loop.
 */
void loop()
{
	dnsServer.processNextRequest();
	webServer.handleClient();
	delay(10);
}
