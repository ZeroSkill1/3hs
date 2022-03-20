# 3hs

Blobfrii 3ds client for erista's hShop

Credits to ihaveamac for the seeddb

3hs is licensed under GPLv3 (see: LICENSE)

nlohmann::json is licensed under MIT (see: LICENSE.nlohann-json.TXT)

# building

requirements:
- devkitarm
- libctru
- citro2d
- citro3d

Run `make` for a debug build and `make RELEASE=1 cia` for a release cia build.

Other variables used in the building process:
- VERSION=_version-int_ => A version to set in the release cia.
- DEVICE\_ID=_device-id-int_ => A device id to limit the build to.
- HS\_DEBUG\_SERVER="_debug-server_" => Servers to use instead of the official ones. Only active in debug builds.

