/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "update.hh" /* includes net constants */
#include "hsapi.hh"
#include "error.hh"
#include "proxy.hh"
#include "ctr.hh"
#include "log.hh"

#include <3rd/json.hh>
#include <algorithm>
#include <malloc.h>

#define SOC_ALIGN       0x100000
#define SOC_BUFFERSIZE  0x20000

#define HS_UPDATE_BASE  "http://download2.erista.me/3hs"
#if !RELEASE && defined(HS_DEBUG_SERVER)
	#define HS_CDN_BASE HS_DEBUG_SERVER ":5001"
	#define HS_BASE_LOC HS_DEBUG_SERVER ":5000/api"
#else
	#define HS_CDN_BASE "http://download4.erista.me"
	#define HS_BASE_LOC "https://hshop.erista.me/api"
#endif

#define OK 0

// openssl s_client -showcerts -servername hshop.erista.me -connect hshop.erista.me:443 </dev/null | openssl x509 -outform DER -out hscert.der; xxd -i hscert.der; rm hscert.der
static unsigned char hscert_der[] = { 0x30, 0x82, 0x05, 0x25, 0x30, 0x82, 0x04, 0x0d, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x12, 0x03, 0x8c, 0x6f, 0xb8, 0x0e, 0xb8, 0xe5, 0xa3, 0xb0, 0x62, 0x1a, 0x8c, 0x58, 0xaf, 0x40, 0xf6, 0xa5, 0xfd, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x32, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x0d, 0x4c, 0x65, 0x74, 0x27, 0x73, 0x20, 0x45, 0x6e, 0x63, 0x72, 0x79, 0x70, 0x74, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x02, 0x52, 0x33, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x31, 0x30, 0x39, 0x32, 0x33, 0x30, 0x38, 0x31, 0x39, 0x31, 0x33, 0x5a, 0x17, 0x0d, 0x32, 0x31, 0x31, 0x32, 0x32, 0x32, 0x30, 0x38, 0x31, 0x39, 0x31, 0x32, 0x5a, 0x30, 0x1a, 0x31, 0x18, 0x30, 0x16, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x0f, 0x68, 0x73, 0x68, 0x6f, 0x70, 0x2e, 0x65, 0x72, 0x69, 0x73, 0x74, 0x61, 0x2e, 0x6d, 0x65, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xaf, 0xeb, 0xa1, 0xb2, 0x4d, 0x39, 0x39, 0x5e, 0x95, 0xc5, 0xdf, 0xe2, 0x03, 0xb2, 0xdb, 0x69, 0x6a, 0x26, 0xcb, 0x13, 0xed, 0x5f, 0x50, 0x0b, 0x62, 0xcb, 0x01, 0xe8, 0x63, 0x10, 0x31, 0x9e, 0x68, 0x0c, 0x92, 0xde, 0x42, 0x6f, 0x92, 0x4d, 0xb8, 0x90, 0x47, 0xcd, 0x03, 0x58, 0x0a, 0xb0, 0xc6, 0x4c, 0xa1, 0x28, 0x16, 0x7e, 0x57, 0x18, 0x12, 0x87, 0x7f, 0x34, 0xdb, 0x05, 0x0b, 0x38, 0x6d, 0x8b, 0x30, 0xa8, 0x54, 0x56, 0x1d, 0x48, 0x99, 0x86, 0x43, 0x93, 0xdd, 0xb3, 0x8b, 0xb6, 0x4d, 0xb1, 0xb7, 0x3b, 0x06, 0x9b, 0x0f, 0xea, 0xa9, 0x28, 0x02, 0x93, 0x9a, 0xdb, 0x84, 0xb5, 0xce, 0x7b, 0xfa, 0x11, 0xb2, 0x9a, 0x59, 0xa3, 0x2f, 0x8c, 0xbb, 0x5a, 0x2c, 0x25, 0x94, 0x28, 0x47, 0xae, 0x62, 0x56, 0x56, 0xd1, 0x80, 0x0a, 0x89, 0xe4, 0xb4, 0x60, 0xdc, 0x42, 0x43, 0xd8, 0xf8, 0xff, 0x2d, 0xca, 0x90, 0x2f, 0xeb, 0x59, 0xd9, 0x48, 0xdb, 0x8b, 0x1e, 0xd9, 0xbe, 0x6e, 0x44, 0xdd, 0x0d, 0x89, 0x63, 0x2e, 0xb8, 0xb4, 0x46, 0xf5, 0x32, 0x5b, 0x2b, 0x3a, 0x08, 0xe2, 0x04, 0x3a, 0x07, 0x96, 0x0e, 0x16, 0x74, 0x16, 0x87, 0x0a, 0x33, 0xe4, 0x33, 0xd3, 0xff, 0x96, 0xb9, 0x17, 0xfc, 0x88, 0x74, 0x41, 0x93, 0x87, 0x42, 0xd6, 0xa9, 0x6d, 0x6a, 0x69, 0x07, 0xa2, 0x05, 0xe7, 0xe8, 0x8d, 0x67, 0x28, 0xde, 0xac, 0xca, 0xd6, 0xaa, 0x92, 0xcd, 0x26, 0xb8, 0x0c, 0xa8, 0x47, 0xd6, 0x0a, 0xd8, 0x81, 0xf0, 0x9f, 0xf2, 0x48, 0xa9, 0x09, 0xf1, 0x02, 0xb5, 0x45, 0xe4, 0xc2, 0xab, 0x22, 0x90, 0x74, 0xfc, 0xb5, 0xb1, 0x7d, 0xd1, 0x98, 0xb4, 0xc0, 0x18, 0x69, 0xa5, 0x72, 0x73, 0xac, 0x5a, 0x3d, 0x7c, 0x83, 0x96, 0xa4, 0xfb, 0xe0, 0x4b, 0x56, 0xe6, 0xff, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x82, 0x02, 0x4b, 0x30, 0x82, 0x02, 0x47, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x05, 0xa0, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x25, 0x04, 0x16, 0x30, 0x14, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x52, 0xc8, 0x5a, 0x1c, 0xdf, 0xba, 0xfa, 0x84, 0x0f, 0x06, 0xaf, 0x11, 0x87, 0x57, 0xd3, 0x7b, 0x8c, 0x5e, 0xfb, 0xf2, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x14, 0x2e, 0xb3, 0x17, 0xb7, 0x58, 0x56, 0xcb, 0xae, 0x50, 0x09, 0x40, 0xe6, 0x1f, 0xaf, 0x9d, 0x8b, 0x14, 0xc2, 0xc6, 0x30, 0x55, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x01, 0x04, 0x49, 0x30, 0x47, 0x30, 0x21, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x01, 0x86, 0x15, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x72, 0x33, 0x2e, 0x6f, 0x2e, 0x6c, 0x65, 0x6e, 0x63, 0x72, 0x2e, 0x6f, 0x72, 0x67, 0x30, 0x22, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x02, 0x86, 0x16, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x72, 0x33, 0x2e, 0x69, 0x2e, 0x6c, 0x65, 0x6e, 0x63, 0x72, 0x2e, 0x6f, 0x72, 0x67, 0x2f, 0x30, 0x1a, 0x06, 0x03, 0x55, 0x1d, 0x11, 0x04, 0x13, 0x30, 0x11, 0x82, 0x0f, 0x68, 0x73, 0x68, 0x6f, 0x70, 0x2e, 0x65, 0x72, 0x69, 0x73, 0x74, 0x61, 0x2e, 0x6d, 0x65, 0x30, 0x4c, 0x06, 0x03, 0x55, 0x1d, 0x20, 0x04, 0x45, 0x30, 0x43, 0x30, 0x08, 0x06, 0x06, 0x67, 0x81, 0x0c, 0x01, 0x02, 0x01, 0x30, 0x37, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xdf, 0x13, 0x01, 0x01, 0x01, 0x30, 0x28, 0x30, 0x26, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x02, 0x01, 0x16, 0x1a, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x63, 0x70, 0x73, 0x2e, 0x6c, 0x65, 0x74, 0x73, 0x65, 0x6e, 0x63, 0x72, 0x79, 0x70, 0x74, 0x2e, 0x6f, 0x72, 0x67, 0x30, 0x82, 0x01, 0x05, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0xd6, 0x79, 0x02, 0x04, 0x02, 0x04, 0x81, 0xf6, 0x04, 0x81, 0xf3, 0x00, 0xf1, 0x00, 0x76, 0x00, 0x94, 0x20, 0xbc, 0x1e, 0x8e, 0xd5, 0x8d, 0x6c, 0x88, 0x73, 0x1f, 0x82, 0x8b, 0x22, 0x2c, 0x0d, 0xd1, 0xda, 0x4d, 0x5e, 0x6c, 0x4f, 0x94, 0x3d, 0x61, 0xdb, 0x4e, 0x2f, 0x58, 0x4d, 0xa2, 0xc2, 0x00, 0x00, 0x01, 0x7c, 0x11, 0xf3, 0xa9, 0x2f, 0x00, 0x00, 0x04, 0x03, 0x00, 0x47, 0x30, 0x45, 0x02, 0x20, 0x21, 0x3c, 0x3c, 0x4d, 0x29, 0x1f, 0x61, 0x7f, 0x69, 0xf2, 0x98, 0xef, 0x93, 0xa1, 0xf1, 0xb7, 0x19, 0xe2, 0x97, 0x3f, 0xf1, 0x98, 0x37, 0xc1, 0x9f, 0xb1, 0xce, 0xcd, 0xbc, 0xaa, 0xbe, 0x3c, 0x02, 0x21, 0x00, 0xf8, 0xcd, 0x11, 0x25, 0x2b, 0x38, 0x64, 0xa5, 0xd5, 0x25, 0x84, 0x22, 0x4f, 0x1a, 0xdd, 0x6a, 0xf1, 0x9d, 0xf9, 0x69, 0x3e, 0x28, 0xaf, 0x24, 0xd5, 0xbb, 0x2b, 0x83, 0x60, 0x27, 0xfa, 0x08, 0x00, 0x77, 0x00, 0x7d, 0x3e, 0xf2, 0xf8, 0x8f, 0xff, 0x88, 0x55, 0x68, 0x24, 0xc2, 0xc0, 0xca, 0x9e, 0x52, 0x89, 0x79, 0x2b, 0xc5, 0x0e, 0x78, 0x09, 0x7f, 0x2e, 0x6a, 0x97, 0x68, 0x99, 0x7e, 0x22, 0xf0, 0xd7, 0x00, 0x00, 0x01, 0x7c, 0x11, 0xf3, 0xa9, 0x58, 0x00, 0x00, 0x04, 0x03, 0x00, 0x48, 0x30, 0x46, 0x02, 0x21, 0x00, 0xf6, 0x34, 0x4d, 0x1d, 0x7f, 0x94, 0xa0, 0x1c, 0x1a, 0x98, 0xd7, 0xc3, 0x04, 0x22, 0xc5, 0x39, 0x9d, 0x43, 0xf8, 0x73, 0x86, 0x7b, 0xcc, 0x5f, 0x6a, 0x06, 0xbf, 0xf7, 0x82, 0x54, 0xd9, 0xdf, 0x02, 0x21, 0x00, 0xeb, 0x17, 0x40, 0x00, 0xc2, 0x71, 0x4a, 0xc3, 0xe1, 0x32, 0x6f, 0xdd, 0x8c, 0x7d, 0x4d, 0x1e, 0xfb, 0x6e, 0x2c, 0xc9, 0xa9, 0x4d, 0x1e, 0xdd, 0x7c, 0x04, 0x07, 0x95, 0x0d, 0x4c, 0x8b, 0x98, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x60, 0x93, 0xa5, 0x34, 0xec, 0x84, 0x1e, 0xae, 0x6e, 0xb1, 0xec, 0x94, 0xdd, 0x96, 0xc7, 0x7e, 0x1d, 0x04, 0x24, 0x47, 0x7c, 0xaa, 0x58, 0x48, 0xae, 0x9e, 0x73, 0x45, 0x5e, 0xd8, 0xa4, 0x28, 0xac, 0xde, 0x6e, 0x98, 0x40, 0x33, 0x60, 0x1a, 0x71, 0xbe, 0x40, 0x4b, 0xb5, 0xa6, 0x7e, 0x40, 0xb0, 0x60, 0x18, 0x34, 0x47, 0x5d, 0x27, 0xa1, 0x3f, 0xf1, 0xf7, 0x0c, 0x83, 0x5f, 0xc6, 0x82, 0xd4, 0x67, 0x85, 0xb3, 0x09, 0x0d, 0x8e, 0xc4, 0xbd, 0xe1, 0x93, 0x5b, 0x75, 0x2b, 0xd3, 0xb0, 0x45, 0x72, 0x9d, 0x67, 0x36, 0xea, 0xae, 0x63, 0xa7, 0xd7, 0x97, 0x52, 0xe1, 0x12, 0x61, 0xdd, 0x4c, 0x08, 0xd2, 0x0d, 0x25, 0x66, 0x23, 0x45, 0xd5, 0xde, 0x65, 0x5c, 0xd0, 0x8a, 0x9a, 0x3b, 0x82, 0xa5, 0xbd, 0x2b, 0x80, 0x79, 0x95, 0x5b, 0xe3, 0x59, 0x0e, 0xc4, 0xf4, 0xb5, 0xc9, 0xa5, 0xad, 0x91, 0x63, 0x8e, 0xe9, 0xe3, 0xf6, 0x89, 0xd5, 0x24, 0xec, 0x2c, 0xe0, 0xf4, 0xd7, 0x5c, 0x5c, 0x08, 0x07, 0x1b, 0x1d, 0x33, 0x3b, 0xa0, 0xc4, 0x58, 0x1f, 0x49, 0x4d, 0xdc, 0xaa, 0x24, 0x7f, 0x34, 0x81, 0x4e, 0x3f, 0xbb, 0x47, 0x46, 0x57, 0x45, 0xe5, 0xc5, 0x2c, 0x37, 0x45, 0x61, 0xf9, 0x7b, 0xf9, 0x42, 0x2b, 0x90, 0xf6, 0xb5, 0x11, 0x46, 0x35, 0x33, 0x66, 0x90, 0x90, 0xb7, 0xbe, 0x8e, 0x43, 0x1f, 0x5a, 0xc9, 0xb2, 0x17, 0x64, 0x23, 0xcc, 0x02, 0x7d, 0xa5, 0x5f, 0x22, 0x5f, 0x4a, 0x0c, 0xda, 0x1d, 0x9a, 0x8e, 0x62, 0xfb, 0x25, 0x13, 0x37, 0x49, 0x35, 0xd0, 0xf1, 0xaf, 0x5c, 0x25, 0xc8, 0x42, 0x6c, 0x9d, 0x53, 0x06, 0xc7, 0x4e, 0x9e, 0x43, 0x43, 0xd0, 0xa5, 0x9e, 0xc3, 0x2a, 0x64, 0x8f, 0x28, 0x1c, 0x35, 0x4e, 0x3e, 0xfe, 0x9f, 0x22, 0x33, 0x13, 0x24 };
static unsigned int hscert_der_len = 1321;

extern "C" void        hsapi_token(char *); /* hsapi_auth.c */
extern "C" const int   hsapi_token_length;  /* hsapi_auth.c */
extern "C" const char *hsapi_user;          /* hsapi_auth.c */

using ojson = nlohmann::ordered_json;
using json = nlohmann::json;

static u32 *g_socbuf = nullptr;
static hsapi::Index g_index;
hsapi::Index *hsapi::get_index()
{ return &g_index; }

static Result basereq(const std::string& url, std::string& data, HTTPC_RequestMethod reqmeth = HTTPC_METHOD_GET)
{
	httpcContext ctx;
	Result res = OK;

#define TRY(expr) do { res = (expr); if(R_FAILED(res)) { httpcCloseContext(&ctx); return res; } } while(0)
	TRY(httpcOpenContext(&ctx, reqmeth, url.c_str(), 0));
	TRY(httpcSetSSLOpt(&ctx, SSLCOPT_DisableVerify));
	TRY(httpcSetKeepAlive(&ctx, HTTPC_KEEPALIVE_ENABLED));
	TRY(httpcAddRequestHeaderField(&ctx, "Connection", "Keep-Alive"));
	TRY(httpcAddRequestHeaderField(&ctx, "User-Agent", USER_AGENT));
	TRY(httpcAddRequestHeaderField(&ctx, "X-Auth-User", hsapi_user));
/*TRY(httpcAddRequestHeaderField(&ctx, "X-Auth-Token", token));*/char*token=(char*)malloc(hsapi_token_length+1);hsapi_token(token);token[hsapi_token_length]=0;TRY(httpcAddRequestHeaderField(&ctx,"X-Auth-Token",token));memset(token,0,hsapi_token_length);free(token);
	if(url.find("https") == 0) // only use certs on https
		TRY(httpcAddTrustedRootCA(&ctx, hscert_der, hscert_der_len));
	TRY(proxy::apply(&ctx));

	TRY(httpcBeginRequest(&ctx));

	u32 status = 0;
	TRY(httpcGetResponseStatusCode(&ctx, &status));
	vlog("API status code on %s: %lu", url.c_str(), status);

	// Do we want to redirect?
	if(status / 100 == 3)
	{
		char newurl[2048];
		TRY(httpcGetResponseHeader(&ctx, "location", newurl, 2048));
		std::string redir(newurl);

		vlog("Redirected to %s", redir.c_str());
		httpcCloseContext(&ctx);
		return basereq(redir, data, reqmeth);
	}

	if(status != 200)
	{
#ifdef RELEASE
		// We _may_ require a different 3hs version
		if(status == 400)
		{
			char minver[2048] = {0};
			/* we can assume it doesn't have the header if this fails */
			if(R_SUCCEEDED(httpcGetResponseHeader(&ctx, "x-minimum", minver, 2048)))
			{
				httpcCloseContext(&ctx);
				panic(PSTRING(min_constraint, VVERSION, minver));
			}
		}
#endif

		httpcCloseContext(&ctx);
		return APPERR_NON200;
	}

	u32 totalSize = 0;
	TRY(httpcGetDownloadSizeState(&ctx, nullptr, &totalSize));
	if(totalSize != 0) data.reserve(totalSize);

	char buffer[4096];
	u32 dled = 0;

	do {
		res = httpcDownloadData(&ctx, (unsigned char *) buffer, sizeof(buffer), &dled);
		ui::Keys k = ui::RenderQueue::get_keys();
		if(R_SUCCEEDED(res) && ((k.kDown | k.kHeld) & (KEY_B | KEY_START)))
			res = APPERR_CANCELLED;
		// Other type of fail
		if(R_FAILED(res) && res != (Result) HTTPC_RESULTCODE_DOWNLOADPENDING)
		{
			/* httpcCloseContext() seems to hang if you don't cancel before
			 * calling it */
			httpcCancelConnection(&ctx);
			httpcCloseContext(&ctx);
			return res;
		}
		data += std::string(buffer, dled);
	} while(res == (Result) HTTPC_RESULTCODE_DOWNLOADPENDING);

	vlog("API data gotten:\n%s", data.c_str());
	httpcCloseContext(&ctx);
	return OK;
#undef TRY
}

template <typename J>
static Result basereq(const std::string& url, J& j, HTTPC_RequestMethod reqmeth = HTTPC_METHOD_GET)
{
	std::string data;
	Result res = basereq(url, data, reqmeth);
	if(R_FAILED(res)) return res;

	j = J::parse(data, nullptr, false);
	if(j == J::value_t::discarded)
		return APPERR_JSON_FAIL;
	return OK;
}

static void serialize_subcategories(std::vector<hsapi::Subcategory>& rscats, const std::string& cat, ojson& scats)
{
	for(ojson::iterator scat = scats.begin(); scat != scats.end(); ++scat)
	{
		ojson& jscat = scat.value();

		rscats.emplace_back();
		hsapi::Subcategory& s = rscats.back();

		s.disp = jscat["display_name"].get<std::string>();
		s.desc = jscat["description"].get<std::string>();
		s.name = scat.key();
		s.cat = cat;

		s.titles = jscat["total_content_count"].get<hsapi::hsize>();
		s.size = jscat["size"].get<hsapi::hsize>();
	}
}

static void serialize_categories(std::vector<hsapi::Category>& rcats, ojson& cats)
{
	for(ojson::iterator cat = cats.begin(); cat != cats.end(); ++cat)
	{
		ojson& jcat = cat.value();

		rcats.emplace_back();
		hsapi::Category& c = rcats.back();

		c.titles = jcat["total_content_count"].get<hsapi::hsize>();
		c.disp = jcat["display_name"].get<std::string>();
		c.desc = jcat["description"].get<std::string>();
		c.prio = jcat["priority"].get<hsapi::hprio>();
		c.size = jcat["size"].get<hsapi::hsize>();
		c.name = cat.key();

		serialize_subcategories(c.subcategories, c.name, jcat["subcategories"]);
	}
}

static void serialize_title(hsapi::Title& t, json& jt)
{
	t.size = jt["size"].get<hsapi::hsize>();
	t.id = jt["id"].get<hsapi::hid>();
	t.tid = ctr::str_to_tid(jt["title_id"].get<std::string>());
	t.cat = jt["category"].get<std::string>();
	t.subcat = jt["subcategory"].get<std::string>();
	t.name = jt["name"].get<std::string>();
}

static void serialize_titles(std::vector<hsapi::Title>& rtitles, json& j)
{
	for(json::iterator it = j.begin(); it != j.end(); ++it)
	{
		rtitles.emplace_back();
		serialize_title(rtitles.back(), it.value());
	}
}

static void serialize_titles_ver(std::vector<hsapi::FullTitle>& rtitles, json& j)
{
	for(json::iterator it = j.begin(); it != j.end(); ++it)
	{
		rtitles.emplace_back();
		serialize_title(rtitles.back(), it.value());
		rtitles.back().version = j["version"].get<hsapi::hiver>();
	}
}

// https://en.wikipedia.org/wiki/Percent-encoding
static std::string url_encode(const std::string& str)
{
	std::string ret;
	ret.reserve(str.size() * 3);
	char hex[4];

	for(size_t i = 0; i < str.size(); ++i)
	{
		if((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= '0' && str[i] <= '9')
			|| str[i] == '.' || str[i] == '-' || str[i] == '_' || str[i] == '~')
			ret += str[i];
		else
		{
			// C++ can be retarded at times
			// Why is there no std::hex(uchar) function?
			snprintf(hex, 4, "%%%02X", str[i]);
			ret += hex;
			break;
		}
	}

	return ret;
}

// https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string#1798170
static void trim(std::string& str, const std::string& whitespace)
{
	const size_t strBegin = str.find_first_not_of(whitespace);
	if(strBegin == std::string::npos) { str = ""; return; }

	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;

	str = str.substr(strBegin, strRange);
}

hsapi::Subcategory *hsapi::Category::find(const std::string& name)
{
	for(size_t i = 0; i < this->subcategories.size(); ++i)
	{
		if(this->subcategories[i].name == name || this->subcategories[i].disp == name)
			return &this->subcategories[i];
	}
	return nullptr;
}

hsapi::Category *hsapi::Index::find(const std::string& name)
{
	for(size_t i = 0; i < this->categories.size(); ++i)
	{
		if(this->categories[i].name == name || this->categories[i].disp == name)
			return &this->categories[i];
	}
	return nullptr;
}

Result hsapi::fetch_index()
{
	ojson j;
	Result res = OK;
	if(R_FAILED(res = basereq<ojson>(HS_BASE_LOC "/title-index", j)))
		return res;
	j = j["value"];

	g_index.titles = j["total_content_count"].get<hsapi::hsize>();
	g_index.size = j["size"].get<hsapi::hsize>();

	serialize_categories(g_index.categories, j["entries"]);
	std::sort(g_index.categories.begin(), g_index.categories.end());

	return OK;
}

Result hsapi::titles_in(std::vector<hsapi::Title>& ret, const std::string& cat, const std::string& scat)
{
	json j;
	Result res = OK;
	if(R_FAILED(res = basereq<json>(HS_BASE_LOC "/title/category/" + cat + "/" + scat, j)))
		return res;
	j = j["value"];

	serialize_titles(ret, j);
	return OK;
}

void hsapi::global_deinit()
{
	socExit();
	if(g_socbuf != NULL)
		free(g_socbuf);
}

bool hsapi::global_init()
{
	if((g_socbuf = (u32 *) memalign(SOC_ALIGN, SOC_BUFFERSIZE)) == NULL)
		return false;
	if(R_FAILED(socInit(g_socbuf, SOC_BUFFERSIZE)))
		return false;
	return true;
}

Result hsapi::title_meta(hsapi::FullTitle& ret, hsapi::hid id)
{
	json j;
	Result res = OK;
	if(R_FAILED(res = basereq<json>(HS_BASE_LOC "/title/" + std::to_string(id), j)))
		return res;
	j = j["value"];

	serialize_title(ret, j);

	// now we serialize for the FullTitle exclusive fields
	ret.prod = j["product_code"].get<std::string>();
	ret.version = j["version"].get<hsapi::hiver>();

	return OK;
}

Result hsapi::get_download_link(std::string& ret, const hsapi::Title& meta)
{
	json j;
	Result res = OK;
	if(R_FAILED(res = basereq<json>(HS_CDN_BASE "/content/" + std::to_string(meta.id) + "/request", j)))
		return res;
	j = j["value"];

	ret = HS_CDN_BASE "/content/" + std::to_string(meta.id) + "?token=" + j["token"].get<std::string>();
	return OK;
}

Result hsapi::search(std::vector<hsapi::Title>& ret, const std::string& query)
{
	json j;
	Result res = OK;
	if(R_FAILED(res = basereq<json>(HS_BASE_LOC "/title/search?q=" + url_encode(query), j)))
		return res;
	j = j["value"];

	serialize_titles(ret, j);
	return OK;
}

Result hsapi::batch_related(hsapi::BatchRelated& ret, const std::vector<hsapi::htid>& tids)
{
	if(tids.size() == 0) return OK;

	std::string url = HS_BASE_LOC "/title/related/batch?title_ids=" + ctr::tid_to_str(tids[0]);
	for(size_t i = 1; i < tids.size(); ++i) url += "&title_ids=" + ctr::tid_to_str(tids[i]);

	json j;
	Result res = OK;
	if(R_FAILED(res = basereq<json>(url, j)))
		return res;
	j = j["value"];

	for(json::iterator it = j.begin(); it != j.end(); ++it)
	{
		htid tid = ctr::str_to_tid(it.key());
		serialize_titles_ver(ret[tid].updates, it.value()["updates"]);
		serialize_titles_ver(ret[tid].dlc, it.value()["dlc"]);
	}

	return OK;
}

Result hsapi::get_latest_version_string(std::string& ret)
{
	Result res = OK;
	if(R_FAILED(res = basereq(HS_UPDATE_BASE "/version", ret)))
		return res;
	trim(ret, " \t\n");
	return OK;
}


std::string hsapi::update_location(const std::string& ver)
{
	return HS_UPDATE_BASE "/app-" + ver + ".cia";
}

std::string hsapi::parse_vstring(hsapi::hiver version)
{
	// based on:
	//  "{(ver >> 10) & 0x3F}.{(ver >> 4) & 0x3F}.{ver & 0xF}"
	return "v"
		+ std::to_string(version >> 10 & 0x3F) + "."
		+ std::to_string(version >> 4  & 0x3F) + "."
		+ std::to_string(version       & 0xF );
}

