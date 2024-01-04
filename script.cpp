#include "stdafx.h"

BlipList* pBlipList;
//BlipList* pBlipList;
///GtaThread_VTable gGtaThreadOriginal;
///GtaThread_VTable gGtaThreadNew;
HANDLE mainFiber;
DWORD wakeAt;
#pragma region key sync
bool isKeyPressedOnce(bool& bIsPressed, DWORD vk)
{
	if (IsKeyPressed(vk))
	{
		if (bIsPressed == FALSE)
		{
			bIsPressed = TRUE;
			return TRUE;
		}
	}
	else if (bIsPressed == TRUE)
	{
		bIsPressed = FALSE;
	}
	return FALSE;
}

void ReleaseKeys()
{
	keybd_event(VK_NUMPAD1, 0, KEYEVENTF_KEYUP, 0);
}
#pragma endregion
#pragma region menu
void draw_rect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7)
{
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}

static const float TEXT_HEIGHT_TITLE = 24.0f;
void draw_menu_header_line(std::string caption, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active, bool rescaleText = true, int curPage = 1, int pageCount = 1)
{
	// default values
	int text_col[4] = { 13, 123, 86, 255 }, // header colours possibly
		rect_col[4] = { 0, 0, 0, 153 }; // was previously all 0's

	float text_scale = rescaleText ? 0.60 : 0.35;

	int font = 7/*2*/;
	bool outline = false;
	bool dropShadow = false;

	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	float lineWidthScaled = lineWidth / (float)screen_w; // line width
	float lineTopScaled = lineTop / (float)screen_h; // line top offset
	float textLeftScaled = textLeft / (float)screen_w; // text left offset
	float lineHeightScaled = lineHeight / (float)screen_h; // line height

	float lineLeftScaled = lineLeft / (float)screen_w;

	float textHeightScaled = TEXT_HEIGHT_TITLE / (float)screen_h;

	// this is how it's done in original scripts

	// text upper part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);

	if (outline)
	{
		UI::SET_TEXT_OUTLINE();
	}

	if (dropShadow)
	{
		UI::SET_TEXT_DROPSHADOW(5, 0, 78, 255, 255);
	}

	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());

	float textY = lineTopScaled + (0.5f * (lineHeightScaled - textHeightScaled));
	float leftMarginScaled = textLeftScaled - lineLeftScaled;

	UI::_DRAW_TEXT(textLeftScaled, textY);

	// rect
	draw_rect(lineLeftScaled, lineTopScaled,
		lineWidthScaled, lineHeightScaled,
		rect_col[0], rect_col[1], rect_col[2], rect_col[3]);

	// draw page count in different colour
	if (pageCount > 1)
	{
		std::ostringstream ss;
		ss << " ~r~" << curPage << "~w~ of ~r~" << pageCount;

		text_col[0] = 255;
		text_col[1] = 180;
		text_col[2] = 0;

		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(0.0, text_scale);
		UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
		UI::SET_TEXT_RIGHT_JUSTIFY(1);

		if (outline)
		{
			UI::SET_TEXT_OUTLINE();
		}

		if (dropShadow)
		{
			UI::SET_TEXT_DROPSHADOW(5, 0, 78, 255, 255);
		}


		UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
		UI::SET_TEXT_WRAP(0.0f, lineLeftScaled + lineWidthScaled - leftMarginScaled);
		UI::_SET_TEXT_ENTRY("STRING");

		auto ssStr = ss.str();
		UI::_ADD_TEXT_COMPONENT_STRING((char*)ssStr.c_str());
		UI::_DRAW_TEXT(0, textY);
	}
}

void draw_menu_line(std::string caption, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active, bool title, bool rescaleText = TRUE)
{
	// default values
	int text_col[4] = { 255, 255, 255, 255 },
		rect_col[4] = { 255, 255, 255, 160 };
	float text_scale = 0.35f;
	int font = 4;
	// correcting values for active line
	if (active)
	{
		text_col[0] = 255;
		text_col[1] = 255;
		text_col[2] = 255;
		text_col[3] = 255;

		rect_col[0] = 255;
		rect_col[1] = 0;
		rect_col[2] = 0;
		rect_col[3] = 225;

		if (rescaleText) text_scale = 0.35f;
	}

	if (title)
	{
		text_col[0] = 255;
		text_col[1] = 255;
		text_col[2] = 255;
		text_col[3] = 255;

		rect_col[0] = 0;
		rect_col[1] = 0;
		rect_col[2] = 0;
		rect_col[3] = 180;

		if (rescaleText) text_scale = 0.50f;
		font = 4;
	}

	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	textLeft += lineLeft;

	float lineWidthScaled = lineWidth / (float)screen_w; // line width
	float lineTopScaled = lineTop / (float)screen_h; // line top offset
	float textLeftScaled = textLeft / (float)screen_w; // text left offset
	float lineHeightScaled = lineHeight / (float)screen_h; // line height
	float lineLeftScaled = lineLeft / (float)screen_w;

	// this is how it's done in original scripts
	UI::SET_TEXT_OUTLINE();
	// text upper part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	UI::_DRAW_TEXT(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

	// text lower part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_BEGIN_TEXT_COMMAND_LINE_COUNT("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	int num25 = UI::_GET_TEXT_SCREEN_LINE_COUNT(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

	// rect
	draw_rect(lineLeftScaled, lineTopScaled + (0.00125f),
		lineWidthScaled, ((((float)(num25)*UI::_GET_TEXT_SCALE_HEIGHT(text_scale, 0)) + (lineHeightScaled * 2.0f)) + 0.005f),
		rect_col[0], rect_col[1], rect_col[2], rect_col[3]);
}

std::string line_as_str(std::string text, bool* pState)
{
	while (text.size() < 0) text += "";
	return text + (pState ? (*pState ? " :  ~g~On" : " :  ~r~Off") : "");
}

bool trainer_switch_pressed()
{
	static bool bF4Pressed = FALSE;
	return isKeyPressedOnce(bF4Pressed, VK_F4);
}

void get_button_state(bool* a, bool* b, bool* up, bool* down, bool* l, bool* r)
{
	static bool bNum5Pressed = FALSE;
	if (a) *a = isKeyPressedOnce(bNum5Pressed, VK_NUMPAD5);

	static bool bNum0Pressed = FALSE;
	if (b) *b = isKeyPressedOnce(bNum0Pressed, VK_NUMPAD0) || trainer_switch_pressed() || isKeyPressedOnce(bNum0Pressed, VK_BACK);

	static bool bNum8Pressed = FALSE;
	if (up) *up = isKeyPressedOnce(bNum8Pressed, VK_NUMPAD8);

	static bool bNum2Pressed = FALSE;
	if (down) *down = isKeyPressedOnce(bNum2Pressed, VK_NUMPAD2);

	static bool bNum6Pressed = FALSE;
	if (r) *r = isKeyPressedOnce(bNum6Pressed, VK_NUMPAD6);

	static bool bNum4Pressed = FALSE;
	if (l) *l = isKeyPressedOnce(bNum4Pressed, VK_NUMPAD4);
}

void menu_beep_cancel()
{
	AUDIO::PLAY_SOUND_FRONTEND(-1, "CANCEL", "HUD_FRONTEND_DEFAULT_SOUNDSET", 1);
}

void menu_beep_up_down()
{
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 1);
}

void menu_beep_select()
{
	AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", 1);
}

std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

void update_status_text()
{
	if (GetTickCount() < statusTextDrawTicksMax)
	{
		if (statusTextGxtEntry)
		{
			UI::_SET_NOTIFICATION_TEXT_ENTRY((char*)statusText.c_str());
		}
		else
		{
			UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char*)statusText.c_str());
		}
		UI::_DRAW_NOTIFICATION(FALSE, FALSE);
	}
}

void set_status_text(std::string str, DWORD time = 250, bool isGxtEntry = FALSE)
{
	statusText = str;
	statusTextDrawTicksMax = GetTickCount() + time;
	statusTextGxtEntry = isGxtEntry;
}

void set_map_text(char* text)
{
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
}

void set_creator_text(char* text)
{
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_FRANKLIN", "CHAR_FRANKLIN", 1, rand() % 8, "~h~~r~iTzNeutron", "~h~~c~Version ~g~420", 1, "___@DEV", rand() % 8, 1);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
	std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	std::string welcome = "~h~Welcome ~y~";
	set_status_text(welcome + playerName);
}

void set_online_text(char* message, char* name)
{
	char text[100];
	sprintf_s(text, message, name);
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
}

void map_player_status(char* message, char* name, int health, int armor, float distance, char* me)
{
	char text[500];
	sprintf_s(text, message, name, health, armor, distance, me);
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
}

void player_coords()
{
	Vector3 playerCoord = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 0);
	std::string textX = "~r~x~s~: ";
	std::string xText = std::to_string(playerCoord.x);
	std::string textY = "~n~~r~y~s~: ";
	std::string yText = std::to_string(playerCoord.y);
	std::string textZ = "~n~~r~z~s~: ";
	std::string zText = std::to_string(playerCoord.z);
	set_status_text(textX + xText + textY + yText + textZ + zText);
}

void show_notification(char* text)
{
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
}
#pragma endregion
#pragma region cam coords
Vector3 rotDirection(Vector3* rot)
{
	float radianz = rot->z * 0.0174532924f;
	float radianx = rot->x * 0.0174532924f;
	float num = std::abs((float)std::cos((double)radianx));

	Vector3 dir;

	dir.x = (float)((double)((float)(-(float)std::sin((double)radianz))) * (double)num);
	dir.y = (float)((double)((float)std::cos((double)radianz)) * (double)num);
	dir.z = (float)std::sin((double)radianx);

	return dir;
}

Vector3 add(Vector3* a, Vector3* b)
{
	Vector3 result;

	result.x = a->x;
	result.y = a->y;
	result.z = a->z;
	result.x += b->x;
	result.y += b->y;
	result.z += b->z;

	return result;
}

Vector3 multiply(Vector3* coords, float coordz)
{
	Vector3 result;

	result.x = coords->x;
	result.y = coords->y;
	result.z = coords->z;
	result.x *= coordz;
	result.y *= coordz;
	result.z *= coordz;

	return result;
}

float distance(Vector3* pointa, Vector3* pointb)
{
	float ax = pointa->x;
	float ay = pointa->y;
	float az = pointa->z;
	float bx = pointb->x;
	float by = pointb->y;
	float bz = pointb->z;

	double xba = (double)(bx - ax);
	double yba = (double)(by - ay);
	double zba = (double)(bz - az);

	double y2 = yba * yba;
	double x2 = xba * xba;
	double sum = y2 + x2;

	return (float)std::sqrt(sum + zba);
}
#pragma endregion
#pragma region booleans
// features
bool featurePlayerESP = FALSE;
bool featurePlayerESPBoxes = FALSE;
bool featurePlayerESPTrace = FALSE;
bool featurePlayerInvincible = FALSE;
bool featurePlayerInvincibleUpdated = FALSE;
bool featurePlayerNeverWanted = FALSE;
bool featurePlayerIgnored = FALSE;
bool featurePlayerIgnoredUpdated = FALSE;
bool featurePlayerUnlimitedAbility = FALSE;
bool featurePlayerNoNoise = FALSE;
bool featurePlayerNoNoiseUpdated = FALSE;
bool featurePlayerFastSwim = FALSE;
bool featurePlayerFastSwimUpdated = FALSE;
bool featurePlayerFastRun = FALSE;
bool featurePlayerFastRunUpdated = FALSE;
bool featurePlayerSuperJump = FALSE;
bool playerInvisible = FALSE;
bool playerInvisibleUpdated = FALSE;
bool cashAdd = FALSE;
bool featureNightVision = FALSE;
bool featureNightVisionUpdated = FALSE;
bool featureThermalVision = FALSE;
bool featureThermalVisionUpdated = FALSE;
bool featurePlayerUltraJump = FALSE;
bool featurePlayerSuperRun = FALSE;
bool featurePlayerBloodDamage = FALSE;
bool featurePlayerAutoHeal = FALSE;
bool featurePlayerHancock = FALSE;
bool featureAirBrake = FALSE;
DWORD cashUpdateTime;

bool featureWeaponTriggerbot = FALSE;
bool featureWeaponNoReload = FALSE;
bool featureWeaponNoReloadUpdated = FALSE;
bool featureWeaponFireAmmo = FALSE;
bool featureWeaponExplosiveAmmo = FALSE;
bool featureWeaponExplosiveMelee = FALSE;
bool featureWeaponFirework = FALSE;
bool featureWeaponVehicleRocket = FALSE;
bool featureWeaponTank = FALSE;
bool featureWeaponRailGun = FALSE;
bool featureWeaponHeavySniper = FALSE;
bool featureWeaponMinigun = FALSE;
bool featureWeaponPlaneRocket = FALSE;
bool featureWeaponGrenadeLauncher = FALSE;
bool featureWeaponAirstrikeRocket = FALSE;
bool featureWeaponSavage = FALSE;
bool featureWeaponSpaceRocket = FALSE;
bool featureWeaponPropMoneyBag = FALSE;
bool featureWeaponPropCokeBlock = FALSE;
bool featureWeaponPropWeed = FALSE;
bool featureWeaponPropWeedPallet = FALSE;
bool featureWeaponPropXmasTree = FALSE;
bool featureWeaponPropXmas = FALSE;
bool featureWeaponPropDildo = FALSE;
bool weaponOnePunchKill = FALSE;
bool weaponOnePunchKillUpdated = FALSE;
bool featureWeaponDamageModifier = FALSE;
bool featureWeaponDamageModifierUpdate = FALSE;
bool weaponForceGun = FALSE;
bool weaponGravityGun = FALSE;
bool gravityLock = FALSE;
bool weaponSpaghettios = FALSE;
bool weaponDeleteObjects = FALSE;

bool featureVehInvincible = FALSE;
bool featureVehInvincibleUpdated = FALSE;
bool featureVehInvisible = FALSE;
bool featureVehInvisibleUpdated = FALSE;
bool featureVehInvincibleWheels = FALSE;
bool featureVehInvincibleWheelsUpdated = FALSE;
bool featureVehSeatbelt = FALSE;
bool featureVehSeatbeltUpdated = FALSE;
bool featureVehSpeedBoost = FALSE;
bool featureVehWrapInSpawned = TRUE;
DWORD vehicleUpdateTime;
bool featureVehRainbow = FALSE;
bool featureVehDrift = FALSE;
bool featureVehGrip = FALSE;
bool featureVehSlammed = FALSE;
bool featureVehSlammedUpdated = FALSE;
bool featureVehAutoFix = FALSE;
bool featureVehDamageModifier = FALSE;
bool featureVehDamageModifierUpdate = FALSE;

bool featureWorldMoonGravity = FALSE;
bool worldClearVehicles = FALSE;
bool worldClearObjects = FALSE;
bool worldClearPeds = FALSE;
bool worldClearCops = FALSE;
bool worldClearProjectiles = FALSE;
bool worldMassacre = FALSE;

bool featureMiscGlobeRadio = FALSE;
bool featureMiscGlobeRadioUpdated = FALSE;
bool featureMiscLockRadio = FALSE;
bool featureMiscHideHud = FALSE;
bool featureMiscRestrictedZones = FALSE;
bool featurePedestrianInvincibleBodyguard = FALSE;
bool featurePedestrianInvincibleBodyguardUpdate = FALSE;
bool featureNearbyPeds = FALSE;
bool featureSetPedCalm0 = FALSE;
bool featureSetPedCalm1 = FALSE;
bool featureKillPed = FALSE;
bool featureRichPed = FALSE;
bool featureRevivePed = FALSE;
bool featureEspPed = FALSE;

bool featureSkinChanger = FALSE;
bool playerWasDisconnected = TRUE;
bool featurePlayerBlips = FALSE;
bool featurePlayerHeadDisplay = FALSE;
bool featurePlayerBlipCone = FALSE;
bool featurePlayerNotifications = FALSE;
bool featureShowVoiceChatSpeaker = FALSE;
bool menuVisible = FALSE;
#pragma endregion
#pragma region lpcstr
LPCSTR weaponNames[55] = { "WEAPON_KNIFE", "WEAPON_NIGHTSTICK", "WEAPON_HAMMER", "WEAPON_BAT", "WEAPON_GOLFCLUB", "WEAPON_CROWBAR", "WEAPON_PISTOL", "WEAPON_COMBATPISTOL", "WEAPON_APPISTOL", "WEAPON_PISTOL50", "WEAPON_MICROSMG", "WEAPON_SMG", "WEAPON_ASSAULTSMG", "WEAPON_ASSAULTRIFLE", "WEAPON_CARBINERIFLE", "WEAPON_ADVANCEDRIFLE", "WEAPON_MG", "WEAPON_COMBATMG", "WEAPON_PUMPSHOTGUN", "WEAPON_SAWNOFFSHOTGUN", "WEAPON_ASSAULTSHOTGUN", "WEAPON_BULLPUPSHOTGUN", "WEAPON_STUNGUN", "WEAPON_SNIPERRIFLE", "WEAPON_HEAVYSNIPER", "WEAPON_GRENADELAUNCHER", "WEAPON_GRENADELAUNCHER_SMOKE", "WEAPON_RPG", "WEAPON_MINIGUN", "WEAPON_GRENADE", "WEAPON_STICKYBOMB", "WEAPON_SMOKEGRENADE", "WEAPON_BZGAS", "WEAPON_MOLOTOV", "WEAPON_FIREEXTINGUISHER", "WEAPON_PETROLCAN", "WEAPON_SNSPISTOL", "WEAPON_SPECIALCARBINE", "WEAPON_HEAVYPISTOL", "WEAPON_BULLPUPRIFLE", "WEAPON_HOMINGLAUNCHER", "WEAPON_PROXMINE", "WEAPON_SNOWBALL", "WEAPON_VINTAGEPISTOL", "WEAPON_DAGGER", "WEAPON_FIREWORK", "WEAPON_MUSKET", "WEAPON_MARKSMANRIFLE", "WEAPON_HEAVYSHOTGUN", "WEAPON_GUSENBERG", "WEAPON_HATCHET", "WEAPON_RAILGUN", "WEAPON_COMBATPDW", "WEAPON_KNUCKLE", "WEAPON_MARKSMANPISTOL" };

LPCSTR weaponComponents[98] = { "COMPONENT_PISTOL_CLIP_02", "COMPONENT_AT_PI_SUPP_02", "COMPONENT_AT_PI_FLSH", "COMPONENT_PISTOL_VARMOD_LUXE" , "COMPONENT_SNSPISTOL_CLIP_02" , "COMPONENT_HEAVYPISTOL_CLIP_02", "COMPONENT_AT_PI_SUPP", "COMPONENT_AT_PI_FLSH", "COMPONENT_HEAVYPISTOL_VARMOD_LUXE" , "COMPONENT_COMBATPISTOL_CLIP_02", "COMPONENT_AT_PI_SUPP", "COMPONENT_AT_PI_FLSH" , "COMPONENT_APPISTOL_CLIP_02", "COMPONENT_AT_PI_SUPP", "COMPONENT_AT_PI_FLSH", "COMPONENT_APPISTOL_VARMOD_LUXE" , "COMPONENT_PISTOL50_CLIP_02", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_AT_PI_FLSH", "COMPONENT_PISTOL50_VARMOD_LUXE" , "COMPONENT_VINTAGEPISTOL_CLIP_02", "COMPONENT_AT_PI_SUPP" ,"COMPONENT_MICROSMG_CLIP_02", "COMPONENT_AT_SCOPE_MACRO", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_AT_PI_FLSH", "COMPONENT_MICROSMG_VARMOD_LUXE" , "COMPONENT_SMG_CLIP_02", "COMPONENT_AT_SCOPE_MACRO_02", "COMPONENT_AT_PI_SUPP", "COMPONENT_AT_AR_FLSH", "COMPONENT_SMG_VARMOD_LUXE" , "COMPONENT_ASSAULTSMG_CLIP_02", "COMPONENT_AT_SCOPE_MACRO", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_AT_AR_FLSH" , "COMPONENT_ASSAULTRIFLE_CLIP_02", "COMPONENT_AT_SCOPE_MACRO", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_AT_AR_AFGRIP", "COMPONENT_AT_AR_FLSH", "COMPONENT_ASSAULTRIFLE_VARMOD_LUXE" , "COMPONENT_CARBINERIFLE_CLIP_02", "COMPONENT_AT_SCOPE_MEDIUM", "COMPONENT_AT_AR_SUPP", "COMPONENT_AT_AR_AFGRIP", "COMPONENT_AT_AR_FLSH", "COMPONENT_CARBINERIFLE_VARMOD_LUXE" , "COMPONENT_ADVANCEDRIFLE_CLIP_02", "COMPONENT_AT_SCOPE_SMALL", "COMPONENT_AT_AR_SUPP", "COMPONENT_AT_AR_FLSH", "COMPONENT_ADVANCEDRIFLE_VARMOD_LUXE" , "COMPONENT_MG_CLIP_02", "COMPONENT_AT_SCOPE_SMALL_02" , "COMPONENT_COMBATMG_CLIP_02", "COMPONENT_AT_SCOPE_MEDIUM", "COMPONENT_AT_AR_AFGRIP" , "COMPONENT_SAWNOFFSHOTGUN_VARMOD_LUXE" , "COMPONENT_AT_SR_SUPP", "COMPONENT_AT_AR_FLSH" , "COMPONENT_ASSAULTSHOTGUN_CLIP_02", "COMPONENT_AT_AR_SUPP", "COMPONENT_AT_AR_AFGRIP" , "COMPONENT_AT_AR_FLSH", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_AT_AR_AFGRIP" , "COMPONENT_AT_SCOPE_MAX", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_SNIPERRIFLE_VARMOD_LUXE" , "COMPONENT_AT_SCOPE_LARGE" , "COMPONENT_AT_SCOPE_SMALL", "COMPONENT_AT_AR_FLSH", "COMPONENT_AT_AR_AFGRIP" , "COMPONENT_BULLPUPRIFLE_CLIP_02", "COMPONENT_AT_AR_FLSH", "COMPONENT_AT_SCOPE_SMALL", "COMPONENT_AT_AR_SUPP", "COMPONENT_AT_AR_AFGRIP" , "COMPONENT_GUSENBERG_CLIP_02" , "COMPONENT_HEAVYSHOTGUN_CLIP_02", "COMPONENT_AT_AR_FLSH", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_AT_AR_AFGRIP" , "COMPONENT_MARKSMANRIFLE_CLIP_02", "COMPONENT_AT_AR_FLSH", "COMPONENT_AT_AR_SUPP", "COMPONENT_AT_AR_AFGRIP", "COMPONENT_MARKSMANRIFLE_VARMOD_LUXE" , "COMPONENT_SPECIALCARBINE_CLIP_02", "COMPONENT_AT_AR_FLSH", "COMPONENT_AT_SCOPE_MEDIUM", "COMPONENT_AT_AR_SUPP_02", "COMPONENT_AT_AR_AFGRIP" , "COMPONENT_COMBATPDW_CLIP_02", "COMPONENT_AT_AR_FLSH", "COMPONENT_AT_SCOPE_SMALL", "COMPONENT_AT_AR_AFGRIP" };
#pragma endregion weapon
#pragma region extras
// player model control, switching on normal ped model when needed	
void check_player_model()
{
	// common variables
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!ENTITY::DOES_ENTITY_EXIST(playerPed)) return;

	Hash model = ENTITY::GET_ENTITY_MODEL(playerPed);
	if (ENTITY::IS_ENTITY_DEAD(playerPed) || PLAYER::IS_PLAYER_BEING_ARRESTED(player, TRUE))
		if (model != GAMEPLAY::GET_HASH_KEY("mp_f_freemode_01") && model != GAMEPLAY::GET_HASH_KEY("mp_m_freemode_01"))
		{
			std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
			std::string text = " ~y~change your outfit to restore your skin.";
			set_status_text(playerName + text);
			WAIT(1000);

			model = GAMEPLAY::GET_HASH_KEY("mp_f_freemode_01");
			STREAMING::REQUEST_MODEL(model);
			while (!STREAMING::HAS_MODEL_LOADED(model))
				WAIT(0);
			PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);

			// wait until player is ressurected
			while (ENTITY::IS_ENTITY_DEAD(PLAYER::PLAYER_PED_ID()) || PLAYER::IS_PLAYER_BEING_ARRESTED(player, TRUE))
				WAIT(0);

		}
}

void requestControl(DWORD entity)
{
	NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
	int tick = 0;
	while (tick <= 50)
	{
		if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity))
			WAIT(0);
		else
			return;
		NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
		tick++;
	}
}

void requestControlPlayer(DWORD entity)
{
	Any networkId = NETWORK::NETWORK_GET_NETWORK_ID_FROM_ENTITY(entity);
	int tick = 0;

	while (!NETWORK::NETWORK_HAS_CONTROL_OF_NETWORK_ID(networkId) && tick <= 50)
	{
		NETWORK::NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(networkId);
		tick++;
	}
}

void applyForceToEntity(Entity entity, float x, float y, float z)
{
	ENTITY::APPLY_FORCE_TO_ENTITY(entity, 1, x, y, z, 0.0f, 0.0f, 0.0f, 0, TRUE, TRUE, TRUE, FALSE, TRUE);
}

void cash(Ped ped)
{
	if (cashUpdateTime + 500.0 < GetTickCount())
	{
		cashUpdateTime = GetTickCount();
		DWORD hash = GAMEPLAY::GET_HASH_KEY((char*)"a_c_rat");
		if (STREAMING::IS_MODEL_IN_CDIMAGE(hash) && STREAMING::IS_MODEL_VALID(hash))
		{
			STREAMING::REQUEST_MODEL(hash);
			while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(0);
			Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(ped, FALSE);
			Ped rat = PED::CREATE_PED(1, hash, playerCoords.x, playerCoords.y, playerCoords.z, ENTITY::GET_ENTITY_HEADING(ped), TRUE, TRUE);
			PED::SET_PED_MONEY(rat, rand() % 2000);
			WAIT(0);
			ENTITY::SET_ENTITY_HEALTH(rat, 0);
			WAIT(0);
			PED::DELETE_PED(&rat);
			WAIT(0);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
		}
	}
	std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	std::string text = " ~g~cash was added.";
	set_status_text(playerName + text);
}

void animation(Ped playerPed, char* dict, char* anim)
{
	STREAMING::REQUEST_ANIM_DICT(dict);
	while (!STREAMING::HAS_ANIM_DICT_LOADED(dict))
		WAIT(0);
	AI::TASK_PLAY_ANIM(playerPed, dict, anim, 1.0f, -1.f, -1, 48, 0.0f, FALSE, FALSE, FALSE);
	STREAMING::REMOVE_ANIM_DICT(dict);
	std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	std::string message = " ~g~nice moves.";
	set_status_text(caption + message);
}

void framePlayer(Ped Ped, int value, char* name)
{
	for (int i = 0; i < 32; i++)
	{
		if (ENTITY::DOES_ENTITY_EXIST(Ped))
		{
			set_online_text("%s ~g~was framed.", name);
			Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED(i), 0.0f, 0.0f, 0.0f);
			FIRE::ADD_OWNED_EXPLOSION(Ped, coords.x, coords.y, coords.z, 37, 5.0f, FALSE, TRUE, FALSE);
		}
		else
		{
			std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(value));
			std::string message = " ~r~does not exist.";
			set_status_text(caption + message);
		}
	}
}

void freezePlayer(Ped ped)
{
	AI::CLEAR_PED_TASKS_IMMEDIATELY(ped);
	AI::CLEAR_PED_SECONDARY_TASK(ped);
}

void superRounds(Ped ped, char* value)
{
	float startDistance = distance(&CAM::GET_GAMEPLAY_CAM_COORD(), &ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE));
	float endDistance = distance(&CAM::GET_GAMEPLAY_CAM_COORD(), &ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE));
	startDistance += 0.25f;
	endDistance += 1000.0f;
	if (PED::IS_PED_SHOOTING(ped))
	{
		Hash weaponAssetRocket = GAMEPLAY::GET_HASH_KEY(value);
		if (!WEAPON::HAS_WEAPON_ASSET_LOADED(weaponAssetRocket))
		{
			WEAPON::REQUEST_WEAPON_ASSET(weaponAssetRocket, 31, FALSE);
			while (!WEAPON::HAS_WEAPON_ASSET_LOADED(weaponAssetRocket))
				WAIT(0);
		}
		GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), startDistance)).x, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), startDistance)).y, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), startDistance)).z, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), endDistance)).x, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), endDistance)).y, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), endDistance)).z, 250, 1, weaponAssetRocket, ped, 1, 0, -1);
	}
}

void shootObject(Ped ped, char* value)
{
	float spawnDistance = distance(&CAM::GET_GAMEPLAY_CAM_COORD(), &ENTITY::GET_ENTITY_COORDS(ped, FALSE));
	//spawnDistance += 2;
	Hash hash = GAMEPLAY::GET_HASH_KEY(value);
	if (PED::IS_PED_SHOOTING(ped) && STREAMING::IS_MODEL_VALID(hash))
	{
		Object object = OBJECT::CREATE_OBJECT(hash, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), spawnDistance)).x, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), spawnDistance)).y, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), spawnDistance)).z, TRUE, TRUE, FALSE);
		if (!STREAMING::HAS_MODEL_LOADED(hash))
		{
			STREAMING::REQUEST_MODEL(hash);
			while (!STREAMING::HAS_MODEL_LOADED(hash))
			{
				WAIT(0);
			}
		}
		else if (STREAMING::HAS_MODEL_LOADED(hash))
		{
			if (ENTITY::DOES_ENTITY_EXIST(object))
			{
				applyForceToEntity(object, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).x * 2500.0f, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).y * 2500.0f, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).z * 2500.0f);
				ENTITY::SET_ENTITY_LOD_DIST(object, 3000);
			}
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
		}
	}
}

void gravityGun()
{
	Entity entity;
	DWORD wepaonHash;
	WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &wepaonHash, 1);
	float spawnDistance = distance(&CAM::GET_GAMEPLAY_CAM_COORD(), &ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE));
	spawnDistance += 5;
	if (gravityLock)
	{
		if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(PLAYER::PLAYER_ID(), &entity) && GetAsyncKeyState(VK_RBUTTON))
		{
			Vector3 EntityTargetPos = ENTITY::GET_ENTITY_COORDS(entity, FALSE);
			PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), TRUE);
			if (ENTITY::IS_ENTITY_A_PED(entity) && PED::IS_PED_IN_ANY_VEHICLE(entity, 1))
			{
				entity = PED::GET_VEHICLE_PED_IS_IN(entity, 0);
			}
			requestControl(entity);
			if (ENTITY::IS_ENTITY_A_VEHICLE(entity)) ENTITY::SET_ENTITY_HEADING(entity, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_ID()));
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), spawnDistance)).x, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), spawnDistance)).y, add(&CAM::GET_GAMEPLAY_CAM_COORD(), &multiply(&rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)), spawnDistance)).z, 0, 0, 0);
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "Foot_Swish", entity, "docks_heist_finale_2a_sounds", 0, 0);
				ENTITY::SET_ENTITY_HEADING(entity, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_ID()));
				applyForceToEntity(entity, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).x * 10000.0f, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).y * 10000.0f, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).z * 10000.0f);
				gravityLock = FALSE;
				PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), FALSE);
			}
		}
	}
	if (!PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(PLAYER::PLAYER_ID(), &entity))
	{
		gravityLock = TRUE;
		PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), FALSE);
	}
}

void forceGun(Player player, Ped ped)
{
	Entity entity;
	DWORD dword;
	WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &dword, TRUE);
	float spawnDistance = distance(&CAM::GET_GAMEPLAY_CAM_COORD(), &ENTITY::GET_ENTITY_COORDS(ped, TRUE));
	if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &entity))
	{
		requestControl(entity);
		if (ENTITY::IS_ENTITY_A_PED(entity) && PED::IS_PED_IN_ANY_VEHICLE(entity, TRUE))
		{
			entity = PED::GET_VEHICLE_PED_IS_IN(entity, FALSE);
		}
		if (PED::IS_PED_SHOOTING(ped))
		{
			applyForceToEntity(entity, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).x * 10000.0f, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).y * 10000.0f, rotDirection(&CAM::GET_GAMEPLAY_CAM_ROT(0)).z * 10000.0f);
		}
	}
}

Vehicle clonePedVehicle(Ped ped)
{
	Vehicle pedVeh = NULL;
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	if (PED::IS_PED_IN_ANY_VEHICLE(ped, FALSE))
		pedVeh = PED::GET_VEHICLE_PED_IS_IN(ped, FALSE);
	else
		pedVeh = PED::GET_VEHICLE_PED_IS_IN(ped, TRUE);
	if (ENTITY::DOES_ENTITY_EXIST(pedVeh))
	{
		Hash vehicleModelHash = ENTITY::GET_ENTITY_MODEL(pedVeh);
		STREAMING::REQUEST_MODEL(vehicleModelHash);
		Vector3 playerPosition = ENTITY::GET_ENTITY_COORDS(playerPed, FALSE);
		Vehicle playerVeh = VEHICLE::CREATE_VEHICLE(vehicleModelHash, playerPosition.x, playerPosition.y, playerPosition.z, ENTITY::GET_ENTITY_HEADING(playerPed), TRUE, TRUE);
		PED::SET_PED_INTO_VEHICLE(playerPed, playerVeh, -1);
		int primaryColor, secondaryColor;
		VEHICLE::GET_VEHICLE_COLOURS(pedVeh, &primaryColor, &secondaryColor);
		VEHICLE::SET_VEHICLE_COLOURS(playerVeh, primaryColor, secondaryColor);
		if (VEHICLE::GET_IS_VEHICLE_PRIMARY_COLOUR_CUSTOM(pedVeh))
		{
			int r, g, b;
			VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(pedVeh, &r, &g, &b);
			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(playerVeh, r, g, b);
		}
		if (VEHICLE::GET_IS_VEHICLE_SECONDARY_COLOUR_CUSTOM(pedVeh))
		{
			int r, g, b;
			VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(pedVeh, &r, &g, &b);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(playerVeh, r, g, b);
		}
		if (VEHICLE::IS_THIS_MODEL_A_CAR(vehicleModelHash) || VEHICLE::IS_THIS_MODEL_A_BIKE(vehicleModelHash))
		{
			VEHICLE::SET_VEHICLE_MOD_KIT(playerVeh, 0);
			VEHICLE::SET_VEHICLE_WHEEL_TYPE(playerVeh, VEHICLE::GET_VEHICLE_WHEEL_TYPE(pedVeh));
			for (int i = 0; i <= 24; i++)
			{
				if (i > 16 && i < 23)
					VEHICLE::TOGGLE_VEHICLE_MOD(playerVeh, i, VEHICLE::IS_TOGGLE_MOD_ON(pedVeh, i));
				else
					VEHICLE::SET_VEHICLE_MOD(playerVeh, i, VEHICLE::GET_VEHICLE_MOD(pedVeh, i), VEHICLE::GET_VEHICLE_MOD_VARIATION(pedVeh, i));
			}
			int tireSmokeColor[3], pearlescentColor, wheelColor;
			VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR(pedVeh, &tireSmokeColor[0], &tireSmokeColor[1], &tireSmokeColor[2]);
			VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(playerVeh, tireSmokeColor[0], tireSmokeColor[1], tireSmokeColor[2]);
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(playerVeh, VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(pedVeh));
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(playerVeh, VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(pedVeh));
			VEHICLE::GET_VEHICLE_EXTRA_COLOURS(pedVeh, &pearlescentColor, &wheelColor);
			VEHICLE::SET_VEHICLE_EXTRA_COLOURS(playerVeh, pearlescentColor, wheelColor);
			if (VEHICLE::IS_VEHICLE_A_CONVERTIBLE(pedVeh, 0))
			{
				int convertableState = VEHICLE::GET_CONVERTIBLE_ROOF_STATE(pedVeh);
				if (convertableState == 0 || convertableState == 3 || convertableState == 5)
					VEHICLE::RAISE_CONVERTIBLE_ROOF(playerVeh, 1);
				else
					VEHICLE::LOWER_CONVERTIBLE_ROOF(playerVeh, 1);
			}
			for (int i = 0; i <= 3; i++)
			{
				VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(playerVeh, i, VEHICLE::_IS_VEHICLE_NEON_LIGHT_ENABLED(pedVeh, i));
			}
			for (int i = 0; i <= 11; i++)
			{
				if (VEHICLE::DOES_EXTRA_EXIST(pedVeh, i))
					VEHICLE::SET_VEHICLE_EXTRA(playerVeh, i, !VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(pedVeh, i));
			}
			if ((VEHICLE::GET_VEHICLE_LIVERY_COUNT(pedVeh) > 1) && VEHICLE::GET_VEHICLE_LIVERY(pedVeh) >= 0)
			{
				VEHICLE::SET_VEHICLE_LIVERY(playerVeh, VEHICLE::GET_VEHICLE_LIVERY(pedVeh));
			}
			int neonColor[3];
			VEHICLE::_GET_VEHICLE_NEON_LIGHTS_COLOUR(pedVeh, &neonColor[0], &neonColor[1], &neonColor[2]);
			VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(playerVeh, neonColor[0], neonColor[1], neonColor[2]);
			VEHICLE::SET_VEHICLE_WINDOW_TINT(playerVeh, VEHICLE::GET_VEHICLE_WINDOW_TINT(pedVeh));
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(playerVeh, VEHICLE::GET_VEHICLE_DIRT_LEVEL(pedVeh));
			VEHICLE::SET_VEHICLE_ENGINE_ON(playerVeh, TRUE, TRUE, 1);
		}
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehicleModelHash);
	}
	return pedVeh;
}

void boostVehicleStats(Vehicle vehicle)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0.0f);
	ENTITY::SET_ENTITY_INVINCIBLE(vehicle, TRUE);
	ENTITY::SET_ENTITY_PROOFS(vehicle, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
	VEHICLE::SET_VEHICLE_STRONG(vehicle, TRUE);
	VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_CAN_BREAK(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_ENGINE_CAN_DEGRADE(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_IS_STOLEN(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, FALSE);
	Hash vehicleModel = ENTITY::GET_ENTITY_MODEL(vehicle);
	if (VEHICLE::IS_THIS_MODEL_A_CAR(vehicleModel) || VEHICLE::IS_THIS_MODEL_A_BIKE(vehicleModel))
	{
		VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
		VEHICLE::SET_VEHICLE_HAS_STRONG_AXLES(vehicle, TRUE);
		VEHICLE::SET_VEHICLE_MOD(vehicle, 11, 3, FALSE);
		VEHICLE::SET_VEHICLE_MOD(vehicle, 12, 2, FALSE);
		VEHICLE::SET_VEHICLE_MOD(vehicle, 13, 2, FALSE);
		VEHICLE::SET_VEHICLE_MOD(vehicle, 15, 3, FALSE);
		VEHICLE::SET_VEHICLE_MOD(vehicle, 16, 4, FALSE);
		VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 18, TRUE);
	}
	VEHICLE::SET_VEHICLE_BODY_HEALTH(vehicle, 1000.0f);
}

void killSpaghettios(Ped ped)
{
	//for (int i = 0; i <= 1000; i++)
	//{
	//	Blip_t* blip = pBlipList->m_Blips[i].m_pBlip;
	//	if (blip)
	//	{
	//		if (blip->dwColor != BLIPCOLOR_BLUE)
	//		{
	//			if (blip->iIcon == BLIP_COP || blip->iIcon == BLIP_SPAGHETTIO || (blip->iIcon == BLIP_CIRCLE && blip->dwColor == BLIPCOLOR_RED))
	//			{
	//				static bool bShoot = FALSE;
	//				bShoot = !bShoot;
	//				if (bShoot)
	//				{
	//					static int weaponList[20] = { WEAPON_ADVANCEDRIFLE, WEAPON_APPISTOL, WEAPON_ASSAULTRIFLE, WEAPON_ASSAULTSMG, WEAPON_CARBINERIFLE, WEAPON_COMBATMG, WEAPON_COMBATPDW, WEAPON_COMBATPISTOL, WEAPON_HEAVYPISTOL, WEAPON_HEAVYSNIPER, WEAPON_MARKSMANRIFLE, WEAPON_MG, WEAPON_MICROSMG, WEAPON_PISTOL, WEAPON_PISTOL50, WEAPON_SMG, WEAPON_SNIPERRIFLE, WEAPON_SNSPISTOL, WEAPON_SPECIALCARBINE, WEAPON_MINIGUN };
	//					if (blip->fScale >= 1.0f)
	//						FIRE::ADD_OWNED_EXPLOSION(ped, blip->x, blip->y, blip->z, EXPLOSION_TANKER, 1000.0f, FALSE, TRUE, 0.0f);
	//					else
	//					{
	//						srand((unsigned int)time(NULL));
	//						GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(blip->x + 0.1f, blip->y, blip->z - 0.15f, blip->x - 0.1f, blip->y, blip->z + 1, 1000, TRUE, weaponList[rand() % (sizeof(weaponList) / 4)], ped, TRUE, TRUE, 1.0f);
	//					}
	//				}
	//			}
	//			if ((blip->dwColor == BLIPCOLOR_NONE && (blip->iIcon == BLIP_HELIBLADESENEMY || blip->iIcon == BLIP_COPHELICOPTER)) ||
	//				((blip->dwColor == BLIPCOLOR_RED || blip->dwColor == BLIPCOLOR_REDMISSION) && (blip->iIcon == BLIP_PLANE || blip->iIcon == BLIP_MOTORCYCLE || blip->iIcon == BLIP_CAR || blip->iIcon == BLIP_HELICOPTER || blip->iIcon == BLIP_JET2 || blip->iIcon == BLIP_HELICOPTERBLADES || blip->iIcon == BLIP_PLANEVEHICLE)))
	//			{
	//				FIRE::ADD_OWNED_EXPLOSION(ped, blip->x, blip->y, blip->z, EXPLOSION_TANKER, 1000.0f, FALSE, TRUE, 0.0f);
	//			}
	//		}
	//	}
	//}
}

void crashPlayer(Ped ped, Ped playerPed)
{
	std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	std::string text = " ~y~please be patient.";
	set_status_text(playerName + text);
	const int maxPeds = 70;
	Ped peds[maxPeds];
	if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(ENTITY::GET_ENTITY_COORDS(playerPed, FALSE).x, ENTITY::GET_ENTITY_COORDS(playerPed, FALSE).y, ENTITY::GET_ENTITY_COORDS(playerPed, FALSE).z, ENTITY::GET_ENTITY_COORDS(ped, FALSE).x, ENTITY::GET_ENTITY_COORDS(ped, FALSE).y, ENTITY::GET_ENTITY_COORDS(ped, FALSE).z, FALSE) > 250.0f)
	{
		for (int i = 0; i < maxPeds; i++)
		{
			peds[i] = PED::CLONE_PED(ped, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()), 1, 1);
			WAIT(0);
			ENTITY::ATTACH_ENTITY_TO_ENTITY(peds[i], ped, PED::GET_PED_BONE_INDEX(ped, 0x796e), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, FALSE, FALSE, FALSE, FALSE, 2, TRUE);
			WAIT(0);
			AI::CLEAR_PED_TASKS_IMMEDIATELY(ped);
			WAIT(0);
		}
		for (int i = 0; i < maxPeds; i++)
		{
			requestControl(peds[i]);
			WAIT(0);
			PED::DELETE_PED(&peds[i]);
			WAIT(0);
			AI::CLEAR_PED_TASKS_IMMEDIATELY(ped);
			WAIT(0);
			ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&peds[i]);
		}
	}
	else
	{
		std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
		std::string text = " ~r~please try crash again.";
		set_status_text(playerName + text);
	}
	return;
}

void upgrade(Vehicle vehicle, Ped ped)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 0, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 0) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 1, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 1) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 2, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 2) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 3, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 3) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 4, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 4) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 5, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 5) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 6, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 6) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 7, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 7) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 8, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 8) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 9, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 9) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 10, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 10) - 1, 10);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 11, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 11) - 1, 11);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 12, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 12) - 1, 12);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 13, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 13) - 1, 13);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 14, rand() % 32, 0);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 15, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 15) - 1, 1);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 16, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 16) - 1, 1);
	VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 18, TRUE);
	VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 20, TRUE);
	VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, 20, 20, 20);
	VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 22, TRUE);
	VEHICLE::SET_VEHICLE_WHEEL_TYPE(vehicle, rand() % 7);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 23, rand() % 23, 0);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 24, rand() % 23, 0);
	VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, NULL, rand() % 157);
	VEHICLE::SET_VEHICLE_WINDOW_TINT(vehicle, 1);
	VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(ped, 0), 0, 1);
	VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(ped, 0), 1, 1);
	VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(ped, 0), 2, 1);
	VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(ped, 0), 3, 1);
	VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(vehicle, 32, 32, 32);
	VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle, rand() % 5);
	int modIndex = VEHICLE::GET_VEHICLE_MOD(vehicle, 23);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 23, modIndex, TRUE);
	VEHICLE::SET_VEHICLE_MOD(vehicle, 24, modIndex, TRUE);
	VEHICLE::SET_VEHICLE_COLOURS(vehicle, 0, 0);
	VEHICLE::SET_VEHICLE_FIXED(vehicle);
	VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(vehicle);
	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_HAS_STRONG_AXLES(vehicle, TRUE);
	VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0.0f);
}

void nearbyPeds(Ped ped)
{
	const int amount = 20;
	const int arrSize = amount * 2 + 2;
	int peds[arrSize];
	peds[0] = amount;
	int count = PED::GET_PED_NEARBY_PEDS(ped, peds, -1);
	for (int i = 0; i < count; ++i)
		for (int index = 0; index < 32; index++)
		{
			int offsettedID = i * 2 + 2;
			if ((peds[offsettedID]) == PLAYER::GET_PLAYER_PED(index)) continue;
			{
				if (ENTITY::DOES_ENTITY_EXIST(peds[offsettedID]))
				{
					requestControl(peds[offsettedID]);

					if (featureSetPedCalm0)
					{
						PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peds[offsettedID], TRUE);
						PED::SET_PED_FLEE_ATTRIBUTES(peds[offsettedID], 0, 0);
						PED::SET_PED_COMBAT_ATTRIBUTES(peds[offsettedID], 17, 1);
					}

					if (featureSetPedCalm1)
					{
						AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peds[offsettedID], TRUE);
						PED::SET_PED_FLEE_ATTRIBUTES(peds[offsettedID], 0, 0);
						PED::SET_PED_COMBAT_ATTRIBUTES(peds[offsettedID], 17, 1);
					}

					if (featureKillPed)
					{
						FIRE::ADD_OWNED_EXPLOSION(ped, ENTITY::GET_ENTITY_COORDS(peds[offsettedID], FALSE).x, ENTITY::GET_ENTITY_COORDS(peds[offsettedID], FALSE).y, ENTITY::GET_ENTITY_COORDS(peds[offsettedID], FALSE).z, 37, 5, FALSE, TRUE, 0);
					}

					if (featureRichPed)
					{
						PED::SET_PED_MONEY(peds[offsettedID], rand() % 2000);
					}

					if (featureRevivePed && PED::IS_PED_DEAD_OR_DYING(peds[offsettedID], 1))
					{
						PED::REVIVE_INJURED_PED(peds[offsettedID]);
						PED::RESURRECT_PED(peds[offsettedID]);
					}

					if (featureEspPed)
					{
						Vector3 entityCoords = ENTITY::GET_ENTITY_COORDS(peds[offsettedID], TRUE);
						float x, y;
						if (GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(entityCoords.x, entityCoords.y, entityCoords.z, &x, &y))
						{
							Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(ped, TRUE);
							float distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(playerCoords.x, playerCoords.y, playerCoords.z, entityCoords.x, entityCoords.y, entityCoords.z, TRUE);
							if (distance < 100.0 && !ENTITY::IS_ENTITY_DEAD(peds[offsettedID]))
							{
								char text[250];
								sprintf_s(text, "~y~Neutral ~y~%.02f", distance);
								UI::SET_TEXT_OUTLINE();
								UI::SET_TEXT_FONT(0);
								UI::SET_TEXT_SCALE(0.2f, 0.2f);
								UI::SET_TEXT_COLOUR(255, 0, 0, 255);
								UI::SET_TEXT_WRAP(0.0f, 1.0f);
								UI::SET_TEXT_CENTRE(TRUE);
								UI::_SET_TEXT_ENTRY("STRING");
								UI::_ADD_TEXT_COMPONENT_STRING(text);
								UI::_DRAW_TEXT(x, y);
							}
						}
					}
				}
			}
		}
}

void nearbyVehicles(Ped ped)
{
	const int amount = 20;
	const int arrSize = amount * 2 + 2;
	int vehicles[arrSize];
	vehicles[0] = amount;
	int count = PED::GET_PED_NEARBY_VEHICLES(ped, vehicles);
	for (int i = 0; i < count; ++i)
	{
		int offsettedID = i * 2 + 2;
		if (ENTITY::DOES_ENTITY_EXIST(vehicles[offsettedID]))
		{
			if ((vehicles[offsettedID]) == ped) continue;

			{
				upgrade(vehicles[offsettedID], ped);
			}
		}
	}
}

void hancock(Ped ped)
{
	PED::SET_PED_TO_RAGDOLL_WITH_FALL(ped, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	if (GetAsyncKeyState(0xDB))
	{
		WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, 0xFBAB5776, 1, FALSE);
		applyForceToEntity(ped, 0.0f, 3.0f, 0.0f);
	}

	if (GetAsyncKeyState(0xDD))
	{
		WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, 0xFBAB5776, 1, FALSE);
		applyForceToEntity(ped, 0.0f, 0.0f, 2.0f);
		AI::TASK_PARACHUTE(ped, TRUE);
	}
}

void bodyguard(Ped ped, Player player)
{
	if (ENTITY::DOES_ENTITY_EXIST(ped))
	{
		GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(TRUE, "FMMC_KEY_TIP8", "", "a_f_y_topless_01", "", "", "", 20);
		while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(0);
		if (!STREAMING::IS_MODEL_IN_CDIMAGE(GAMEPLAY::GET_HASH_KEY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())))
		{
			return;
		}
		else
		{
			Vector3 get_entity_coords = ENTITY::GET_ENTITY_COORDS(ped, 0);
			Ped get_ped_group_index = PED::GET_PED_GROUP_INDEX(ped);
			char* get_onscreen_keyboard_result = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
			Hash get_hash_key = GAMEPLAY::GET_HASH_KEY(get_onscreen_keyboard_result);
			STREAMING::REQUEST_MODEL(get_hash_key);
			while (!STREAMING::HAS_MODEL_LOADED(get_hash_key))
				WAIT(0);
			Ped create_ped = PED::CREATE_PED(1, get_hash_key, get_entity_coords.x, get_entity_coords.y + 5.0f, get_entity_coords.z, ENTITY::GET_ENTITY_HEADING(ped), TRUE, TRUE);
			WAIT(0);
			PED::SET_PED_CAN_RAGDOLL(create_ped, FALSE);
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(create_ped, FALSE);
			PED::SET_PED_AS_GROUP_MEMBER(create_ped, get_ped_group_index);
			PED::SET_PED_COMBAT_RANGE(create_ped, 500);
			PED::SET_PED_ALERTNESS(create_ped, 100);
			PED::SET_PED_ACCURACY(create_ped, 100);
			PED::SET_PED_CAN_SWITCH_WEAPON(create_ped, 1);
			PED::SET_PED_SHOOT_RATE(create_ped, 200);
			PED::SET_PED_KEEP_TASK(create_ped, TRUE);
			AI::TASK_COMBAT_HATED_TARGETS_AROUND_PED(create_ped, 5000.f, 0);
			WAIT(0);
			for (int a = 0; a < sizeof(weaponNames) / sizeof(weaponNames[0]); a++)
			{
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(create_ped, GAMEPLAY::GET_HASH_KEY((char*)weaponNames[a]), 1000, 0);
			}
			WAIT(0);
			PED::SET_PED_CAN_SWITCH_WEAPON(create_ped, TRUE);
			PED::SET_PED_KEEP_TASK(create_ped, TRUE);
			PED::SET_PED_GENERATES_DEAD_BODY_EVENTS(create_ped, TRUE);
			// misc invincible bodyguard
			if (featurePedestrianInvincibleBodyguardUpdate)
			{
				if (!featurePedestrianInvincibleBodyguard)
					ENTITY::SET_ENTITY_INVINCIBLE(create_ped, FALSE);
				featurePedestrianInvincibleBodyguardUpdate = FALSE;
			}
			if (featurePedestrianInvincibleBodyguard)
			{
				ENTITY::SET_ENTITY_INVINCIBLE(create_ped, TRUE);
			}
			WAIT(0);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(get_hash_key);
		}

		std::string playerName = PLAYER::GET_PLAYER_NAME(player);
		std::string text = " ~g~A bodyguard was spawned for~s~ ";
		set_status_text(text + playerName);
	}
}

void resetMenu()
{
	if (GetAsyncKeyState(VK_F12))
	{
		featurePlayerESP =
			featurePlayerESPBoxes =
			featurePlayerESPTrace =
			featurePlayerInvincible =
			featurePlayerInvincibleUpdated =
			featurePlayerNeverWanted =
			featurePlayerIgnored =
			featurePlayerIgnoredUpdated =
			featurePlayerUnlimitedAbility =
			featurePlayerNoNoise =
			featurePlayerNoNoiseUpdated =
			featurePlayerFastSwim =
			featurePlayerFastSwimUpdated =
			featurePlayerFastRun =
			featurePlayerFastRunUpdated =
			featurePlayerSuperJump =
			playerInvisible =
			playerInvisibleUpdated =
			cashAdd =
			featureNightVision =
			featureNightVisionUpdated =
			featureThermalVision =
			featureThermalVisionUpdated =
			featurePlayerUltraJump =
			featurePlayerSuperRun =
			featurePlayerBloodDamage =
			featurePlayerAutoHeal =
			featurePlayerHancock =
			featureAirBrake =
			featureWeaponTriggerbot =
			featureWeaponNoReload =
			featureWeaponNoReloadUpdated =
			featureWeaponFireAmmo =
			featureWeaponExplosiveAmmo =
			featureWeaponExplosiveMelee =
			featureWeaponFirework =
			featureWeaponVehicleRocket =
			featureWeaponTank =
			featureWeaponRailGun =
			featureWeaponHeavySniper =
			featureWeaponMinigun =
			featureWeaponPlaneRocket =
			featureWeaponGrenadeLauncher =
			featureWeaponAirstrikeRocket =
			featureWeaponSavage =
			featureWeaponSpaceRocket =
			featureWeaponPropMoneyBag =
			featureWeaponPropCokeBlock =
			featureWeaponPropWeed =
			featureWeaponPropWeedPallet =
			featureWeaponPropXmasTree =
			featureWeaponPropXmas =
			featureWeaponPropDildo =
			weaponOnePunchKill =
			weaponOnePunchKillUpdated =
			featureWeaponDamageModifier =
			featureWeaponDamageModifierUpdate =
			weaponForceGun =
			weaponGravityGun =
			gravityLock =
			weaponSpaghettios =
			weaponDeleteObjects =
			featureVehInvincible =
			featureVehInvincibleUpdated =
			featureVehInvisible =
			featureVehInvisibleUpdated =
			featureVehInvincibleWheels =
			featureVehInvincibleWheelsUpdated =
			featureVehSeatbelt =
			featureVehSeatbeltUpdated =
			featureVehSpeedBoost =
			featureVehWrapInSpawned =
			featureVehRainbow =
			featureVehDrift =
			featureVehGrip =
			featureVehSlammed =
			featureVehSlammedUpdated =
			featureVehAutoFix =
			featureVehDamageModifier =
			featureVehDamageModifierUpdate =
			featureWorldMoonGravity =
			worldClearVehicles =
			worldClearObjects =
			worldClearPeds =
			worldClearCops =
			worldClearProjectiles =
			featureMiscGlobeRadio =
			featureMiscGlobeRadioUpdated =
			featureMiscLockRadio =
			featureMiscHideHud =
			featureMiscRestrictedZones =
			featurePedestrianInvincibleBodyguard =
			featurePedestrianInvincibleBodyguardUpdate =
			featureNearbyPeds =
			featureSetPedCalm0 =
			featureSetPedCalm1 =
			featureKillPed =
			featureRichPed =
			featureRevivePed =
			featureEspPed =
			/*featurePlayerBlips =
			featurePlayerHeadDisplay =
			featurePlayerBlipCone =
			featurePlayerNotifications =
			featureShowVoiceChatSpeaker =*/
			featureSkinChanger = 0;
		set_creator_text("~h~~g~iTzNeutron reset successfully.");
	}
}

void superHotkey(Ped ped, Player player)
{
	//if (GetAsyncKeyState(VK_F6))
	//{
	//	set_creator_text("~h~~g~Super hotkey F4.");
	//	std::string playerName1 = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	//	std::string text1 = " ~g~your wanted level is down.";
	//	set_status_text(playerName1 + text1);
	//	std::string playerName2 = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	//	std::string text2 = " ~g~your player was fixed.";
	//	set_status_text(playerName2 + text2);
	//	if (PLAYER::GET_PLAYER_WANTED_LEVEL(player) > 0)
	//	{
	//		PLAYER::SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) - 1, 0);
	//		PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, 0);
	//	}
	//	ENTITY::SET_ENTITY_HEALTH(ped, ENTITY::GET_ENTITY_MAX_HEALTH(ped));
	//	PED::ADD_ARMOUR_TO_PED(ped, PLAYER::GET_PLAYER_MAX_ARMOUR(player) - PED::GET_PED_ARMOUR(ped));
	//	if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0))
	//	{
	//		Vehicle playerVeh = PED::GET_VEHICLE_PED_IS_USING(ped);
	//		if (ENTITY::DOES_ENTITY_EXIST(playerVeh) && !ENTITY::IS_ENTITY_DEAD(playerVeh))
	//			VEHICLE::SET_VEHICLE_FIXED(playerVeh);
	//	}
	//	Entity e = ped;
	//	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(ped);
	//	Vector3 coords;
	//	bool success = FALSE;
	//	for (int i = 0; i <= 1000; i++)
	//	{
	//		Blip_t* blip = pBlipList->m_Blips[i].m_pBlip;
	//		if (blip)
	//		{
	//			if ((blip->dwColor == BLIPCOLOR_MISSION && blip->iIcon == BLIP_CIRCLE) ||
	//				(blip->dwColor == BLIPCOLOR_YELLOWMISSION && blip->iIcon == BLIP_CIRCLE) ||
	//				(blip->dwColor == BLIPCOLOR_YELLOWMISSION2 && (blip->iIcon == BLIP_CIRCLE || blip->iIcon == BLIP_DOLLARSIGN)) ||
	//				(blip->dwColor == BLIPCOLOR_NONE && blip->iIcon == BLIP_RACEFLAG) ||
	//				(blip->dwColor == BLIPCOLOR_GREEN && blip->iIcon == BLIP_CIRCLE) ||
	//				(blip->iIcon == BLIP_SPECIALCRATE))
	//			{
	//				if (veh)
	//					e = veh;
	//				requestControl(veh);
	//				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, blip->x, blip->y, blip->z, FALSE, FALSE, FALSE);
	//				break;
	//			}
	//		}
	//	}
	//	requestControl(veh);
	//	bool blipFound = FALSE;
	//	// search for marker blip
	//	int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
	//	for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
	//	{
	//		if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4)
	//		{
	//			coords = UI::GET_BLIP_INFO_ID_COORD(i);
	//			blipFound = TRUE;
	//			break;
	//		}
	//	}
	//	if (blipFound)
	//	{
	//		// load needed map region and check height levels for ground existence
	//		bool groundFound = FALSE;
	//		static float groundCheckHeight[] = {
	//			100.0, 300.0, 50.0, 0.0, 300.0, 250.0, 300.0, 350.0, 400.0,
	//			450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
	//		};
	//		for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++)
	//		{
	//			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, groundCheckHeight[i], 0, 0, 1);
	//			WAIT(100);
	//			if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, groundCheckHeight[i], &coords.z))
	//			{
	//				groundFound = TRUE;
	//				coords.z += 3.0;
	//				break;
	//			}
	//		}
	//		// if ground not found then set Z in air and give player a parachute
	//		if (!groundFound)
	//		{
	//			coords.z = 1000.0;
	//		}
	//		success = TRUE;
	//	}
	//	else
	//	{
	//		std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	//		std::string text = " ~r~your map marker isn't set.";
	//		set_status_text(playerName + text);
	//	}
	//	// set player pos
	//	if (success)
	//	{
	//		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, coords.z, 0, 0, 1);
	//		WAIT(0);
	//		std::string playerName3 = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	//		std::string text3 = " ~g~your teleport was complete.";
	//		set_status_text(playerName3 + text3);
	//	}
	//}
}

void VectorToFloat(Vector3 unk, float* Out)
{
	Out[0] = unk.x;
	Out[1] = unk.y;
	Out[2] = unk.z;
}

void offset_from_entity(int entity, float X, float Y, float Z, float* Out)
{
	VectorToFloat(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(entity, X, Y, Z), Out);
}

void PlaceObject(Hash hash, float x, float y, float z, float pitch, float roll, float yaw)
{
	if (STREAMING::IS_MODEL_IN_CDIMAGE(hash) && STREAMING::IS_MODEL_VALID(hash))
	{
		STREAMING::REQUEST_MODEL(hash);
		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(0);
		Object object = OBJECT::CREATE_OBJECT(hash, x, y, z, TRUE, TRUE, FALSE);
		ENTITY::SET_ENTITY_ROTATION(object, pitch, roll, yaw, 2, TRUE);
		ENTITY::FREEZE_ENTITY_POSITION(object, TRUE);
		ENTITY::SET_ENTITY_LOD_DIST(object, 696969);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
	}
}

void set_massacre_mode()
{
	float tempCoords1[3];
	DWORD tempHash;
	DWORD tempPed = PLAYER::PLAYER_PED_ID();
	DWORD tempVehicle;
	bool tick = 0;

	offset_from_entity(tempPed, GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f), tempCoords1);

	// Kick away naughty players
	for (int i = 0; i <= 32; i++)
	{
		if (PLAYER::IS_PLAYER_TARGETTING_ENTITY(i, tempPed) || PLAYER::IS_PLAYER_FREE_AIMING_AT_ENTITY(i, tempPed))
		{
			if (!tick)
			{
				set_status_text("~h~You~s~ are being aimed at. Taking care of it!");
				tick = TRUE;
			}
			ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::GET_PLAYER_PED(i), 1, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 1, 1, 1, 1, 0, 1);
		}
	}

	// Kick away nearest vehicle
	tempVehicle = VEHICLE::GET_CLOSEST_VEHICLE(tempCoords1[0], tempCoords1[1], tempCoords1[2], 400.0f, 0, 4);
	if (ENTITY::DOES_ENTITY_EXIST(tempVehicle)) ENTITY::APPLY_FORCE_TO_ENTITY(tempVehicle, 1, 5.0f, 5.0f, 5.0f, 5.0f, 13.0f, 6.5f, 1, 1, 1, 1, 0, 1);

	// Exploding vehicles
	switch (GAMEPLAY::GET_RANDOM_INT_IN_RANGE(1, 10))
	{
	case 1: tempHash = 3945366167; break;
	case 2: tempHash = 92612664; break;
	case 3: tempHash = 1488164764; break;
	case 4: tempHash = 117401876; break;
	case 5: tempHash = 408192225; break;
	case 6: tempHash = 3863274624; break;
	case 7: tempHash = 165154707; break;
	case 8: tempHash = 1011753235; break;
	case 9: tempHash = 941800958; break;
	case 10: tempHash = 970385471; break;
	}

	tempVehicle = VEHICLE::CREATE_VEHICLE(tempHash, tempCoords1[0], tempCoords1[1], tempCoords1[2], GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.0f, 360.0f), 1, 1);
	ENTITY::APPLY_FORCE_TO_ENTITY(tempVehicle, 1, 5.0f, 5.0f, 5.0f, 5.0f, 13.0f, 6.5f, 1, 1, 1, 1, 0, 1);
	NETWORK::NETWORK_EXPLODE_VEHICLE(tempVehicle, 1, 1, 1);

	// Dying peds
	switch (GAMEPLAY::GET_RANDOM_INT_IN_RANGE(1, 10))
	{
	case 1: tempHash = 1682622302; break;
	case 2: tempHash = 113504370; break;
	case 3: tempHash = 4244282910; break;
	case 4: tempHash = 808859815; break;
	case 5: tempHash = 1004114196; break;
	case 6: tempHash = 3696858125; break;
	case 7: tempHash = 921110016; break;
	case 8: tempHash = 880829941; break;
	case 9: tempHash = 1462895032; break;
	case 10: tempHash = 773063444; break;
	}

	offset_from_entity(tempPed, GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f), tempCoords1);
	tempPed = PED::CREATE_PED(22, tempHash, tempCoords1[0], tempCoords1[1], tempCoords1[2], GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.0f, 360.0f), 1, 1);
	ENTITY::APPLY_FORCE_TO_ENTITY(tempPed, 1, 5.0f, 5.0f, 5.0f, 5.0f, 13.0f, 6.5f, 1, 1, 1, 1, 0, 1);
	PED::EXPLODE_PED_HEAD(tempPed, 324506233);
	FIRE::ADD_EXPLOSION(tempCoords1[0], tempCoords1[1], tempCoords1[2], 26, 4.0f, 1, 0, 3.0f);

	// Water hydrants
	offset_from_entity(tempPed, GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f), tempCoords1);
	FIRE::ADD_EXPLOSION(tempCoords1[0], tempCoords1[1], tempCoords1[2], 13, 8.0f, 1, 0, 3.0f);

	// Zombies
	offset_from_entity(tempPed, GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f), tempCoords1);
	tempPed = PED::CREATE_PED(22, -1404353274, tempCoords1[0], tempCoords1[1], tempCoords1[2], GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.0f, 360.0f), 1, 1);
	NETWORK::PED_TO_NET(tempPed);
	PED::SET_PED_COMBAT_ABILITY(tempPed, 100);
	AI::TASK_COMBAT_HATED_TARGETS_AROUND_PED(tempPed, 400.0f, 0);
}

int travelSpeed = 0;
Vector3 curLocation;
Vector3 curRotation;
float curHeading;
void airbrake(Ped ped)
{
	float rotationSpeed = 2.5;
	float forwardPush;

	switch (travelSpeed)
	{
	case 0:
		rotationSpeed = 0.8f;
		forwardPush = 0.2f;
		break;
	case 1:
		forwardPush = 1.8f;
		break;
	case 2:
		forwardPush = 3.6f;
		break;
	}

	float xVect = forwardPush * -1.0f;
	float yVect = forwardPush;

	int moveUpKey = IsKeyPressed(0x51);//q
	int moveDownKey = IsKeyPressed(0x5A);//z
	int moveForwardKey = IsKeyPressed(0x57);//w
	int moveBackKey = IsKeyPressed(0x53);//s
	int rotateLeftKey = IsKeyPressed(0x41);//a
	int rotateRightKey = IsKeyPressed(0x44);//d

	//Airbrake controls vehicle if occupied
	Entity target = ped;
	if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0))
	{
		target = PED::GET_VEHICLE_PED_IS_USING(ped);
	}

	BOOL xBoolParam = 1;
	BOOL yBoolParam = 1;
	BOOL zBoolParam = 1;

	ENTITY::SET_ENTITY_VELOCITY(target, 0.0f, 0.0f, 0.0f);
	ENTITY::SET_ENTITY_ROTATION(target, 0.0f, 0.0f, 0.0f, 0, FALSE);

	if (!PED::IS_PED_IN_ANY_VEHICLE(ped, 0))
	{
		AI::TASK_PLAY_ANIM(ped, "amb@world_human_stand_impatient@male@no_sign@base", "base", 8.0f, 0.0f, -1, 9, 0, 0, 0, 0);
	}
	if (IsKeyPressed(0x10))//shift
	{
		travelSpeed++;
		if (travelSpeed > 2)
		{
			travelSpeed = 0;
		}
	}
	if (moveUpKey)
	{
		curLocation.z += forwardPush / 2;
	}
	else if (moveDownKey)
	{
		curLocation.z -= forwardPush / 2;
	}
	if (moveForwardKey)
	{
		curLocation.x += xVect;
		curLocation.y += yVect;
	}
	else if (moveBackKey)
	{
		curLocation.x -= xVect;
		curLocation.y -= yVect;
	}
	if (rotateLeftKey)
	{
		curHeading += rotationSpeed;
	}
	else if (rotateRightKey)
	{
		curHeading -= rotationSpeed;
	}

	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(target, curLocation.x, curLocation.y, curLocation.z, xBoolParam, yBoolParam, zBoolParam);
	ENTITY::SET_ENTITY_HEADING(target, curHeading - rotationSpeed);
	//curLocation = ENTITY::GET_ENTITY_COORDS(ped, 0);
	//curRotation = ENTITY::GET_ENTITY_ROTATION(ped, 0);
	//curHeading = ENTITY::GET_ENTITY_HEADING(ped);
}

Player playerId;
//playerinfo playerdb[MAX_PLAYERS];
Vehicle ownedveh;
void updateStuff()
{
	//if (NETWORK::NETWORK_IS_SESSION_STARTED())
	//{
	//	if (playerWasDisconnected) {
	//		NETWORK::NETWORK_SET_FRIENDLY_FIRE_OPTION(1);
	//		playerId = PLAYER::PLAYER_ID();
	//		PED::SET_CAN_ATTACK_FRIENDLY(playerId, 1, 1);
	//	}

	//	std::string voice_status_msg = "Currently Talking:";
	//	bool isVoiceChatRunning = FALSE;

	//	for (int i = 0; i < MAX_PLAYERS; i++)
	//	{
	//		if (featureShowVoiceChatSpeaker && isVoiceChatRunning)
	//			update_status_text();

	//		if (NETWORK::NETWORK_IS_PLAYER_CONNECTED(i))
	//		{
	//			std::string name = (char*)PLAYER::GET_PLAYER_NAME(i);

	//			if (featureShowVoiceChatSpeaker && NETWORK::NETWORK_IS_PLAYER_TALKING(i))
	//			{
	//				if (!isVoiceChatRunning)
	//					isVoiceChatRunning = TRUE;
	//				voice_status_msg += "~n~" + name;
	//			}

	//			if (i != playerId)
	//			{
	//				Ped pedId = PLAYER::GET_PLAYER_PED(i);
	//				unsigned int headDisplayId = UI::_0xBFEFE3321A3F5015(pedId, (char*)"", 0, 0, (char*)"", 0); // CREATE_PED_HEAD_DISPLAY

	//				if (UI::_0x4E929E7A5796FD26(headDisplayId))
	//				{
	//					playerdb[i].head = headDisplayId;
	//					if (featurePlayerHeadDisplay)
	//						UI::SET_MP_GAMER_TAG_NAME(headDisplayId, (Any*)name.c_str()); // SET_HEAD_DISPLAY_STRING
	//					UI::_0x63BB75ABEDC1F6A0(headDisplayId, 0, 1); // SET_HEAD_DISPLAY_FLAG
	//				}

	//				if (playerWasDisconnected || !UI::DOES_BLIP_EXIST(playerdb[i].blip))
	//				{
	//					if (featurePlayerBlips)
	//					{
	//						playerdb[i].blip = UI::ADD_BLIP_FOR_ENTITY(pedId);
	//						UI::SET_BLIP_COLOUR(playerdb[i].blip, 0);
	//						UI::SET_BLIP_SCALE(playerdb[i].blip, 0.8f);
	//						if (featurePlayerBlipCone)
	//							UI::SET_BLIP_SHOW_CONE(playerdb[i].blip, 1);
	//						UI::SET_BLIP_NAME_TO_PLAYER_NAME(playerdb[i].blip, i);
	//						UI::SET_BLIP_CATEGORY(playerdb[i].blip, 7);
	//					}

	//					if (playerWasDisconnected || playerdb[i].name != name) // Making sure the player wasn't already here and only changed his ped (ex. skin change)
	//					{
	//						if (featurePlayerNotifications)
	//						{
	//							std::string msg = "<C>" + name + "</C> joined.";
	//							show_notification((char*)msg.c_str());
	//						}

	//						PED::SET_CAN_ATTACK_FRIENDLY(i, 1, 1);
	//						playerdb[i].name = name;
	//					}
	//					playerdb[i].ped = pedId;
	//				}
	//			}
	//		}
	//		else if (playerdb[i].name != "")
	//		{
	//			if (featurePlayerNotifications)
	//			{
	//				std::string msg = "<C>" + playerdb[i].name + "</C> left.";
	//				show_notification((char*)msg.c_str());
	//			}

	//			if (UI::IS_MP_GAMER_TAG_ACTIVE(playerdb[i].head))
	//				UI::SET_MP_GAMER_TAG_NAME(playerdb[i].head, (Any*)"");
	//			if (UI::DOES_BLIP_EXIST(playerdb[i].blip))
	//				UI::REMOVE_BLIP(playerdb[i].blip);

	//			playerdb[i].name = "";
	//		}
	//	}
	//	playerWasDisconnected = FALSE;

	//	if (isVoiceChatRunning)
	//		set_status_text(voice_status_msg);
	//}
	//else
	//{
	//	playerWasDisconnected = TRUE;
	//}
}

void esp(Ped ped)
{
	// player esp
	if (featurePlayerESP)
	{
		for (int index = 0; index < 32; index++)
		{
			Player playerOnline = PLAYER::GET_PLAYER_PED(index);
			BOOL bPlayerOnlineExists = ENTITY::DOES_ENTITY_EXIST(playerOnline);
			Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
			Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
			Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(ped, FALSE);
			if (playerOnline == ped) continue;
			char* name = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
			int health = ENTITY::GET_ENTITY_HEALTH(playerOnline);
			float distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(playerCoords.x, playerCoords.y, playerCoords.z, playerOnlineCoords.x, playerOnlineCoords.y, playerOnlineCoords.z, TRUE);
			int armor = PED::GET_PED_ARMOUR(playerOnline);
			char text[250];
			if (distance < 5000.0f && bPlayerOnlineExists)
			{
				float xa;
				float ya;
				BOOL screenCoords = GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(playerOnlineCoords.x, playerOnlineCoords.y, playerOnlineCoords.z, &xa, &ya);
				if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, playerOnline, 17) && bPlayerOnlineExists)
				{
					sprintf_s(text, "%s ~y~%.02f\n~g~%d ~b~%d", name, distance, health, armor);
					UI::SET_TEXT_FONT(0);
					UI::SET_TEXT_OUTLINE();
					UI::SET_TEXT_SCALE(0.2f, 0.2f);
					UI::SET_TEXT_COLOUR(0, 255, 0, 255);
					UI::SET_TEXT_WRAP(0.0f, 1.0f);
					UI::SET_TEXT_CENTRE(TRUE);
					UI::_SET_TEXT_ENTRY("STRING");
					UI::_ADD_TEXT_COMPONENT_STRING(text);
				}
				else
				{
					sprintf_s(text, "%s\n ~y~%.02f", name, distance);
					UI::SET_TEXT_FONT(0);
					UI::SET_TEXT_OUTLINE();
					UI::SET_TEXT_SCALE(0.12f, 0.12f);
					UI::SET_TEXT_COLOUR(255, 0, 0, 75);
					UI::SET_TEXT_WRAP(0.0f, 1.0f);
					UI::SET_TEXT_CENTRE(TRUE);
					UI::_SET_TEXT_ENTRY("STRING");
					UI::_ADD_TEXT_COMPONENT_STRING(text);
				}
				UI::_DRAW_TEXT(xa, ya);
			}
		}
	}

	// player esp boxes
	if (featurePlayerESPBoxes)
	{
		for (int index = 0; index < 32; index++)
		{
			Player playerOnline = PLAYER::GET_PLAYER_PED(index);
			BOOL bPlayerOnlineExists = ENTITY::DOES_ENTITY_EXIST(playerOnline);
			Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
			if (playerOnline == ped) continue;
			if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, playerOnline, 17) && bPlayerOnlineExists)
			{
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 0, 255, 0, 255);
			}
			else
			{
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x + 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y - 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
				GRAPHICS::DRAW_LINE(playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z - 0.75f, playerOnlineCoords.x - 0.5f, playerOnlineCoords.y + 0.5f, playerOnlineCoords.z + 0.75f, 255, 0, 0, 75);
			}
		}
	}

	if (featurePlayerESPTrace)
	{
		for (int index = 0; index < 32; index++)
		{
			Player playerOnline = PLAYER::GET_PLAYER_PED(index);
			BOOL bPlayerOnlineExists = ENTITY::DOES_ENTITY_EXIST(playerOnline);
			Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
			if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, playerOnline, 17) && bPlayerOnlineExists)
			{
				GRAPHICS::DRAW_LINE(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED(index), 0.0f, 0.0f, 0.0f).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED(index), 0.0f, 0.0f, 0.0f).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED(index), 0.0f, 0.0f, 0.0f).z, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0f, 0.0f, 0.0f).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0f, 0.0f, 0.0f).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0f, 0.0f, 0.0f).z, 0, 255, 0, 255);
			}
			else
			{
				GRAPHICS::DRAW_LINE(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED(index), 0.0f, 0.0f, 0.0f).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED(index), 0.0f, 0.0f, 0.0f).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED(index), 0.0f, 0.0f, 0.0f).z, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0f, 0.0f, 0.0f).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0f, 0.0f, 0.0f).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0f, 0.0f, 0.0f).z, 255, 0, 0, 75);
			}
		}
	}
}
#pragma endregion
// Updates all features that can be turned off by the game, being called each game frame
void update_features()
{
	update_status_text();

	// changing player model if died/arrested while being in another skin, since it can cause inf loading loop
	if (featureSkinChanger)
	{
		check_player_model();
	}

	// wait until player is ready, basicly to prevent using the trainer while player is dead or arrested
	while (ENTITY::IS_ENTITY_DEAD(PLAYER::PLAYER_PED_ID()) || PLAYER::IS_PLAYER_BEING_ARRESTED(PLAYER::PLAYER_ID(), TRUE))
		WAIT(0);

	// common variables
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(playerPed);

	if (bPlayerExists)
	{
		esp(playerPed);
	}

	if (bPlayerExists)
	{
		updateStuff();
	}

	if (cashAdd && (GetAsyncKeyState(VK_SUBTRACT) && 0x8000))
	{
		cash(playerPed);
	}

	// player invincible
	if (featurePlayerInvincibleUpdated)
	{
		if (bPlayerExists && !featurePlayerInvincible)
			PLAYER::SET_PLAYER_INVINCIBLE(player, FALSE);
		featurePlayerInvincibleUpdated = FALSE;
	}
	if (featurePlayerInvincible)
	{
		if (bPlayerExists)
			PLAYER::SET_PLAYER_INVINCIBLE(player, TRUE);
	}

	if (playerInvisibleUpdated && bPlayerExists)
	{
		ENTITY::RESET_ENTITY_ALPHA(playerPed);
		ENTITY::SET_ENTITY_VISIBLE(playerPed, TRUE, 1);
		GRAPHICS::SET_ENTITY_ICON_VISIBILITY(playerPed, TRUE);
	}
	if (playerInvisible && bPlayerExists)
	{
		ENTITY::SET_ENTITY_ALPHA(playerPed, 0, FALSE);
		ENTITY::SET_ENTITY_VISIBLE(playerPed, FALSE, 1);
		GRAPHICS::SET_ENTITY_ICON_VISIBILITY(playerPed, FALSE);
	}

	// player never wanted
	if (featurePlayerNeverWanted)
	{
		if (bPlayerExists)
			PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);
	}

	// police ignore player
	if (featurePlayerIgnoredUpdated)
	{
		if (bPlayerExists)
			PLAYER::SET_POLICE_IGNORE_PLAYER(player, featurePlayerIgnored);
		featurePlayerIgnoredUpdated = FALSE;
	}

	// player special ability
	if (featurePlayerUnlimitedAbility)
	{
		if (bPlayerExists)
			PLAYER::SPECIAL_ABILITY_FILL_METER(player, 1);
	}

	// player no noise
	if (featurePlayerNoNoiseUpdated)
	{
		if (bPlayerExists && !featurePlayerNoNoise)
			PLAYER::SET_PLAYER_NOISE_MULTIPLIER(player, 1.0f);
		featurePlayerNoNoiseUpdated = FALSE;
	}
	if (featurePlayerNoNoise)
	{
		PLAYER::SET_PLAYER_NOISE_MULTIPLIER(player, 0.0f);
	}

	// player fast swim
	if (featurePlayerFastSwimUpdated)
	{
		if (bPlayerExists && !featurePlayerFastSwim)
			PLAYER::SET_SWIM_MULTIPLIER_FOR_PLAYER(player, 1.0f);
		featurePlayerFastSwimUpdated = FALSE;
	}
	if (featurePlayerFastSwim)
	{
		PLAYER::SET_SWIM_MULTIPLIER_FOR_PLAYER(player, 1.49f);
	}

	// player fast run
	if (featurePlayerFastRunUpdated)
	{
		if (bPlayerExists && !featurePlayerFastRun)
			PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 1.0f);
		featurePlayerFastRunUpdated = FALSE;
	}
	if (featurePlayerFastRun)
	{
		PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 1.49f);
	}

	// player super jump
	if (featurePlayerSuperJump)
	{
		if (bPlayerExists)
			GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(player);
	}

	if (featureNightVisionUpdated)
	{
		GRAPHICS::SET_NIGHTVISION(featureNightVision);
		featureNightVisionUpdated = FALSE;
	}

	if (featureThermalVisionUpdated)
	{
		GRAPHICS::SET_SEETHROUGH(featureThermalVision);
		featureThermalVisionUpdated = FALSE;
	}

	// player super jump
	if (featurePlayerUltraJump)
	{
		GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(player);
		if (PED::IS_PED_JUMPING(playerPed))
		{
			applyForceToEntity(playerPed, 0.0f, 0.0f, 5.0f);
		}
	}

	// player super run
	if (featurePlayerSuperRun)
	{
		if (AI::IS_PED_RUNNING(playerPed) || AI::IS_PED_SPRINTING(playerPed) && !PED::IS_PED_RUNNING_RAGDOLL_TASK(playerPed))
		{
			applyForceToEntity(playerPed, 0.0f, 1.0f, 0.0f);
		}
	}

	// player blood damage
	if (featurePlayerBloodDamage)
	{
		if (bPlayerExists)
			PED::CLEAR_PED_BLOOD_DAMAGE(playerPed);
		PED::RESET_PED_VISIBLE_DAMAGE(playerPed);
	}

	if (featurePlayerAutoHeal && ENTITY::GET_ENTITY_HEALTH(playerPed) < 120)
	{
		ENTITY::SET_ENTITY_HEALTH(playerPed, 200);
	}

	if (featurePlayerHancock)
	{
		hancock(playerPed);
	}

	if (featureAirBrake)
	{
		airbrake(playerPed);
	}

	// weapon triggerbot
	Entity entity;
	if (featureWeaponTriggerbot && bPlayerExists && PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &entity) && ENTITY::IS_ENTITY_A_PED(entity) && !ENTITY::IS_ENTITY_DEAD(entity) && ENTITY::GET_ENTITY_ALPHA(entity) == 255)
	{
		PED::SET_PED_SHOOTS_AT_COORD(PLAYER::PLAYER_PED_ID(), PED::GET_PED_BONE_COORDS(entity, 0x796e, 0.0f, 0.0f, 0.0f).x, PED::GET_PED_BONE_COORDS(entity, 0x796e, 0.0f, 0.0f, 0.0f).y, PED::GET_PED_BONE_COORDS(entity, 0x796e, 0.0f, 0.0f, 0.0f).z, TRUE);
	}

	// weapon
	if (featureWeaponFireAmmo)
	{
		if (bPlayerExists)
			GAMEPLAY::SET_FIRE_AMMO_THIS_FRAME(player);
	}
	if (featureWeaponExplosiveAmmo)
	{
		if (bPlayerExists)
			GAMEPLAY::SET_EXPLOSIVE_AMMO_THIS_FRAME(player);
	}
	if (featureWeaponExplosiveMelee)
	{
		if (bPlayerExists)
			GAMEPLAY::SET_EXPLOSIVE_MELEE_THIS_FRAME(player);
	}
	static LPCSTR weaponNames[55] = { "WEAPON_KNIFE", "WEAPON_NIGHTSTICK", "WEAPON_HAMMER", "WEAPON_BAT", "WEAPON_GOLFCLUB", "WEAPON_CROWBAR", "WEAPON_PISTOL", "WEAPON_COMBATPISTOL", "WEAPON_APPISTOL", "WEAPON_PISTOL50", "WEAPON_MICROSMG", "WEAPON_SMG", "WEAPON_ASSAULTSMG", "WEAPON_ASSAULTRIFLE", "WEAPON_CARBINERIFLE", "WEAPON_ADVANCEDRIFLE", "WEAPON_MG", "WEAPON_COMBATMG", "WEAPON_PUMPSHOTGUN", "WEAPON_SAWNOFFSHOTGUN", "WEAPON_ASSAULTSHOTGUN", "WEAPON_BULLPUPSHOTGUN", "WEAPON_STUNGUN", "WEAPON_SNIPERRIFLE", "WEAPON_HEAVYSNIPER", "WEAPON_GRENADELAUNCHER", "WEAPON_GRENADELAUNCHER_SMOKE", "WEAPON_RPG", "WEAPON_MINIGUN", "WEAPON_GRENADE", "WEAPON_STICKYBOMB", "WEAPON_SMOKEGRENADE", "WEAPON_BZGAS", "WEAPON_MOLOTOV", "WEAPON_FIREEXTINGUISHER", "WEAPON_PETROLCAN", "WEAPON_SNSPISTOL", "WEAPON_SPECIALCARBINE", "WEAPON_HEAVYPISTOL", "WEAPON_BULLPUPRIFLE", "WEAPON_HOMINGLAUNCHER", "WEAPON_PROXMINE", "WEAPON_SNOWBALL", "WEAPON_VINTAGEPISTOL", "WEAPON_DAGGER", "WEAPON_FIREWORK", "WEAPON_MUSKET", "WEAPON_MARKSMANRIFLE", "WEAPON_HEAVYSHOTGUN", "WEAPON_GUSENBERG", "WEAPON_HATCHET", "WEAPON_RAILGUN", "WEAPON_COMBATPDW", "WEAPON_KNUCKLE", "WEAPON_MARKSMANPISTOL" };

	// weapon no reload
	if (featureWeaponNoReloadUpdated && bPlayerExists)
	{
		WEAPON::SET_PED_INFINITE_AMMO(playerPed, FALSE, GAMEPLAY::GET_HASH_KEY((char*)weaponNames));
		WEAPON::SET_PED_INFINITE_AMMO_CLIP(playerPed, FALSE);
	}
	if (featureWeaponNoReload && bPlayerExists)
	{
		Hash x = GAMEPLAY::GET_HASH_KEY((char*)weaponNames);
		if (WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &x, TRUE))
			if (WEAPON::IS_WEAPON_VALID(GAMEPLAY::GET_HASH_KEY((char*)weaponNames)))
				WEAPON::SET_PED_INFINITE_AMMO(playerPed, TRUE, GAMEPLAY::GET_HASH_KEY((char*)weaponNames));
		WEAPON::SET_PED_INFINITE_AMMO_CLIP(playerPed, TRUE);
	}

	// weapon firework rounds
	if (featureWeaponFirework)
	{
		superRounds(playerPed, "WEAPON_FIREWORK");
	}
	// weapon rps rounds
	if (featureWeaponVehicleRocket)
	{
		superRounds(playerPed, "WEAPON_VEHICLE_ROCKET");
	}
	if (featureWeaponTank)
	{
		superRounds(playerPed, "VEHICLE_WEAPON_TANK");
	}
	if (featureWeaponRailGun)
	{
		superRounds(playerPed, "WEAPON_RAILGUN");
	}
	if (featureWeaponHeavySniper)
	{
		superRounds(playerPed, "WEAPON_HEAVYSNIPER");
	}
	if (featureWeaponMinigun)
	{
		superRounds(playerPed, "WEAPON_MINIGUN");
	}
	if (featureWeaponPlaneRocket)
	{
		superRounds(playerPed, "VEHICLE_WEAPON_PLANE_ROCKET");
	}
	if (featureWeaponGrenadeLauncher)
	{
		superRounds(playerPed, "WEAPON_GRENADELAUNCHER");
	}
	if (featureWeaponAirstrikeRocket)
	{
		superRounds(playerPed, "WEAPON_AIRSTRIKE_ROCKET");
	}
	if (featureWeaponSavage)
	{
		superRounds(playerPed, "VEHICLE_WEAPON_PLAYER_SAVAGE");
	}
	if (featureWeaponSpaceRocket)
	{
		superRounds(playerPed, "VEHICLE_WEAPON_SPACE_ROCKET");
	}

	if (featureWeaponPropMoneyBag)
	{
		shootObject(playerPed, "prop_money_bag_01");
	}
	if (featureWeaponPropCokeBlock)
	{
		shootObject(playerPed, "prop_coke_block_01");
	}
	if (featureWeaponPropWeed)
	{
		shootObject(playerPed, "prop_weed_01");
	}
	if (featureWeaponPropWeedPallet)
	{
		shootObject(playerPed, "prop_weed_pallet");
	}
	if (featureWeaponPropXmasTree)
	{
		shootObject(playerPed, "prop_xmas_tree_int");
	}
	if (featureWeaponPropXmas)
	{
		shootObject(playerPed, "prop_xmas_ext");
	}
	if (featureWeaponPropDildo)
	{
		shootObject(playerPed, "prop_cs_dildo_01");
	}

	if (weaponOnePunchKillUpdated)
	{
		PLAYER::SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(player, 1.0f);
	}
	if (weaponOnePunchKill)
	{
		PLAYER::SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(player, 50.0f);
	}

	// weapon damage modifier
	if (featureWeaponDamageModifierUpdate)
	{
		PLAYER::SET_PLAYER_WEAPON_DAMAGE_MODIFIER(player, 1.0f);
	}
	if (featureWeaponDamageModifier)
	{
		PLAYER::SET_PLAYER_WEAPON_DAMAGE_MODIFIER(player, 50.0f);
	}

	if (weaponForceGun)
	{
		forceGun(player, playerPed);
	}

	if (weaponGravityGun)
	{
		gravityGun();
	}

	if (weaponSpaghettios && GetAsyncKeyState(VK_DIVIDE))
	{
		killSpaghettios(playerPed);
	}

	Object object;
	if (weaponDeleteObjects && bPlayerExists && PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &object) && ENTITY::IS_ENTITY_AN_OBJECT(object) && ENTITY::GET_ENTITY_ALPHA(object) == 255.0)
	{
		OBJECT::DELETE_OBJECT(&object);
	}

	// player's vehicle invincible
	if (featureVehInvincibleUpdated)
	{
		if (bPlayerExists && !featureVehInvincible && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
		{
			ENTITY::SET_ENTITY_INVINCIBLE(veh, FALSE);
			ENTITY::SET_ENTITY_PROOFS(veh, 0, 0, 0, 0, 0, 0, 0, 0);
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, 1);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, 1);
			VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(veh, 1);
		}
		featureVehInvincibleUpdated = FALSE;
	}
	if (featureVehInvincible)
	{
		if (bPlayerExists && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
		{
			ENTITY::SET_ENTITY_INVINCIBLE(veh, TRUE);
			ENTITY::SET_ENTITY_PROOFS(veh, 1, 1, 1, 1, 1, 1, 1, 1);
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, 0);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, 0);
			VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(veh, 0);
		}
	}

	// vehicle invisible
	if (featureVehInvisibleUpdated)
	{
		if (bPlayerExists && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0))
			ENTITY::SET_ENTITY_ALPHA(veh, 255, TRUE);
		ENTITY::RESET_ENTITY_ALPHA(veh);
		ENTITY::SET_ENTITY_VISIBLE(veh, TRUE, 1);
		GRAPHICS::SET_ENTITY_ICON_VISIBILITY(veh, TRUE);
	}
	if (featureVehInvisible)
	{
		if (bPlayerExists && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0))
			ENTITY::SET_ENTITY_ALPHA(veh, 0, FALSE);
		ENTITY::SET_ENTITY_VISIBLE(veh, FALSE, 1);
		GRAPHICS::SET_ENTITY_ICON_VISIBILITY(veh, FALSE);
	}

	// player's vehicle invincible wheels, usefull with custom handling
	if (featureVehInvincibleWheelsUpdated)
	{
		if (bPlayerExists && !featureVehInvincibleWheels && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
		{
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, TRUE);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, TRUE);
			VEHICLE::SET_VEHICLE_HAS_STRONG_AXLES(veh, FALSE);
		}
		featureVehInvincibleWheelsUpdated = FALSE;
	}
	if (featureVehInvincibleWheels)
	{
		if (bPlayerExists && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
		{
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, FALSE);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, FALSE);
			VEHICLE::SET_VEHICLE_HAS_STRONG_AXLES(veh, TRUE);
		}
	}

	// seat belt
	const int PED_FLAG_CAN_FLY_THRU_WINDSCREEN = 32;
	if (featureVehSeatbeltUpdated)
	{
		if (bPlayerExists && !featureVehSeatbelt)
			PED::SET_PED_CONFIG_FLAG(playerPed, PED_FLAG_CAN_FLY_THRU_WINDSCREEN, TRUE);
		featureVehSeatbeltUpdated = FALSE;
	}
	if (featureVehSeatbelt)
	{
		if (bPlayerExists)
		{
			if (PED::GET_PED_CONFIG_FLAG(playerPed, PED_FLAG_CAN_FLY_THRU_WINDSCREEN, TRUE))
				PED::SET_PED_CONFIG_FLAG(playerPed, PED_FLAG_CAN_FLY_THRU_WINDSCREEN, FALSE);
		}
	}

	// player's vehicle boost
	if (featureVehSpeedBoost && bPlayerExists && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		DWORD model = ENTITY::GET_ENTITY_MODEL(veh);
		if (GetAsyncKeyState(VK_NUMPAD9) || GetAsyncKeyState(VK_NUMPAD3))
		{
			float speed = ENTITY::GET_ENTITY_SPEED(veh);
			if (GetAsyncKeyState(VK_NUMPAD9))
			{
				if (speed < 3.0f) speed = 3.0f;
				speed += speed * 0.05f;
				//AUDIO::SET_VEHICLE_BOOST_ACTIVE(veh, TRUE);
					//GRAPHICS::_START_SCREEN_EFFECT("RaceTurbo", 0, FALSE);
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, speed);
				applyForceToEntity(veh, 0.0f, 0.0f, -0.5f);
				AUDIO::SET_VEHICLE_BOOST_ACTIVE(veh, FALSE);
			}
			else
				if (ENTITY::IS_ENTITY_IN_AIR(veh) || speed > 5.0f)
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, 0.0f);
		}
	}

	// vehicle rainbow car
	if (featureVehRainbow && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0) && vehicleUpdateTime + 400.0 < GetTickCount())
	{
		vehicleUpdateTime = GetTickCount();
		int colorPrimary = 0, colorSecondary = 0;
		VEHICLE::GET_VEHICLE_COLOURS(veh, &colorPrimary, &colorSecondary);
		if (rand() % 2)
			VEHICLE::SET_VEHICLE_COLOURS(veh, rand() % (159 + 1), colorSecondary);
		else
			VEHICLE::SET_VEHICLE_COLOURS(veh, colorPrimary, rand() % (159 + 1));
	}

	// vehicle dift
	if (featureVehDrift)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
			VEHICLE::SET_VEHICLE_REDUCE_GRIP(PED::GET_VEHICLE_PED_IS_IN(playerPed, 0), 1);
	}
	else
	{
		VEHICLE::SET_VEHICLE_REDUCE_GRIP(PED::GET_VEHICLE_PED_IS_IN(playerPed, 0), 0);
	}

	// vehicle grip
	if (featureVehGrip)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
			VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh);
	}

	// vehicle slammed
	if (featureVehSlammedUpdated && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		VEHICLE::_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 0.0f);
	}
	if (featureVehSlammed && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		applyForceToEntity(veh, 0.0f, 0.0f, -0.666f);
		VEHICLE::_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 1.0f);
		VEHICLE::_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 100.0f);
	}

	// vehicle modifier
	if (featureVehDamageModifierUpdate)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
			PLAYER::SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(player, 1.0f);
	}
	if (featureVehDamageModifier)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
			PLAYER::SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(player, 50.0f);
	}

	if (featureVehAutoFix && VEHICLE::_IS_VEHICLE_DAMAGED(veh))
	{
		VEHICLE::SET_VEHICLE_FIXED(PED::GET_VEHICLE_PED_IS_USING(playerPed));
	}
	if (!worldClearVehicles && bPlayerExists)
	{
		GAMEPLAY::CLEAR_AREA_OF_VEHICLES(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).z, 500.0, TRUE, TRUE, TRUE, TRUE, TRUE);
	}
	if (worldClearVehicles && bPlayerExists)
	{
		GAMEPLAY::CLEAR_AREA_OF_VEHICLES(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).z, 500.00, FALSE, FALSE, FALSE, FALSE, FALSE);
	}
	if (worldClearObjects && bPlayerExists)
	{
		GAMEPLAY::CLEAR_AREA_OF_OBJECTS(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).z, 500.00, TRUE);
	}
	if (worldClearPeds && bPlayerExists)
	{
		GAMEPLAY::CLEAR_AREA_OF_PEDS(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).z, 500.00, TRUE);
	}
	if (worldClearCops && bPlayerExists)
	{
		GAMEPLAY::CLEAR_AREA_OF_COPS(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).z, 500.00, TRUE);
	}
	if (worldClearProjectiles && bPlayerExists)
	{
		GAMEPLAY::CLEAR_AREA_OF_PROJECTILES(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 0, 0).z, 500.00, TRUE);
	}
	if (worldMassacre && bPlayerExists)
	{
		set_massacre_mode();
	}

	// hide hud
	if (featureMiscHideHud)
	{
		UI::HIDE_HUD_AND_RADAR_THIS_FRAME();
	}
	if (featureMiscGlobeRadioUpdated)
	{
		AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(FALSE),
			AUDIO::SET_VEHICLE_RADIO_ENABLED(PED::GET_VEHICLE_PED_IS_USING(playerPed), FALSE);
	}
	if (featureMiscGlobeRadio)
	{
		AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(TRUE),
			AUDIO::SET_VEHICLE_RADIO_ENABLED(PED::GET_VEHICLE_PED_IS_USING(playerPed), TRUE);
	}

	if (featureMiscRestrictedZones)
	{
		GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("am_armybase");
		GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("restrictedareas");
		GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_armybase");
		GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_lossantosintl");
		GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_prison");
		GAMEPLAY::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_prisonvanbreak");
	}

	if (featureNearbyPeds && bPlayerExists)
	{
		nearbyPeds(playerPed);
	}

	if (bPlayerExists)
	{
		resetMenu();

		superHotkey(PLAYER::PLAYER_PED_ID(), PLAYER::PLAYER_ID());
	}
}
#pragma region skin extras
std::string chosenSkinName = "";

bool DEBUG_MODE_SKINS = false;

int skinDetailMenuIndex = 0;
int skinDetailMenuValue = 0;

int skinDrawableMenuIndex = 0;
int skinDrawableMenuValue = -1;

int skinTextureMenuIndex = 0;
int skinTextureMenuValue = -1;

bool skinchanger_used = false;

const std::vector<std::string> MENU_SKINS_TYPES_CAPTIONS{ "Players", "Animals", "Everything Else", "Customise Current", "Reset Current" };

int skinMainMenuPosition = 0;

int skinTypesMenuPositionMemory[3] = { 0, 0, 0 }; //player, animals, general

const std::vector<std::string> SKINS_PLAYER_CAPTIONS{ "Michael", "Franklin", "Trevor" };
const std::vector<std::string> SKINS_PLAYER_VALUES{ "player_zero", "player_one", "player_two" };

const std::vector<std::string> SKINS_ANIMAL_CAPTIONS{ "Boar", "Chimp", "Cow", "Coyote", "Deer", "Fish", "Hen", "Cat", "Hawk", "Cormorant", "Crow", "Dolphin", "Humpback", "Whale", "Pigeon", "Seagull", "Hammerhead Shark", "Pig", "Rat", "Rhesus", "Chop", "Husky", "Mountain Lion", "Retriever", "Tiger Shark", "German Shepherd" };
const std::vector<std::string> SKINS_ANIMAL_VALUES{ "a_c_boar", "a_c_chimp", "a_c_cow", "a_c_coyote", "a_c_deer", "a_c_fish", "a_c_hen", "a_c_cat_01", "a_c_chickenhawk", "a_c_cormorant", "a_c_crow", "a_c_dolphin", "a_c_humpback", "a_c_killerwhale", "a_c_pigeon", "a_c_seagull", "a_c_sharkhammer", "a_c_pig", "a_c_rat", "a_c_rhesus", "a_c_chop", "a_c_husky", "a_c_mtlion", "a_c_retriever", "a_c_sharktiger", "a_c_shepherd" };

const std::vector<std::string> SKINS_GENERAL_CAPTIONS{ "Abigail Mathers", "Abigail Mathers (IG)", "Abner", "African American Male", "Air Hostess", "Air Worker Male", "Al Di Napoli Male", "Alien", "Altruist Cult Mid-Age Male", "Altruist Cult Old Male", "Altruist Cult Old Male 2", "Altruist Cult Young Male", "Altruist Cult Young Male 2", "Amanda De Santa (CS)", "Amanda De Santa (IG)", "Ammu-Nation City Clerk", "Ammu-Nation Rural Clerk", "Andreas Sanchez (CS)", "Andreas Sanchez (IG)", "Anita Mendoza", "Anton Beaudelaire", "Anton Beaudelaire", "Armenian Boss", "Armenian Goon", "Armenian Goon 2", "Armenian Lieutenant", "Armoured Van Security", "Armoured Van Security", "Armoured Van Security 2", "Army Mechanic", "Ashley Butler (CS)", "Ashley Butler (IG)", "Autopsy Tech", "Autoshop Worker", "Autoshop Worker 2", "Azteca", "Baby D", "Ballas East Male", "Ballas Female", "Ballas OG", "Ballas OG (IG)", "Ballas Original Male (IG)", "Ballas South Male", "Bank Manager (CS)", "Bank Manager (IG)", "Bank Manager Male", "Barber Female", "Barman", "Barry (CS)", "Barry (IG)", "Bartender", "Bartender (Rural)", "Baywatch Female", "Baywatch Male", "Beach Female", "Beach Male", "Beach Male 2", "Beach Muscle Male", "Beach Muscle Male 2", "Beach Old Male", "Beach Tramp Female", "Beach Tramp Male", "Beach Young Female", "Beach Young Male", "Beach Young Male 2", "Beach Young Male 3", "Best Man (IG)", "Beverly Felton (CS)", "Beverly Felton (IG)", "Beverly Hills Female", "Beverly Hills Female 2", "Beverly Hills Male", "Beverly Hills Male 2", "Beverly Hills Young Female", "Beverly Hills Young Female 2", "Beverly Hills Young Female 3", "Beverly Hills Young Female 4", "Beverly Hills Young Male", "Beverly Hills Young Male 2", "Bigfoot (CS)", "Bigfoot (IG)", "Bike Hire Guy", "Biker Chic Female", "Black Ops Soldier", "Black Ops Soldier 2", "Black Street Male", "Black Street Male 2", "Bodybuilder Female", "Bouncer", "Brad (CS)", "Brad (IG)", "Brad's Cadaver (CS)", "Breakdancer Male", "Bride", "Bride (IG)", "Burger Drug Worker", "Burger Drug Worker", "Busboy", "Business Casual", "Business Female 2", "Business Male", "Business Young Female", "Business Young Female 2", "Business Young Female 3", "Business Young Female 4", "Business Young Male", "Business Young Male 2", "Business Young Male 3", "Busker", "Car 3 Guy 1", "Car 3 Guy 1 (IG)", "Car 3 Guy 2", "Car 3 Guy 2 (IG)", "Car Buyer (CS)", "Casey (CS)", "Casey (IG)", "Chef", "Chef", "Chef (IG)", "Chemical Plant Security", "Chemical Plant Worker", "Chinese Boss", "Chinese Goon", "Chinese Goon", "Chinese Goon 2", "Chinese Goon Older", "Chip", "Claude Speed", "Clay Jackson (The Pain Giver) (IG)", "Clay Simons (The Lost) (CS)", "Clay Simons (The Lost) (IG)", "Cletus", "Cletus (IG)", "Clown", "Construction Worker", "Construction Worker 2", "Cop", "Cop Female", "Cop Male", "Corpse Female", "Corpse Young Female", "Corpse Young Female 2", "Cris Formage (CS)", "Cris Formage (IG)", "Customer", "Cyclist Male", "Cyclist Male", "Dale (CS)", "Dale (IG)", "Dave Norton (CS)", "Dave Norton (IG)", "Dead Hooker", "Dealer", "Debra (CS)", "Denise (CS)", "Denise (IG)", "Denise's Friend", "Devin (CS)", "Devin (IG)", "Devin's Security", "Dock Worker", "Dock Worker", "Doctor", "Dom Beasley (CS)", "Dom Beasley (IG)", "Doorman", "Downhill Cyclist", "Downtown Female", "Downtown Male", "Dr. Friedlander (CS)", "Dr. Friedlander (IG)", "Dressy Female", "DW Airport Worker", "DW Airport Worker 2", "East SA Female", "East SA Female 2", "East SA Male", "East SA Male 2", "East SA Young Female", "East SA Young Female 2", "East SA Young Female 3", "East SA Young Male", "East SA Young Male 2", "Epsilon Female", "Epsilon Male", "Epsilon Male 2", "Epsilon Tom (CS)", "Epsilon Tom (IG)", "Ex-Army Male", "Ex-Mil Bum", "Fabien (CS)", "Fabien (IG)", "Factory Worker Female", "Factory Worker Male", "Families CA Male", "Families DD Male", "Families DNF Male", "Families Female", "Families FOR Male", "Families Gang Member?", "Families Gang Member? (IG)", "Farmer", "Fat Black Female", "Fat Cult Female", "Fat Latino Male", "Fat White Female", "Ferdinand Kerimov (Mr. K) (CS)", "Ferdinand Kerimov (Mr. K) (IG)", "FIB Architect", "FIB Mugger", "FIB Office Worker", "FIB Office Worker 2", "FIB Security", "FIB Suit (CS)", "FIB Suit (IG)", "Financial Guru", "Fireman Male", "Fitness Female", "Fitness Female 2", "Floyd Hebert (CS)", "Floyd Hebert (IG)", "FOS Rep?", "Free Mode Female", "Free Mode Male", "G?", "Gaffer", "Garbage Worker", "Gardener", "Gay Male", "Gay Male 2", "General Fat Male", "General Fat Male 2", "General Hot Young Female", "General Street Old Female", "General Street Old Male", "General Street Young Male", "General Street Young Male 2", "GLENSTANK? Male", "Golfer Male", "Golfer Young Female", "Golfer Young Male", "Griff", "Grip", "Groom", "Groom (IG)", "Grove Street Dealer", "Guadalope (CS)", "Guido", "Gun Vendor", "GURK? (CS)", "Hairdresser Male", "Hao", "Hao (IG)", "Hasidic Jew Male", "Hasidic Jew Young Male", "Hick", "Hick (IG)", "High Security", "High Security 2", "Highway Cop", "Hiker Female", "Hiker Male", "Hillbilly Male", "Hillbilly Male 2", "Hippie Female", "Hippie Male", "Hippie Male", "Hipster", "Hipster (IG)", "Hipster Female", "Hipster Female 2", "Hipster Female 3", "Hipster Female 4", "Hipster Male", "Hipster Male 2", "Hipster Male 3", "Hooker", "Hooker 2", "Hooker 3", "Hospital Scrubs Female", "Hot Posh Female", "Hugh Welsh", "Hunter (CS)", "Hunter (IG)", "IAA Security", "Impotent Rage", "Imran Shinowa", "Indian Male", "Indian Old Female", "Indian Young Female", "Indian Young Male", "Jane", "Janet (CS)", "Janet (IG)", "Janitor", "Janitor", "Jay Norris (IG)", "Jesco White (Tapdancing Hillbilly)", "Jesus", "Jetskier", "Jewel Heist Driver", "Jewel Heist Gunman", "Jewel Heist Hacker", "Jewel Thief", "Jeweller Assistant", "Jeweller Assistant (CS)", "Jeweller Assistant (IG)", "Jeweller Security", "Jimmy Boston (CS)", "Jimmy Boston (IG)", "Jimmy De Santo (CS)", "Jimmy De Santo (IG)", "Jogger Female", "Jogger Male", "Jogger Male 2", "John Marston", "Johnny Klebitz (CS)", "Johnny Klebitz (IG)", "Josef (CS)", "Josef (IG)", "Josh (CS)", "Josh (IG)", "Juggalo Female", "Juggalo Male", "Justin", "Kerry McIntosh (IG)", "Kifflom Guy", "Korean Boss", "Korean Female", "Korean Female 2", "Korean Lieutenant", "Korean Male", "Korean Old Female", "Korean Old Male", "Korean Young Male", "Korean Young Male", "Korean Young Male 2", "Korean Young Male 2", "Lamar Davis (CS)", "Lamar Davis (IG)", "Latino Handyman Male", "Latino Street Male 2", "Latino Street Young Male", "Latino Young Male", "Lazlow (CS)", "Lazlow (IG)", "Lester Crest (CS)", "Lester Crest (IG)", "Life Invader (CS)", "Life Invader (IG)", "Life Invader 2 (IG)", "Life Invader Male", "Line Cook", "Love Fist Willy", "LS Metro Worker Male", "Magenta (CS)", "Magenta (IG)", "Maid", "Malibu Male", "Mani", "Manuel (CS)", "Manuel (IG)", "Mariachi", "Marine", "Marine", "Marine 2", "Marine Young", "Marine Young 2", "Marine Young 3", "Mark Fostenburg", "Marnie Allen (CS)", "Marnie Allen (IG)", "Martin Madrazo (CS)", "Mary-Ann Quinn (CS)", "Mary-Ann Quinn (IG)", "Maude", "Maude (IG)", "Mechanic", "Mechanic 2", "Merryweather Merc", "Meth Addict", "Mexican", "Mexican (IG)", "Mexican Boss", "Mexican Boss 2", "Mexican Gang Member", "Mexican Goon", "Mexican Goon 2", "Mexican Goon 3", "Mexican Labourer", "Mexican Rural", "Mexican Thug", "Michelle (CS)", "Michelle (IG)", "Migrant Female", "Migrant Male", "Milton McIlroy (CS)", "Milton McIlroy (IG)", "Mime Artist", "Minuteman Joe (CS)", "Minuteman Joe (IG)", "Miranda", "Mistress", "Misty", "Molly (CS)", "Molly (IG)", "Motocross Biker", "Motocross Biker 2", "Movie Astronaut", "Movie Director", "Movie Premiere Female", "Movie Premiere Female (CS)", "Movie Premiere Male", "Movie Premiere Male (CS)", "Movie Star Female", "Mrs. Phillips (CS)", "Mrs. Phillips (IG)", "Mrs. Thornhill (CS)", "Mrs. Thornhill (IG)", "Natalia (CS)", "Natalia (IG)", "Nervous Ron (CS)", "Nervous Ron (IG)", "Nigel (CS)", "Nigel (IG)", "Niko Bellic", "OG Boss", "Old Man 1 (CS)", "Old Man 1 (IG)", "Old Man 2 (CS)", "Old Man 2 (IG)", "Omega (CS)", "Omega (IG)", "O'Neil Brothers (IG)", "Ortega", "Ortega (IG)", "Oscar", "Paparazzi Male", "Paparazzi Young Male", "Paramedic", "Party Target", "Partygoer", "Patricia (CS)", "Patricia (IG)", "Pest Control", "Peter Dreyfuss (CS)", "Peter Dreyfuss (IG)", "Pilot", "Pilot", "Pilot 2", "Pogo the Monkey", "Polynesian", "Polynesian Goon", "Polynesian Goon 2", "Polynesian Young", "Poppy Mitchell", "Porn Dude", "Postal Worker Male", "Postal Worker Male 2", "Priest (CS)", "Priest (IG)", "Princess", "Prison Guard", "Prisoner", "Prisoner", "Prisoner (Muscular)", "Prologue Driver", "Prologue Driver", "Prologue Host Female", "Prologue Host Male", "Prologue Host Old Female", "Prologue Mourner Female", "Prologue Mourner Male", "Prologue Security", "Prologue Security", "Prologue Security 2 (CS)", "Prologue Security 2 (IG)", "PROS?", "Ranger Female", "Ranger Male", "Reporter", "Republican Space Ranger", "Rival Paparazzo", "Road Cyclist", "Robber", "Rocco Pelosi", "Rocco Pelosi (IG)", "Rural Meth Addict Female", "Rural Meth Addict Male", "Russian Drunk (CS)", "Russian Drunk (IG)", "Sales Assistant (High-End)", "Sales Assistant (Low-End)", "Sales Assistant (Mask Stall)", "Sales Assistant (Mid-Price)", "Salton Female", "Salton Male", "Salton Male 2", "Salton Male 3", "Salton Male 4", "Salton Old Female", "Salton Old Male", "Salton Young Male", "Salvadoran Boss", "Salvadoran Goon", "Salvadoran Goon 2", "Salvadoran Goon 3", "Scientist", "Screenwriter", "Screenwriter (IG)", "Security Guard", "Sheriff Female", "Sheriff Male", "Shopkeeper", "Simeon Yetarian (CS)", "Simeon Yetarian (IG)", "Skater Female", "Skater Male", "Skater Young Male", "Skater Young Male 2", "Skid Row Female", "Skid Row Male", "Snow Cop Male", "Solomon Richards (CS)", "Solomon Richards (IG)", "South Central Female", "South Central Female 2", "South Central Latino Male", "South Central Male", "South Central Male 2", "South Central Male 3", "South Central Male 4", "South Central MC Female", "South Central Old Female", "South Central Old Female 2", "South Central Old Male", "South Central Old Male 2", "South Central Old Male 3", "South Central Young Female", "South Central Young Female 2", "South Central Young Female 3", "South Central Young Male", "South Central Young Male 2", "South Central Young Male 3", "South Central Young Male 4", "Sports Biker", "Spy Actor", "Spy Actress", "Stag Party Groom", "Steve Haines (CS)", "Steve Haines (IG)", "Street Performer", "Street Preacher", "Street Punk", "Street Punk 2", "Street Vendor", "Street Vendor Young", "Stretch (CS)", "Stretch (IG)", "Stripper", "Stripper", "Stripper 2", "Stripper 2", "Stripper Lite", "Stripper Lite", "Sunbather Male", "Surfer", "SWAT", "Sweatshop Worker", "Sweatshop Worker Young", "Talina (IG)", "Tanisha (CS)", "Tanisha (IG)", "Tao Cheng (CS)", "Tao Cheng (IG)", "Tao's Translator (CS)", "Tao's Translator (IG)", "Tattoo Artist", "Tennis Coach (CS)", "Tennis Coach (IG)", "Tennis Player Female", "Tennis Player Male", "Terry (CS)", "Terry (IG)", "The Lost MC Female", "The Lost MC Male", "The Lost MC Male 2", "The Lost MC Male 3", "Tom (CS)", "Tonya", "Tonya (IG)", "Topless", "Tourist Female", "Tourist Male", "Tourist Young Female", "Tourist Young Female 2", "Tracey De Santa (CS)", "Tracey De Santa (IG)", "Traffic Warden", "Traffic Warden (IG)", "Tramp Female", "Tramp Male", "Tramp Old Male", "Tramp Old Male", "Transport Worker Male", "Transvestite Male", "Transvestite Male 2", "Trucker Male", "Tyler Dixon (IG)", "United Paper Man (CS)", "United Paper Man (IG)", "UPS Driver", "UPS Driver 2", "US Coastguard", "Vagos Female", "Valet", "Vespucci Beach Male", "Vespucci Beach Male 2", "Vinewood Douche", "Vinewood Female", "Vinewood Female 2", "Vinewood Female 3", "Vinewood Female 4", "Vinewood Male", "Vinewood Male 2", "Vinewood Male 3", "Vinewood Male 4", "Wade (CS)", "Wade (IG)", "Waiter", "Wei Cheng (CS)", "Wei Cheng (IG)", "White Street Male", "White Street Male 2", "Window Cleaner", "Yoga Female", "Yoga Male", "Zimbor (CS)", "Zimbor (IG)", "Zombie" };
const std::vector<std::string> SKINS_GENERAL_VALUES{ "csb_abigail", "ig_abigail", "u_m_y_abner", "a_m_m_afriamer_01", "s_f_y_airhostess_01", "s_m_y_airworker", "u_m_m_aldinapoli", "s_m_m_movalien_01", "a_m_m_acult_01", "a_m_o_acult_01", "a_m_o_acult_02", "a_m_y_acult_01", "a_m_y_acult_02", "cs_amandatownley", "ig_amandatownley", "s_m_y_ammucity_01", "s_m_m_ammucountry", "cs_andreas", "ig_andreas", "csb_anita", "csb_anton", "u_m_y_antonb", "g_m_m_armboss_01", "g_m_m_armgoon_01", "g_m_y_armgoon_02", "g_m_m_armlieut_01", "mp_s_m_armoured_01", "s_m_m_armoured_01", "s_m_m_armoured_02", "s_m_y_armymech_01", "cs_ashley", "ig_ashley", "s_m_y_autopsy_01", "s_m_m_autoshop_01", "s_m_m_autoshop_02", "g_m_y_azteca_01", "u_m_y_babyd", "g_m_y_ballaeast_01", "g_f_y_ballas_01", "csb_ballasog", "ig_ballasog", "g_m_y_ballaorig_01", "g_m_y_ballasout_01", "cs_bankman", "ig_bankman", "u_m_m_bankman", "s_f_m_fembarber", "s_m_y_barman_01", "cs_barry", "ig_barry", "s_f_y_bartender_01", "s_m_m_cntrybar_01", "s_f_y_baywatch_01", "s_m_y_baywatch_01", "a_f_m_beach_01", "a_m_m_beach_01", "a_m_m_beach_02", "a_m_y_musclbeac_01", "a_m_y_musclbeac_02", "a_m_o_beach_01", "a_f_m_trampbeac_01", "a_m_m_trampbeac_01", "a_f_y_beach_01", "a_m_y_beach_01", "a_m_y_beach_02", "a_m_y_beach_03", "ig_bestmen", "cs_beverly", "ig_beverly", "a_f_m_bevhills_01", "a_f_m_bevhills_02", "a_m_m_bevhills_01", "a_m_m_bevhills_02", "a_f_y_bevhills_01", "a_f_y_bevhills_02", "a_f_y_bevhills_03", "a_f_y_bevhills_04", "a_m_y_bevhills_01", "a_m_y_bevhills_02", "cs_orleans", "ig_orleans", "u_m_m_bikehire_01", "u_f_y_bikerchic", "s_m_y_blackops_01", "s_m_y_blackops_02", "a_m_y_stbla_01", "a_m_y_stbla_02", "a_f_m_bodybuild_01", "s_m_m_bouncer_01", "cs_brad", "ig_brad", "cs_bradcadaver", "a_m_y_breakdance_01", "csb_bride", "ig_bride", "csb_burgerdrug", "u_m_y_burgerdrug_01", "s_m_y_busboy_01", "a_m_y_busicas_01", "a_f_m_business_02", "a_m_m_business_01", "a_f_y_business_01", "a_f_y_business_02", "a_f_y_business_03", "a_f_y_business_04", "a_m_y_business_01", "a_m_y_business_02", "a_m_y_business_03", "s_m_o_busker_01", "csb_car3guy1", "ig_car3guy1", "csb_car3guy2", "ig_car3guy2", "cs_carbuyer", "cs_casey", "ig_casey", "s_m_y_chef_01", "csb_chef", "ig_chef", "s_m_m_chemsec_01", "g_m_m_chemwork_01", "g_m_m_chiboss_01", "g_m_m_chigoon_01", "csb_chin_goon", "g_m_m_chigoon_02", "g_m_m_chicold_01", "u_m_y_chip", "mp_m_claude_01", "ig_claypain", "cs_clay", "ig_clay", "csb_cletus", "ig_cletus", "s_m_y_clown_01", "s_m_y_construct_01", "s_m_y_construct_02", "csb_cop", "s_f_y_cop_01", "s_m_y_cop_01", "u_f_m_corpse_01", "u_f_y_corpse_01", "u_f_y_corpse_02", "cs_chrisformage", "ig_chrisformage", "csb_customer", "a_m_y_cyclist_01", "u_m_y_cyclist_01", "cs_dale", "ig_dale", "cs_davenorton", "ig_davenorton", "mp_f_deadhooker", "s_m_y_dealer_01", "cs_debra", "cs_denise", "ig_denise", "csb_denise_friend", "cs_devin", "ig_devin", "s_m_y_devinsec_01", "s_m_m_dockwork_01", "s_m_y_dockwork_01", "s_m_m_doctor_01", "cs_dom", "ig_dom", "s_m_y_doorman_01", "a_m_y_dhill_01", "a_f_m_downtown_01", "a_m_y_downtown_01", "cs_drfriedlander", "ig_drfriedlander", "a_f_y_scdressy_01", "s_m_y_dwservice_01", "s_m_y_dwservice_02", "a_f_m_eastsa_01", "a_f_m_eastsa_02", "a_m_m_eastsa_01", "a_m_m_eastsa_02", "a_f_y_eastsa_01", "a_f_y_eastsa_02", "a_f_y_eastsa_03", "a_m_y_eastsa_01", "a_m_y_eastsa_02", "a_f_y_epsilon_01", "a_m_y_epsilon_01", "a_m_y_epsilon_02", "cs_tomepsilon", "ig_tomepsilon", "mp_m_exarmy_01", "u_m_y_militarybum", "cs_fabien", "ig_fabien", "s_f_y_factory_01", "s_m_y_factory_01", "g_m_y_famca_01", "mp_m_famdd_01", "g_m_y_famdnf_01", "g_f_y_families_01", "g_m_y_famfor_01", "csb_ramp_gang", "ig_ramp_gang", "a_m_m_farmer_01", "a_f_m_fatbla_01", "a_f_m_fatcult_01", "a_m_m_fatlatin_01", "a_f_m_fatwhite_01", "cs_mrk", "ig_mrk", "u_m_m_fibarchitect", "u_m_y_fibmugger_01", "s_m_m_fiboffice_01", "s_m_m_fiboffice_02", "mp_m_fibsec_01", "cs_fbisuit_01", "ig_fbisuit_01", "u_m_o_finguru_01", "s_m_y_fireman_01", "a_f_y_fitness_01", "a_f_y_fitness_02", "cs_floyd", "ig_floyd", "csb_fos_rep", "mp_f_freemode_01", "mp_m_freemode_01", "csb_g", "s_m_m_gaffer_01", "s_m_y_garbage", "s_m_m_gardener_01", "a_m_y_gay_01", "a_m_y_gay_02", "a_m_m_genfat_01", "a_m_m_genfat_02", "a_f_y_genhot_01", "a_f_o_genstreet_01", "a_m_o_genstreet_01", "a_m_y_genstreet_01", "a_m_y_genstreet_02", "u_m_m_glenstank_01", "a_m_m_golfer_01", "a_f_y_golfer_01", "a_m_y_golfer_01", "u_m_m_griff_01", "s_m_y_grip_01", "csb_groom", "ig_groom", "csb_grove_str_dlr", "cs_guadalope", "u_m_y_guido_01", "u_m_y_gunvend_01", "cs_gurk", "s_m_m_hairdress_01", "csb_hao", "ig_hao", "a_m_m_hasjew_01", "a_m_y_hasjew_01", "csb_ramp_hic", "ig_ramp_hic", "s_m_m_highsec_01", "s_m_m_highsec_02", "s_m_y_hwaycop_01", "a_f_y_hiker_01", "a_m_y_hiker_01", "a_m_m_hillbilly_01", "a_m_m_hillbilly_02", "a_f_y_hippie_01", "u_m_y_hippie_01", "a_m_y_hippy_01", "csb_ramp_hipster", "ig_ramp_hipster", "a_f_y_hipster_01", "a_f_y_hipster_02", "a_f_y_hipster_03", "a_f_y_hipster_04", "a_m_y_hipster_01", "a_m_y_hipster_02", "a_m_y_hipster_03", "s_f_y_hooker_01", "s_f_y_hooker_02", "s_f_y_hooker_03", "s_f_y_scrubs_01", "u_f_y_hotposh_01", "csb_hugh", "cs_hunter", "ig_hunter", "s_m_m_ciasec_01", "u_m_y_imporage", "csb_imran", "a_m_m_indian_01", "a_f_o_indian_01", "a_f_y_indian_01", "a_m_y_indian_01", "u_f_y_comjane", "cs_janet", "ig_janet", "csb_janitor", "s_m_m_janitor", "ig_jay_norris", "u_m_o_taphillbilly", "u_m_m_jesus_01", "a_m_y_jetski_01", "hc_driver", "hc_gunman", "hc_hacker", "u_m_m_jewelthief", "u_f_y_jewelass_01", "cs_jewelass", "ig_jewelass", "u_m_m_jewelsec_01", "cs_jimmyboston", "ig_jimmyboston", "cs_jimmydisanto", "ig_jimmydisanto", "a_f_y_runner_01", "a_m_y_runner_01", "a_m_y_runner_02", "mp_m_marston_01", "cs_johnnyklebitz", "ig_johnnyklebitz", "cs_josef", "ig_josef", "cs_josh", "ig_josh", "a_f_y_juggalo_01", "a_m_y_juggalo_01", "u_m_y_justin", "ig_kerrymcintosh", "u_m_y_baygor", "g_m_m_korboss_01", "a_f_m_ktown_01", "a_f_m_ktown_02", "g_m_y_korlieut_01", "a_m_m_ktown_01", "a_f_o_ktown_01", "a_m_o_ktown_01", "g_m_y_korean_01", "a_m_y_ktown_01", "g_m_y_korean_02", "a_m_y_ktown_02", "cs_lamardavis", "ig_lamardavis", "s_m_m_lathandy_01", "a_m_m_stlat_02", "a_m_y_stlat_01", "a_m_y_latino_01", "cs_lazlow", "ig_lazlow", "cs_lestercrest", "ig_lestercrest", "cs_lifeinvad_01", "ig_lifeinvad_01", "ig_lifeinvad_02", "s_m_m_lifeinvad_01", "s_m_m_linecook", "u_m_m_willyfist", "s_m_m_lsmetro_01", "cs_magenta", "ig_magenta", "s_f_m_maid_01", "a_m_m_malibu_01", "u_m_y_mani", "cs_manuel", "ig_manuel", "s_m_m_mariachi_01", "csb_ramp_marine", "s_m_m_marine_01", "s_m_m_marine_02", "s_m_y_marine_01", "s_m_y_marine_02", "s_m_y_marine_03", "u_m_m_markfost", "cs_marnie", "ig_marnie", "cs_martinmadrazo", "cs_maryann", "ig_maryann", "csb_maude", "ig_maude", "s_m_y_xmech_01", "s_m_y_xmech_02", "csb_mweather", "a_m_y_methhead_01", "csb_ramp_mex", "ig_ramp_mex", "g_m_m_mexboss_01", "g_m_m_mexboss_02", "g_m_y_mexgang_01", "g_m_y_mexgoon_01", "g_m_y_mexgoon_02", "g_m_y_mexgoon_03", "a_m_m_mexlabor_01", "a_m_m_mexcntry_01", "a_m_y_mexthug_01", "cs_michelle", "ig_michelle", "s_f_y_migrant_01", "s_m_m_migrant_01", "cs_milton", "ig_milton", "s_m_y_mime", "cs_joeminuteman", "ig_joeminuteman", "u_f_m_miranda", "u_f_y_mistress", "mp_f_misty_01", "cs_molly", "ig_molly", "a_m_y_motox_01", "a_m_y_motox_02", "s_m_m_movspace_01", "u_m_m_filmdirector", "s_f_y_movprem_01", "cs_movpremf_01", "s_m_m_movprem_01", "cs_movpremmale", "u_f_o_moviestar", "cs_mrsphillips", "ig_mrsphillips", "cs_mrs_thornhill", "ig_mrs_thornhill", "cs_natalia", "ig_natalia", "cs_nervousron", "ig_nervousron", "cs_nigel", "ig_nigel", "mp_m_niko_01", "a_m_m_og_boss_01", "cs_old_man1a", "ig_old_man1a", "cs_old_man2", "ig_old_man2", "cs_omega", "ig_omega", "ig_oneil", "csb_ortega", "ig_ortega", "csb_oscar", "a_m_m_paparazzi_01", "u_m_y_paparazzi", "s_m_m_paramedic_01", "u_m_m_partytarget", "u_m_y_party_01", "cs_patricia", "ig_patricia", "s_m_y_pestcont_01", "cs_dreyfuss", "ig_dreyfuss", "s_m_m_pilot_01", "s_m_y_pilot_01", "s_m_m_pilot_02", "u_m_y_pogo_01", "a_m_m_polynesian_01", "g_m_y_pologoon_01", "g_m_y_pologoon_02", "a_m_y_polynesian_01", "u_f_y_poppymich", "csb_porndudes", "s_m_m_postal_01", "s_m_m_postal_02", "cs_priest", "ig_priest", "u_f_y_princess", "s_m_m_prisguard_01", "s_m_y_prisoner_01", "u_m_y_prisoner_01", "s_m_y_prismuscl_01", "u_m_y_proldriver_01", "csb_prologuedriver", "a_f_m_prolhost_01", "a_m_m_prolhost_01", "u_f_o_prolhost_01", "u_f_m_promourn_01", "u_m_m_promourn_01", "csb_prolsec", "u_m_m_prolsec_01", "cs_prolsec_02", "ig_prolsec_02", "mp_g_m_pros_01", "s_f_y_ranger_01", "s_m_y_ranger_01", "csb_reporter", "u_m_y_rsranger_01", "u_m_m_rivalpap", "a_m_y_roadcyc_01", "s_m_y_robber_01", "csb_roccopelosi", "ig_roccopelosi", "a_f_y_rurmeth_01", "a_m_m_rurmeth_01", "cs_russiandrunk", "ig_russiandrunk", "s_f_m_shop_high", "s_f_y_shop_low", "s_m_y_shop_mask", "s_f_y_shop_mid", "a_f_m_salton_01", "a_m_m_salton_01", "a_m_m_salton_02", "a_m_m_salton_03", "a_m_m_salton_04", "a_f_o_salton_01", "a_m_o_salton_01", "a_m_y_salton_01", "g_m_y_salvaboss_01", "g_m_y_salvagoon_01", "g_m_y_salvagoon_02", "g_m_y_salvagoon_03", "s_m_m_scientist_01", "csb_screen_writer", "ig_screen_writer", "s_m_m_security_01", "s_f_y_sheriff_01", "s_m_y_sheriff_01", "mp_m_shopkeep_01", "cs_siemonyetarian", "ig_siemonyetarian", "a_f_y_skater_01", "a_m_m_skater_01", "a_m_y_skater_01", "a_m_y_skater_02", "a_f_m_skidrow_01", "a_m_m_skidrow_01", "s_m_m_snowcop_01", "cs_solomon", "ig_solomon", "a_f_m_soucent_01", "a_f_m_soucent_02", "a_m_m_socenlat_01", "a_m_m_soucent_01", "a_m_m_soucent_02", "a_m_m_soucent_03", "a_m_m_soucent_04", "a_f_m_soucentmc_01", "a_f_o_soucent_01", "a_f_o_soucent_02", "a_m_o_soucent_01", "a_m_o_soucent_02", "a_m_o_soucent_03", "a_f_y_soucent_01", "a_f_y_soucent_02", "a_f_y_soucent_03", "a_m_y_soucent_01", "a_m_y_soucent_02", "a_m_y_soucent_03", "a_m_y_soucent_04", "u_m_y_sbike", "u_m_m_spyactor", "u_f_y_spyactress", "u_m_y_staggrm_01", "cs_stevehains", "ig_stevehains", "s_m_m_strperf_01", "s_m_m_strpreach_01", "g_m_y_strpunk_01", "g_m_y_strpunk_02", "s_m_m_strvend_01", "s_m_y_strvend_01", "cs_stretch", "ig_stretch", "csb_stripper_01", "s_f_y_stripper_01", "csb_stripper_02", "s_f_y_stripper_02", "s_f_y_stripperlite", "mp_f_stripperlite", "a_m_y_sunbathe_01", "a_m_y_surfer_01", "s_m_y_swat_01", "s_f_m_sweatshop_01", "s_f_y_sweatshop_01", "ig_talina", "cs_tanisha", "ig_tanisha", "cs_taocheng", "ig_taocheng", "cs_taostranslator", "ig_taostranslator", "u_m_y_tattoo_01", "cs_tenniscoach", "ig_tenniscoach", "a_f_y_tennis_01", "a_m_m_tennis_01", "cs_terry", "ig_terry", "g_f_y_lost_01", "g_m_y_lost_01", "g_m_y_lost_02", "g_m_y_lost_03", "cs_tom", "csb_tonya", "ig_tonya", "a_f_y_topless_01", "a_f_m_tourist_01", "a_m_m_tourist_01", "a_f_y_tourist_01", "a_f_y_tourist_02", "cs_tracydisanto", "ig_tracydisanto", "csb_trafficwarden", "ig_trafficwarden", "a_f_m_tramp_01", "a_m_m_tramp_01", "u_m_o_tramp_01", "a_m_o_tramp_01", "s_m_m_gentransport", "a_m_m_tranvest_01", "a_m_m_tranvest_02", "s_m_m_trucker_01", "ig_tylerdix", "cs_paper", "ig_paper", "s_m_m_ups_01", "s_m_m_ups_02", "s_m_y_uscg_01", "g_f_y_vagos_01", "s_m_y_valet_01", "a_m_y_beachvesp_01", "a_m_y_beachvesp_02", "a_m_y_vindouche_01", "a_f_y_vinewood_01", "a_f_y_vinewood_02", "a_f_y_vinewood_03", "a_f_y_vinewood_04", "a_m_y_vinewood_01", "a_m_y_vinewood_02", "a_m_y_vinewood_03", "a_m_y_vinewood_04", "cs_wade", "ig_wade", "s_m_y_waiter_01", "cs_chengsr", "ig_chengsr", "a_m_y_stwhi_01", "a_m_y_stwhi_02", "s_m_y_winclean_01", "a_f_y_yoga_01", "a_m_y_yoga_01", "cs_zimbor", "ig_zimbor", "u_m_y_zombie_01" };

/***
* METHODS
*/

void reset_skin_globals()
{
	chosenSkinName = "";
}

bool is_custom_skin_applied()
{
	return skinchanger_used;
}

void set_custom_skin_applied(bool applied)
{
	skinchanger_used = applied;
}

/*
* ===============
* WORKER METHODS
* =================
*/

int findFirstValidPedTexture(int drawable)
{
	for (int j = 0; j < 100; j++)
	{
		if (PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, drawable, j))
		{
			return j;
		}
	}
	return -1;
}

int findFirstValidPedDrawable(int component)
{
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			if (PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, i, j))
			{
				return i;
			}
		}
	}
	return -1;
}

bool applyChosenSkin(std::string skinName)
{
	DWORD model = GAMEPLAY::GET_HASH_KEY((char*)skinName.c_str());
	if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_VALID(model))
	{
		STREAMING::REQUEST_MODEL(model);
		while (!STREAMING::HAS_MODEL_LOADED(model))	WAIT(0);
		//STREAMING::LOAD_ALL_OBJECTS_NOW();
		PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
		//PED::SET_PED_RANDOM_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), FALSE);
		PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
		WAIT(0);

		skinchanger_used = true;

		//reset the skin detail choice
		skinDetailMenuIndex = 0;
		skinDetailMenuValue = 0;

		skinDrawableMenuIndex = 0;
		skinDrawableMenuValue = -1;

		skinTextureMenuIndex = 0;
		skinTextureMenuValue = -1;

		WAIT(100);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);

		chosenSkinName = skinName;

		return true;
	}
	return false;
}

std::string getSkinDetailAttribDescription(int i)
{
	switch (i)
	{
	case 0:
		return "Head/Face";
	case 1:
		return "Beard/Mask";
	case 2:
		return "Hair/Hat";
	case 3:
		return "Top";
	case 4:
		return "Legs";
	case 5:
		return "Accessory/Gloves";
	case 6:
		return "Accessory/Shoes";
	case 7:
	case 8:
	case 9:
		return "Accessory";
	case 10:
		return "Badges";
	case 11:
		return "Shirt/Jacket";
	default:
		return std::to_string(i);
	}
}

/*
* ===============
* TEXTURE MENU
* =================
*/

void onhighlight_skinchanger_texture_menu(int selection, std::string caption, int value)
{
	//TODO: remove these two
	skinTextureMenuIndex = selection;
	skinTextureMenuValue = value;

	if (true)//PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, skinDrawableMenuValue, value))
	{
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, skinDrawableMenuValue, value, 0);
	}
	skinchanger_used = true;
	WAIT(100);
}

bool onconfirm_skinchanger_texture_menu(int selection, std::string caption, int value)
{
	skinTextureMenuIndex = selection;
	skinTextureMenuValue = value;

	onhighlight_skinchanger_texture_menu(selection, caption, value);

	return true;
}

void onexit_skinchanger_texture_menu(bool returnValue)
{
	/*
	//restore the applied selection
	int texture;
	if (skinTextureMenuValue == -1)
	{
	texture = findFirstValidPedTexture(skinDrawableMenuValue);
	}
	else
	{
	texture = skinTextureMenuValue;
	}

	if (PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, skinDrawableMenuValue, texture))
	{
	PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, skinDrawableMenuValue, texture, 0);
	}
	*/
}

bool process_skinchanger_texture_menu(std::string caption)
{
	DWORD waitTime = 150;
	int foundTextures = 0;
	std::vector<std::string> menuCaptions;
	std::vector<int> menuValues;

	DWORD model = GAMEPLAY::GET_HASH_KEY((char*)chosenSkinName.c_str());
	if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_VALID(model))
	{
		STREAMING::REQUEST_MODEL(model);
		while (!STREAMING::HAS_MODEL_LOADED(model))	WAIT(0);

		int textures = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, skinDrawableMenuValue);
		for (int i = 0; i < textures; i++)
		{
			std::ostringstream ss;
			ss << "Texture #" << i;
			menuCaptions.push_back(ss.str());
			menuValues.push_back(i);
		}

		/*
		for (int j = 0; j < 100; j++)
		{
		int texture = j;
		if (PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, skinDrawableMenuValue, texture))
		{
		std::ostringstream ss;
		ss << "Texture #" << std::to_string(texture);

		menuCaptions.push_back(ss.str());
		menuValues.push_back(texture);
		}
		}
		*/

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
	}

	std::ostringstream ss;
	ss << "Available Textures";

	return draw_generic_menu<int>(menuCaptions, menuValues, skinTextureMenuIndex, ss.str(), onconfirm_skinchanger_texture_menu, onhighlight_skinchanger_texture_menu, onexit_skinchanger_texture_menu);
}

/*
* ===============
* SKIN DRAWABLES MENU
* =================
*/

bool onconfirm_skinchanger_drawable_menu(int selection, std::string caption, int value)
{
	skinDrawableMenuIndex = selection;
	skinDrawableMenuValue = value;

	skinTextureMenuIndex = 0;
	skinTextureMenuValue = -1;

	return process_skinchanger_texture_menu(caption);
}

void onhighlight_skinchanger_drawable_menu(int selection, std::string caption, int value)
{
	//TODO: remove these two
	skinDrawableMenuIndex = selection;
	skinDrawableMenuValue = value;

	int texture;
	if (value == skinDrawableMenuValue && skinTextureMenuValue != -1)
	{
		texture = skinTextureMenuValue;
	}
	else
	{
		texture = findFirstValidPedTexture(value);
	}
	if (true)//PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, value, texture))
	{
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, value, texture, 0);
	}
	skinchanger_used = true;
	WAIT(100);
}

void onexit_skinchanger_drawable_menu(bool returnValue)
{
	//restore the applied selection
	int drawable;
	if (skinDrawableMenuValue == -1)
	{
		drawable = findFirstValidPedDrawable(skinDetailMenuValue);
	}
	else
	{
		drawable = skinDrawableMenuValue;
	}

	//restore the applied selection
	int texture;
	if (skinTextureMenuValue == -1)
	{
		texture = findFirstValidPedTexture(skinDrawableMenuValue);
	}
	else
	{
		texture = skinTextureMenuValue;
	}

	if (PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, drawable, texture))
	{
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), skinDetailMenuValue, drawable, texture, 0);
	}
}

bool process_skinchanger_drawable_menu(std::string caption, int component)
{
	DWORD waitTime = 150;
	int foundTextures = 0;
	std::vector<std::string> menuCaptions;
	std::vector<int> menuValues;

	DWORD model = GAMEPLAY::GET_HASH_KEY((char*)chosenSkinName.c_str());
	if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_VALID(model))
	{
		STREAMING::REQUEST_MODEL(model);
		while (!STREAMING::HAS_MODEL_LOADED(model))	WAIT(0);

		int drawables = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), component);
		for (int i = 0; i < drawables; i++)
		{
			int textures = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(PLAYER::PLAYER_PED_ID(), component, i);
			std::ostringstream ss;
			ss << "Drawable #" << i << " (" << textures << ")";
			menuCaptions.push_back(ss.str());
			menuValues.push_back(i);
		}

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
	}

	std::ostringstream ss;
	ss << "Available Drawables";

	return draw_generic_menu<int>(menuCaptions, menuValues, skinDrawableMenuIndex, ss.str(), onconfirm_skinchanger_drawable_menu, onhighlight_skinchanger_drawable_menu, onexit_skinchanger_drawable_menu);
}

/*
* ===============
* SKIN DETAIL MENU
* =================
*/

void onhighlight_skinchanger_detail_menu(int selection, std::string caption, int value)
{
	//do nothing
}

int lastTriedComponentIndex = 0;
int lastTriedPalette = 0;

bool onconfirm_skinchanger_detail_menu(int selection, std::string caption, int value)
{
	skinDetailMenuIndex = selection;
	skinDetailMenuValue = value;

	skinDrawableMenuIndex = 0;
	skinDrawableMenuValue = -1;

	skinTextureMenuIndex = 0;
	skinTextureMenuValue = -1;

	if (DEBUG_MODE_SKINS && value == -1)
	{
		DWORD playerModel = ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID());

		std::ostringstream ss1;
		for (int i = 0; i < 7; i++)
		{
			int d = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), i);
			int t = PED::GET_PED_TEXTURE_VARIATION(PLAYER::PLAYER_PED_ID(), i);
			ss1 << "D.T[" << i << "]=" << d << "." << t << "; ";
		}

		set_status_text(ss1.str(), 3000);

		DWORD time = GetTickCount() + 4000;
		while (GetTickCount() < time)
		{
			make_periodic_feature_call();
			WAIT(0);
		}

		std::ostringstream ss2;
		for (int i = 7; i <= 12; i++)
		{
			int d = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), i);
			int t = PED::GET_PED_TEXTURE_VARIATION(PLAYER::PLAYER_PED_ID(), i);
			ss2 << "D.T[" << i << "]=" << d << "." << t << "; ";
		}

		set_status_text(ss2.str(), 3000);

		return false;
	}
	else if (DEBUG_MODE_SKINS && value == -2)
	{
		DWORD playerModel = ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID());

		std::ostringstream ss1;
		for (int i = 0; i < 7; i++)
		{
			int skins = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), i);
			ss1 << "D" << i << "=" << skins << "; ";
		}

		set_status_text(ss1.str(), 3000);

		DWORD time = GetTickCount() + 4000;
		while (GetTickCount() < time)
		{
			make_periodic_feature_call();
			WAIT(0);
		}

		std::ostringstream ss2;
		for (int i = 7; i <= 12; i++)
		{
			int skins = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), i);
			ss2 << "D" << i << "=" << skins << "; ";
		}

		set_status_text(ss2.str(), 3000);

		return false;
	}
	else
	{
		return process_skinchanger_drawable_menu(caption, value);
	}
}

bool process_skinchanger_detail_menu()
{
	DWORD waitTime = 150;
	int foundTextures = 0;
	std::vector<std::string> menuCaptions;
	std::vector<int> menuValues;

	int fixedChoices = 0;
	const int partVariations = 12;

	int i = 0;

	if (DEBUG_MODE_SKINS)
	{
		menuCaptions.push_back("Output current D/T");
		menuValues.push_back(-1);
		menuCaptions.push_back("Output drawable slots");
		menuValues.push_back(-2);
		fixedChoices += 2;
	}

	if (skinchanger_used)
	{
		DWORD model = GAMEPLAY::GET_HASH_KEY((char*)chosenSkinName.c_str());
		if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_VALID(model))
		{
			STREAMING::REQUEST_MODEL(model);
			while (!STREAMING::HAS_MODEL_LOADED(model))	WAIT(0);

			for (; i < partVariations + fixedChoices; i++)
			{
				bool iFound = false;
				int compIndex = i - fixedChoices;

				int drawables = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), compIndex);
				int textures = 0;
				if (drawables == 1)
				{
					textures = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(PLAYER::PLAYER_PED_ID(), compIndex, 0);
				}
				if (drawables > 1 || textures != 0)
				{
					std::ostringstream ss;
					std::string itemText = getSkinDetailAttribDescription(compIndex);
					ss << "Slot " << compIndex << ": " << itemText << " (" << drawables << ")";
					menuCaptions.push_back(ss.str());
					menuValues.push_back(compIndex);
				}
			}

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
		}
	}
	else
	{
		set_status_text("Please apply a custom skin in order to access this");
		return false;
	}

	return draw_generic_menu<int>(menuCaptions, menuValues, skinDetailMenuIndex, "Skin Details", onconfirm_skinchanger_detail_menu, onhighlight_skinchanger_detail_menu, NULL);
}

/*
* ===============
* SKIN PLAYER CHOICES
* =================
*/

bool onconfirm_skinchanger_choices_players(int selection, std::string caption, std::string value)
{
	skinTypesMenuPositionMemory[0] = selection;
	applyChosenSkin(value);
	return false;
}

bool process_skinchanger_choices_players()
{
	return draw_generic_menu<std::string>(SKINS_PLAYER_CAPTIONS, SKINS_PLAYER_VALUES, skinTypesMenuPositionMemory[0], "Player Skins", onconfirm_skinchanger_choices_players, NULL, NULL);
}

/*
* ===============
* SKIN ANIMAL CHOICES
* =================
*/

bool onconfirm_skinchanger_choices_animals(int selection, std::string caption, std::string value)
{
	skinTypesMenuPositionMemory[1] = selection;
	applyChosenSkin(value);
	return false;
}

bool process_skinchanger_choices_animals()
{
	return draw_generic_menu<std::string>(SKINS_ANIMAL_CAPTIONS, SKINS_ANIMAL_VALUES, skinTypesMenuPositionMemory[1], "Animal Skins", onconfirm_skinchanger_choices_animals, NULL, NULL);
}

/*
* ===============
* SKIN GENERAL CHOICES
* =================
*/

bool onconfirm_skinchanger_choices_misc(int selection, std::string caption, std::string value)
{
	skinTypesMenuPositionMemory[2] = selection;
	applyChosenSkin(value);
	return false;
}

bool process_skinchanger_choices_misc()
{
	return draw_generic_menu<std::string>(SKINS_GENERAL_CAPTIONS, SKINS_GENERAL_VALUES, skinTypesMenuPositionMemory[2], "General Skins", onconfirm_skinchanger_choices_misc, NULL, NULL);
}

/*
* ===============
* SKIN MAIN MENU
* =================
*/

bool onconfirm_skinchanger_menu(int selection, std::string caption, int value)
{
	skinMainMenuPosition = selection;

	switch (value)
	{
	case 0: //Players
		process_skinchanger_choices_players();
		break;
	case 1: //Animals
		process_skinchanger_choices_animals();
		break;
	case 2: //Misc
		process_skinchanger_choices_misc();
		break;
	case 3: //Detail
		process_skinchanger_detail_menu();
		break;
	case 4: //Reset
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		PED::SET_PED_DEFAULT_COMPONENT_VARIATION(playerPed);
		set_status_text("Using default model skin");
		break;
	}
	return false;
}

bool process_skinchanger_menu2()
{
	std::vector<int> menuIDs;
	for (int i = 0; i < MENU_SKINS_TYPES_CAPTIONS.size(); i++)
	{
		menuIDs.push_back(i);
	}

	return draw_generic_menu<int>(MENU_SKINS_TYPES_CAPTIONS, menuIDs, skinMainMenuPosition, "Player Skin Options", onconfirm_skinchanger_menu, NULL, NULL);
}
#pragma endregion
#pragma region lpcstr
LPCSTR pedModels[69][10] = { { "player_zero", "player_one", "player_two", "a_c_boar", "a_c_chimp", "a_c_cow", "a_c_coyote", "a_c_deer", "a_c_fish", "a_c_hen" },{ "a_c_cat_01", "a_c_chickenhawk", "a_c_cormorant", "a_c_crow", "a_c_dolphin", "a_c_humpback", "a_c_killerwhale", "a_c_pigeon", "a_c_seagull", "a_c_sharkhammer" },{ "a_c_pig", "mp_f_freemode_01", "a_c_rhesus", "a_c_chop", "a_c_husky", "a_c_mtlion", "a_c_retriever", "a_c_sharktiger", "a_c_shepherd", "s_m_m_movalien_01" },{ "a_f_m_beach_01", "a_f_m_bevhills_01", "a_f_m_bevhills_02", "a_f_m_bodybuild_01", "a_f_m_business_02", "a_f_m_downtown_01", "a_f_m_eastsa_01", "a_f_m_eastsa_02", "a_f_m_fatbla_01", "a_f_m_fatcult_01" },{ "a_f_m_fatwhite_01", "a_f_m_ktown_01", "a_f_m_ktown_02", "a_f_m_prolhost_01", "a_f_m_salton_01", "a_f_m_skidrow_01", "a_f_m_soucentmc_01", "a_f_m_soucent_01", "a_f_m_soucent_02", "a_f_m_tourist_01" },{ "a_f_m_trampbeac_01", "a_f_m_tramp_01", "a_f_o_genstreet_01", "a_f_o_indian_01", "a_f_o_ktown_01", "a_f_o_salton_01", "a_f_o_soucent_01", "a_f_o_soucent_02", "a_f_y_beach_01", "a_f_y_bevhills_01" },{ "a_f_y_bevhills_02", "a_f_y_bevhills_03", "a_f_y_bevhills_04", "a_f_y_business_01", "a_f_y_business_02", "a_f_y_business_03", "a_f_y_business_04", "a_f_y_eastsa_01", "a_f_y_eastsa_02", "a_f_y_eastsa_03" },{ "a_f_y_epsilon_01", "a_f_y_fitness_01", "a_f_y_fitness_02", "a_f_y_genhot_01", "a_f_y_golfer_01", "a_f_y_hiker_01", "a_f_y_hippie_01", "a_f_y_hipster_01", "a_f_y_hipster_02", "a_f_y_hipster_03" },{ "a_f_y_hipster_04", "a_f_y_indian_01", "a_f_y_juggalo_01", "a_f_y_runner_01", "a_f_y_rurmeth_01", "a_f_y_scdressy_01", "a_f_y_skater_01", "a_f_y_soucent_01", "a_f_y_soucent_02", "a_f_y_soucent_03" },{ "a_f_y_tennis_01", "a_f_y_topless_01", "a_f_y_tourist_01", "a_f_y_tourist_02", "a_f_y_vinewood_01", "a_f_y_vinewood_02", "a_f_y_vinewood_03", "a_f_y_vinewood_04", "a_f_y_yoga_01", "a_m_m_acult_01" },{ "a_m_m_afriamer_01", "a_m_m_beach_01", "a_m_m_beach_02", "a_m_m_bevhills_01", "a_m_m_bevhills_02", "a_m_m_business_01", "a_m_m_eastsa_01", "a_m_m_eastsa_02", "a_m_m_farmer_01", "a_m_m_fatlatin_01" },{ "a_m_m_genfat_01", "a_m_m_genfat_02", "a_m_m_golfer_01", "a_m_m_hasjew_01", "a_m_m_hillbilly_01", "a_m_m_hillbilly_02", "a_m_m_indian_01", "a_m_m_ktown_01", "a_m_m_malibu_01", "a_m_m_mexcntry_01" },{ "a_m_m_mexlabor_01", "a_m_m_og_boss_01", "a_m_m_paparazzi_01", "a_m_m_polynesian_01", "a_m_m_prolhost_01", "a_m_m_rurmeth_01", "a_m_m_salton_01", "a_m_m_salton_02", "a_m_m_salton_03", "a_m_m_salton_04" },{ "a_m_m_skater_01", "a_m_m_skidrow_01", "a_m_m_socenlat_01", "a_m_m_soucent_01", "a_m_m_soucent_02", "a_m_m_soucent_03", "a_m_m_soucent_04", "a_m_m_stlat_02", "a_m_m_tennis_01", "a_m_m_tourist_01" },{ "a_m_m_trampbeac_01", "a_m_m_tramp_01", "a_m_m_tranvest_01", "a_m_m_tranvest_02", "a_m_o_acult_01", "a_m_o_acult_02", "a_m_o_beach_01", "a_m_o_genstreet_01", "a_m_o_ktown_01", "a_m_o_salton_01" },{ "a_m_o_soucent_01", "a_m_o_soucent_02", "a_m_o_soucent_03", "a_m_o_tramp_01", "a_m_y_acult_01", "a_m_y_acult_02", "a_m_y_beachvesp_01", "a_m_y_beachvesp_02", "a_m_y_beach_01", "a_m_y_beach_02" },{ "a_m_y_beach_03", "a_m_y_bevhills_01", "a_m_y_bevhills_02", "a_m_y_breakdance_01", "a_m_y_busicas_01", "a_m_y_business_01", "a_m_y_business_02", "a_m_y_business_03", "a_m_y_cyclist_01", "a_m_y_dhill_01" },{ "a_m_y_downtown_01", "a_m_y_eastsa_01", "a_m_y_eastsa_02", "a_m_y_epsilon_01", "a_m_y_epsilon_02", "a_m_y_gay_01", "a_m_y_gay_02", "a_m_y_genstreet_01", "a_m_y_genstreet_02", "a_m_y_golfer_01" },{ "a_m_y_hasjew_01", "a_m_y_hiker_01", "a_m_y_hippy_01", "a_m_y_hipster_01", "a_m_y_hipster_02", "a_m_y_hipster_03", "a_m_y_indian_01", "a_m_y_jetski_01", "a_m_y_juggalo_01", "a_m_y_ktown_01" },{ "a_m_y_ktown_02", "a_m_y_latino_01", "a_m_y_methhead_01", "a_m_y_mexthug_01", "a_m_y_motox_01", "a_m_y_motox_02", "a_m_y_musclbeac_01", "a_m_y_musclbeac_02", "a_m_y_polynesian_01", "a_m_y_roadcyc_01" },{ "a_m_y_runner_01", "a_m_y_runner_02", "a_m_y_salton_01", "a_m_y_skater_01", "a_m_y_skater_02", "a_m_y_soucent_01", "a_m_y_soucent_02", "a_m_y_soucent_03", "a_m_y_soucent_04", "a_m_y_stbla_01" },{ "a_m_y_stbla_02", "a_m_y_stlat_01", "a_m_y_stwhi_01", "a_m_y_stwhi_02", "a_m_y_sunbathe_01", "a_m_y_surfer_01", "a_m_y_vindouche_01", "a_m_y_vinewood_01", "a_m_y_vinewood_02", "a_m_y_vinewood_03" },{ "a_m_y_vinewood_04", "a_m_y_yoga_01", "u_m_y_proldriver_01", "u_m_y_rsranger_01", "u_m_y_sbike", "u_m_y_staggrm_01", "u_m_y_tattoo_01", "csb_abigail", "csb_anita", "csb_anton" },{ "csb_ballasog", "csb_bride", "csb_burgerdrug", "csb_car3guy1", "csb_car3guy2", "csb_chef", "csb_chin_goon", "csb_cletus", "csb_cop", "csb_customer" },{ "csb_denise_friend", "csb_fos_rep", "csb_g", "csb_groom", "csb_grove_str_dlr", "csb_hao", "csb_hugh", "csb_imran", "csb_janitor", "csb_maude" },{ "csb_mweather", "csb_ortega", "csb_oscar", "csb_porndudes", "csb_porndudes_p", "csb_prologuedriver", "csb_prolsec", "csb_ramp_gang", "csb_ramp_hic", "csb_ramp_hipster" },{ "csb_ramp_marine", "csb_ramp_mex", "csb_reporter", "csb_roccopelosi", "csb_screen_writer", "csb_stripper_01", "csb_stripper_02", "csb_tonya", "csb_trafficwarden", "cs_amandatownley" },{ "cs_andreas", "cs_ashley", "cs_bankman", "cs_barry", "cs_barry_p", "cs_beverly", "cs_beverly_p", "cs_brad", "cs_bradcadaver", "cs_carbuyer" },{ "cs_casey", "cs_chengsr", "cs_chrisformage", "cs_clay", "cs_dale", "cs_davenorton", "cs_debra", "cs_denise", "cs_devin", "cs_dom" },{ "cs_dreyfuss", "cs_drfriedlander", "cs_fabien", "cs_fbisuit_01", "cs_floyd", "cs_guadalope", "cs_gurk", "cs_hunter", "cs_janet", "cs_jewelass" },{ "cs_jimmyboston", "cs_jimmydisanto", "cs_joeminuteman", "cs_johnnyklebitz", "cs_josef", "cs_josh", "cs_lamardavis", "cs_lazlow", "cs_lestercrest", "cs_lifeinvad_01" },{ "cs_magenta", "cs_manuel", "cs_marnie", "cs_martinmadrazo", "cs_maryann", "cs_michelle", "cs_milton", "cs_molly", "cs_movpremf_01", "cs_movpremmale" },{ "cs_mrk", "cs_mrsphillips", "cs_mrs_thornhill", "cs_natalia", "cs_nervousron", "cs_nigel", "cs_old_man1a", "cs_old_man2", "cs_omega", "cs_orleans" },{ "cs_paper", "cs_paper_p", "cs_patricia", "cs_priest", "cs_prolsec_02", "cs_russiandrunk", "cs_siemonyetarian", "cs_solomon", "cs_stevehains", "cs_stretch" },{ "cs_tanisha", "cs_taocheng", "cs_taostranslator", "cs_tenniscoach", "cs_terry", "cs_tom", "cs_tomepsilon", "cs_tracydisanto", "cs_wade", "cs_zimbor" },{ "g_f_y_ballas_01", "g_f_y_families_01", "g_f_y_lost_01", "g_f_y_vagos_01", "g_m_m_armboss_01", "g_m_m_armgoon_01", "g_m_m_armlieut_01", "g_m_m_chemwork_01", "g_m_m_chemwork_01_p", "g_m_m_chiboss_01" },{ "g_m_m_chiboss_01_p", "g_m_m_chicold_01", "g_m_m_chicold_01_p", "g_m_m_chigoon_01", "g_m_m_chigoon_01_p", "g_m_m_chigoon_02", "g_m_m_korboss_01", "g_m_m_mexboss_01", "g_m_m_mexboss_02", "g_m_y_armgoon_02" },{ "g_m_y_azteca_01", "g_m_y_ballaeast_01", "g_m_y_ballaorig_01", "g_m_y_ballasout_01", "g_m_y_famca_01", "g_m_y_famdnf_01", "g_m_y_famfor_01", "g_m_y_korean_01", "g_m_y_korean_02", "g_m_y_korlieut_01" },{ "g_m_y_lost_01", "g_m_y_lost_02", "g_m_y_lost_03", "g_m_y_mexgang_01", "g_m_y_mexgoon_01", "g_m_y_mexgoon_02", "g_m_y_mexgoon_03", "g_m_y_mexgoon_03_p", "g_m_y_pologoon_01", "g_m_y_pologoon_01_p" },{ "g_m_y_pologoon_02", "g_m_y_pologoon_02_p", "g_m_y_salvaboss_01", "g_m_y_salvagoon_01", "g_m_y_salvagoon_02", "g_m_y_salvagoon_03", "g_m_y_salvagoon_03_p", "g_m_y_strpunk_01", "g_m_y_strpunk_02", "hc_driver" },{ "hc_gunman", "hc_hacker", "ig_abigail", "ig_amandatownley", "ig_andreas", "ig_ashley", "ig_ballasog", "ig_bankman", "ig_barry", "ig_barry_p" },{ "ig_bestmen", "ig_beverly", "ig_beverly_p", "ig_brad", "ig_bride", "ig_car3guy1", "ig_car3guy2", "ig_casey", "ig_chef", "ig_chengsr" },{ "ig_chrisformage", "ig_clay", "ig_claypain", "ig_cletus", "ig_dale", "ig_davenorton", "ig_denise", "ig_devin", "ig_dom", "ig_dreyfuss" },{ "ig_drfriedlander", "ig_fabien", "ig_fbisuit_01", "ig_floyd", "ig_groom", "ig_hao", "ig_hunter", "ig_janet", "ig_jay_norris", "ig_jewelass" },{ "ig_jimmyboston", "ig_jimmydisanto", "ig_joeminuteman", "ig_johnnyklebitz", "ig_josef", "ig_josh", "ig_kerrymcintosh", "ig_lamardavis", "ig_lazlow", "ig_lestercrest" },{ "ig_lifeinvad_01", "ig_lifeinvad_02", "ig_magenta", "ig_manuel", "ig_marnie", "ig_maryann", "ig_maude", "ig_michelle", "ig_milton", "ig_molly" },{ "ig_mrk", "ig_mrsphillips", "ig_mrs_thornhill", "ig_natalia", "ig_nervousron", "ig_nigel", "ig_old_man1a", "ig_old_man2", "ig_omega", "ig_oneil" },{ "ig_orleans", "ig_ortega", "ig_paper", "ig_patricia", "ig_priest", "ig_prolsec_02", "ig_ramp_gang", "ig_ramp_hic", "ig_ramp_hipster", "ig_ramp_mex" },{ "ig_roccopelosi", "ig_russiandrunk", "ig_screen_writer", "ig_siemonyetarian", "ig_solomon", "ig_stevehains", "ig_stretch", "ig_talina", "ig_tanisha", "ig_taocheng" },{ "ig_taostranslator", "ig_taostranslator_p", "ig_tenniscoach", "ig_terry", "ig_tomepsilon", "ig_tonya", "ig_tracydisanto", "ig_trafficwarden", "ig_tylerdix", "ig_wade" },{ "ig_zimbor", "mp_f_deadhooker", "mp_f_freemode_01", "mp_f_misty_01", "mp_f_stripperlite", "mp_g_m_pros_01", "mp_headtargets", "mp_m_claude_01", "mp_m_exarmy_01", "mp_m_famdd_01" },{ "mp_m_fibsec_01", "mp_m_freemode_01", "mp_m_marston_01", "mp_m_niko_01", "mp_m_shopkeep_01", "mp_s_m_armoured_01", "", "", "", "" },{ "", "s_f_m_fembarber", "s_f_m_maid_01", "s_f_m_shop_high", "s_f_m_sweatshop_01", "s_f_y_airhostess_01", "s_f_y_bartender_01", "s_f_y_baywatch_01", "s_f_y_cop_01", "s_f_y_factory_01" },{ "s_f_y_hooker_01", "s_f_y_hooker_02", "s_f_y_hooker_03", "s_f_y_migrant_01", "s_f_y_movprem_01", "s_f_y_ranger_01", "s_f_y_scrubs_01", "s_f_y_sheriff_01", "s_f_y_shop_low", "s_f_y_shop_mid" },{ "s_f_y_stripperlite", "s_f_y_stripper_01", "s_f_y_stripper_02", "s_f_y_sweatshop_01", "s_m_m_ammucountry", "s_m_m_armoured_01", "s_m_m_armoured_02", "s_m_m_autoshop_01", "s_m_m_autoshop_02", "s_m_m_bouncer_01" },{ "s_m_m_chemsec_01", "s_m_m_ciasec_01", "s_m_m_cntrybar_01", "s_m_m_dockwork_01", "s_m_m_doctor_01", "s_m_m_fiboffice_01", "s_m_m_fiboffice_02", "s_m_m_gaffer_01", "s_m_m_gardener_01", "s_m_m_gentransport" },{ "s_m_m_hairdress_01", "s_m_m_highsec_01", "s_m_m_highsec_02", "s_m_m_janitor", "s_m_m_lathandy_01", "s_m_m_lifeinvad_01", "s_m_m_linecook", "s_m_m_lsmetro_01", "s_m_m_mariachi_01", "s_m_m_marine_01" },{ "s_m_m_marine_02", "s_m_m_migrant_01", "u_m_y_zombie_01", "s_m_m_movprem_01", "s_m_m_movspace_01", "s_m_m_paramedic_01", "s_m_m_pilot_01", "s_m_m_pilot_02", "s_m_m_postal_01", "s_m_m_postal_02" },{ "s_m_m_prisguard_01", "s_m_m_scientist_01", "s_m_m_security_01", "s_m_m_snowcop_01", "s_m_m_strperf_01", "s_m_m_strpreach_01", "s_m_m_strvend_01", "s_m_m_trucker_01", "s_m_m_ups_01", "s_m_m_ups_02" },{ "s_m_o_busker_01", "s_m_y_airworker", "s_m_y_ammucity_01", "s_m_y_armymech_01", "s_m_y_autopsy_01", "s_m_y_barman_01", "s_m_y_baywatch_01", "s_m_y_blackops_01", "s_m_y_blackops_02", "s_m_y_busboy_01" },{ "s_m_y_chef_01", "s_m_y_clown_01", "s_m_y_construct_01", "s_m_y_construct_02", "s_m_y_cop_01", "s_m_y_dealer_01", "s_m_y_devinsec_01", "s_m_y_dockwork_01", "s_m_y_doorman_01", "s_m_y_dwservice_01" },{ "s_m_y_dwservice_02", "s_m_y_factory_01", "s_m_y_fireman_01", "s_m_y_garbage", "s_m_y_grip_01", "s_m_y_hwaycop_01", "s_m_y_marine_01", "s_m_y_marine_02", "s_m_y_marine_03", "s_m_y_mime" },{ "s_m_y_pestcont_01", "s_m_y_pilot_01", "s_m_y_prismuscl_01", "s_m_y_prisoner_01", "s_m_y_ranger_01", "s_m_y_robber_01", "s_m_y_sheriff_01", "s_m_y_shop_mask", "s_m_y_strvend_01", "s_m_y_swat_01" },{ "s_m_y_uscg_01", "s_m_y_valet_01", "s_m_y_waiter_01", "s_m_y_winclean_01", "s_m_y_xmech_01", "s_m_y_xmech_02", "u_f_m_corpse_01", "u_f_m_miranda", "u_f_m_promourn_01", "u_f_o_moviestar" },{ "u_f_o_prolhost_01", "u_f_y_bikerchic", "u_f_y_comjane", "u_f_y_corpse_01", "u_f_y_corpse_02", "u_f_y_hotposh_01", "u_f_y_jewelass_01", "u_f_y_mistress", "u_f_y_poppymich", "u_f_y_princess" },{ "u_f_y_spyactress", "u_m_m_aldinapoli", "u_m_m_bankman", "u_m_m_bikehire_01", "u_m_m_fibarchitect", "u_m_m_filmdirector", "u_m_m_glenstank_01", "u_m_m_griff_01", "u_m_m_jesus_01", "u_m_m_jewelsec_01" },{ "u_m_m_jewelthief", "u_m_m_markfost", "u_m_m_partytarget", "u_m_m_prolsec_01", "u_m_m_promourn_01", "u_m_m_rivalpap", "u_m_m_spyactor", "u_m_m_willyfist", "u_m_o_finguru_01", "u_m_o_taphillbilly" },{ "u_m_o_tramp_01", "u_m_y_abner", "u_m_y_antonb", "u_m_y_babyd", "u_m_y_baygor", "u_m_y_burgerdrug_01", "u_m_y_chip", "u_m_y_cyclist_01", "u_m_y_fibmugger_01", "u_m_y_guido_01" },{ "u_m_y_gunvend_01", "u_m_y_hippie_01", "u_m_y_imporage", "u_m_y_justin", "u_m_y_mani", "u_m_y_militarybum", "u_m_y_paparazzi", "u_m_y_party_01", "u_m_y_pogo_01", "u_m_y_prisoner_01" } };

LPCSTR pedModelNames[69][10] = { { "michael", "franklin", "trevor", "boar", "chimp", "cow", "coyote", "deer", "fish", "hen" },{ "cat", "hawk", "cormorant", "crow", "dolphin", "humpback", "whale", "pigeon", "seagull", "sharkhammer" },{ "pig", "female", "rhesus", "chop", "husky", "mtlion", "retriever", "sharktiger", "shepherd", "alien" },{ "beach", "bevhills", "bevhills", "bodybuild", "business", "downtown", "eastsa", "eastsa", "fatbla", "fatcult" },{ "fatwhite", "ktown", "ktown", "prolhost", "salton", "skidrow", "soucentmc", "soucent", "soucent", "tourist" },{ "trampbeac", "tramp", "genstreet", "indian", "ktown", "salton", "soucent", "soucent", "beach", "bevhills" },{ "bevhills", "bevhills", "bevhills", "business", "business", "business", "business", "eastsa", "eastsa", "eastsa" },{ "epsilon", "fitness", "fitness", "genhot", "golfer", "hiker", "hippie", "hipster", "hipster", "hipster" },{ "hipster", "indian", "juggalo", "runner", "rurmeth", "scdressy", "skater", "soucent", "soucent", "soucent" },{ "tennis", "topless", "tourist", "tourist", "vinewood", "vinewood", "vinewood", "vinewood", "yoga", "acult" },{ "afriamer", "beach", "beach", "bevhills", "bevhills", "business", "eastsa", "eastsa", "farmer", "fatlatin" },{ "genfat", "genfat", "golfer", "hasjew", "hillbilly", "hillbilly", "indian", "ktown", "malibu", "mexcntry" },{ "mexlabor", "og_boss", "paparazzi", "polynesian", "prolhost", "rurmeth", "salton", "salton", "salton", "salton" },{ "skater", "skidrow", "socenlat", "soucent", "soucent", "soucent", "soucent", "stlat", "tennis", "tourist" },{ "trampbeac", "tramp", "tranvest", "tranvest", "acult", "acult", "beach", "genstreet", "ktown", "salton" },{ "soucent", "soucent", "soucent", "tramp", "acult", "acult", "beachvesp", "beachvesp", "beach", "beach" },{ "beach", "bevhills", "bevhills", "breakdance", "busicas", "business", "business", "business", "cyclist", "dhill" },{ "downtown", "eastsa", "eastsa", "epsilon", "epsilon", "gay", "gay", "genstreet", "genstreet", "golfer" },{ "hasjew", "hiker", "hippy", "hipster", "hipster", "hipster", "indian", "jetski", "juggalo", "ktown" },{ "ktown", "latino", "methhead", "mexthug", "motox", "motox", "musclbeac", "musclbeac", "polynesian", "roadcyc" },{ "runner", "runner", "salton", "skater", "skater", "soucent", "soucent", "soucent", "soucent", "stbla" },{ "stbla", "stlat", "stwhi", "stwhi", "sunbathe", "surfer", "vindouche", "vinewood", "vinewood", "vinewood" },{ "vinewood", "yoga", "proldriver", "rsranger", "sbike", "staggrm", "tattoo", "abigail", "anita", "anton" },{ "ballasog", "bride", "burgerdrug", "car3guy1", "car3guy2", "chef", "chin_goon", "cletus", "cop", "customer" },{ "denise_friend", "fos_rep", "g", "groom", "dlr", "hao", "hugh", "imran", "janitor", "maude" },{ "mweather", "ortega", "oscar", "porndudes", "porndudes_p", "prologuedriver", "prolsec", "gang", "hic", "hipster" },{ "marine", "mex", "reporter", "roccopelosi", "screen_writer", "stripper", "stripper", "tonya", "trafficwarden", "amandatownley" },{ "andreas", "ashley", "bankman", "barry", "barry_p", "beverly", "beverly_p", "brad", "bradcadaver", "carbuyer" },{ "casey", "chengsr", "chrisformage", "clay", "dale", "davenorton", "debra", "denise", "devin", "dom" },{ "dreyfuss", "drfriedlander", "fabien", "fbisuit", "floyd", "guadalope", "gurk", "hunter", "janet", "jewelass" },{ "jimmyboston", "jimmydisanto", "joeminuteman", "johnnyklebitz", "josef", "josh", "lamardavis", "lazlow", "lestercrest", "lifeinvad" },{ "magenta", "manuel", "marnie", "martinmadrazo", "maryann", "michelle", "milton", "molly", "movpremf", "movpremmale" },{ "mrk", "mrsphillips", "mrs_thornhill", "natalia", "nervousron", "nigel", "old_man1a", "old_man2", "omega", "orleans" },{ "paper", "paper_p", "patricia", "priest", "prolsec", "russiandrunk", "siemonyetarian", "solomon", "stevehains", "stretch" },{ "tanisha", "taocheng", "taostranslator", "tenniscoach", "terry", "tom", "tomepsilon", "tracydisanto", "wade", "zimbor" },{ "ballas", "families", "lost", "vagos", "armboss", "armgoon", "armlieut", "chemwork", "chemwork_p", "chiboss" },{ "chiboss_p", "chicold", "chicold_p", "chigoon", "chigoon_p", "chigoon", "korboss", "mexboss", "mexboss", "armgoon" },{ "azteca", "ballaeast", "ballaorig", "ballasout", "famca", "famdnf", "famfor", "korean", "korean", "korlieut" },{ "lost", "lost", "lost", "mexgang", "mexgoon", "mexgoon", "mexgoon", "mexgoon_p", "pologoon", "pologoon_p" },{ "pologoon", "pologoon_p", "salvaboss", "salvagoon", "salvagoon", "salvagoon", "salvagoon_p", "strpunk", "strpunk", "hc_driver" },{ "hc_gunman", "hc_hacker", "abigail", "amandatownley", "andreas", "ashley", "ballasog", "bankman", "barry", "barry_p" },{ "bestmen", "beverly", "beverly_p", "brad", "bride", "car3guy1", "car3guy2", "casey", "chef", "chengsr" },{ "chrisformage", "clay", "claypain", "cletus", "dale", "davenorton", "denise", "devin", "dom", "dreyfuss" },{ "drfriedlander", "fabien", "fbisuit", "floyd", "groom", "hao", "hunter", "janet", "jay_norris", "jewelass" },{ "jimmyboston", "jimmydisanto", "joeminuteman", "johnnyklebitz", "josef", "josh", "kerrymcintosh", "lamardavis", "lazlow", "lestercrest" },{ "lifeinvad", "lifeinvad", "magenta", "manuel", "marnie", "maryann", "maude", "michelle", "milton", "molly" },{ "mrk", "mrsphillips", "mrs_thornhill", "natalia", "nervousron", "nigel", "old_man1a", "old_man2", "omega", "oneil" },{ "orleans", "ortega", "paper", "patricia", "priest", "prolsec", "gang", "hic", "hipster", "mex" },{ "roccopelosi", "russiandrunk", "screen_writer", "siemonyetarian", "solomon", "stevehains", "stretch", "talina", "tanisha", "taocheng" },{ "taostranslator", "taostranslator_p", "tenniscoach", "terry", "tomepsilon", "tonya", "tracydisanto", "trafficwarden", "tylerdix", "wade" },{ "zimbor", "deadhooker", "freemode", "misty", "stripperlite", "pros", "mp_headtargets", "claude", "exarmy", "famdd" },{ "fibsec", "freemode", "marston", "niko", "shopkeep", "armoured", "none", "none", "none", "none" },{ "none", "fembarber", "maid", "shop_high", "sweatshop", "airhostess", "bartender", "baywatch", "cop", "factory" },{ "hooker", "hooker", "hooker", "migrant", "movprem", "ranger", "scrubs", "sheriff", "shop_low", "shop_mid" },{ "stripperlite", "stripper", "stripper", "sweatshop", "ammucountry", "armoured", "armoured", "autoshop", "autoshop", "bouncer" },{ "chemsec", "ciasec", "cntrybar", "dockwork", "doctor", "fiboffice", "fiboffice", "gaffer", "gardener", "gentransport" },{ "hairdress", "highsec", "highsec", "janitor", "lathandy", "lifeinvad", "linecook", "lsmetro", "mariachi", "marine" },{ "marine", "migrant", "zombie", "movprem", "movspace", "paramedic", "pilot", "pilot", "postal", "postal" },{ "prisguard", "scientist", "security", "snowcop", "strperf", "strpreach", "strvend", "trucker", "ups", "ups" },{ "busker", "airworker", "ammucity", "armymech", "autopsy", "barman", "baywatch", "blackops", "blackops", "busboy" },{ "chef", "clown", "construct", "construct", "cop", "dealer", "devinsec", "dockwork", "doorman", "dwservice" },{ "dwservice", "factory", "fireman", "garbage", "grip", "hwaycop", "marine", "marine", "marine", "mime" },{ "pestcont", "pilot", "prismuscl", "prisoner", "ranger", "robber", "sheriff", "shop_mask", "strvend", "swat" },{ "uscg", "valet", "waiter", "winclean", "xmech", "xmech", "corpse", "miranda", "promourn", "moviestar" },{ "prolhost", "bikerchic", "comjane", "corpse", "corpse", "hotposh", "jewelass", "mistress", "poppymich", "princess" },{ "spyactress", "aldinapoli", "bankman", "bikehire", "fibarchitect", "filmdirector", "glenstank", "griff", "jesus", "jewelsec" },{ "jewelthief", "markfost", "partytarget", "prolsec", "promourn", "rivalpap", "spyactor", "willyfist", "finguru", "taphillbilly" },{ "tramp", "abner", "antonb", "babyd", "baygor", "burgerdrug", "chip", "cyclist", "fibmugger", "guido" },{ "gunvend", "hippie", "imporage", "justin", "mani", "militarybum", "paparazzi", "party", "pogo", "prisoner" } };
#pragma endregion ped models
int skinchangerActiveLineIndex = 0;
int skinchangerActiveItemIndex = 0;
bool process_skinchanger_menu()
{
	DWORD waitTime = 200;
	const int lineCount = 69;
	const int itemCount = 10;
	const int itemCountLastLine = itemCount;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			char caption[32];
			sprintf_s(caption, "~r~Skin ~w~Changer   %d / %d", skinchangerActiveLineIndex + 1, lineCount);
			draw_menu_line(caption, 350.0, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < itemCount; i++)
				if (strlen(pedModels[skinchangerActiveLineIndex][i]) || strcmp(pedModelNames[skinchangerActiveLineIndex][i], "NONE") == 0)
					draw_menu_line(pedModelNames[skinchangerActiveLineIndex][i], 100.0f, 5.0f, 200.0f, 100.0f + i * 110.0f, 5.0f, i == skinchangerActiveItemIndex, FALSE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		bool bSelect, bBack, bUp, bDown, bLeft, bRight;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, &bLeft, &bRight);

		if (bSelect)
		{
			menu_beep_select();
			DWORD model = GAMEPLAY::GET_HASH_KEY((char*)pedModels[skinchangerActiveLineIndex][skinchangerActiveItemIndex]);
			if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_VALID(model))
			{
				STREAMING::REQUEST_MODEL(model);
				while (!STREAMING::HAS_MODEL_LOADED(model))	WAIT(0);
				//STREAMING::LOAD_ALL_OBJECTS_NOW();
				PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
				//PED::SET_PED_RANDOM_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), FALSE);
				PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
				WAIT(0);
				for (int i = 0; i < 12; i++)
					for (int j = 0; j < 100; j++)
					{
						int drawable = rand() % 10;
						int texture = rand() % 10;
						if (PED::IS_PED_COMPONENT_VARIATION_VALID(PLAYER::PLAYER_PED_ID(), i, drawable, texture))
						{
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), i, drawable, texture, 0);
							break;
						}
					}
				featureSkinChanger = TRUE;
				WAIT(100);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
				waitTime = 200;
			}
		}
		else
			if (bBack)
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bRight)
				{
					menu_beep_up_down();
					skinchangerActiveItemIndex++;
					int itemsMax = (skinchangerActiveLineIndex == (lineCount - 1)) ? itemCountLastLine : itemCount;
					if (skinchangerActiveItemIndex == itemsMax)
						skinchangerActiveItemIndex = 0;
					waitTime = 100;
				}
				else
					if (bLeft)
					{
						menu_beep_up_down();
						if (skinchangerActiveItemIndex == 0)
							skinchangerActiveItemIndex = (skinchangerActiveLineIndex == (lineCount - 1)) ? itemCountLastLine : itemCount;
						skinchangerActiveItemIndex--;
						waitTime = 100;
					}
					else
						if (bUp)
						{
							menu_beep_up_down();
							if (skinchangerActiveLineIndex == 0)
								skinchangerActiveLineIndex = lineCount;
							skinchangerActiveLineIndex--;
							waitTime = 200;
						}
						else
							if (bDown)
							{
								menu_beep_up_down();
								skinchangerActiveLineIndex++;
								if (skinchangerActiveLineIndex == lineCount)
									skinchangerActiveLineIndex = 0;
								waitTime = 200;
							}
		if (skinchangerActiveLineIndex == (lineCount - 1))
			if (skinchangerActiveItemIndex >= itemCountLastLine)
				skinchangerActiveItemIndex = 0;
	}
	return FALSE;
}

int activeLineIndexClothing = 0;
void process_clothing_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 12;

	std::string caption = "~r~Attire ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
	{	"Random Face",	NULL,	NULL	},
	{	"Random Beard",	NULL,	NULL	},
	{	"Random Haircut",	NULL,	NULL	},
	{	"Random Shirt",	NULL,	NULL	},
	{	"Random Pants",	NULL,	NULL	},
	{	"Random Hands",	NULL,	NULL	},
	{	"Random Shoes",	NULL,	NULL	},
	{	"Random Eyes",	NULL,	NULL	},
	{	"Random Accessories",	NULL,	NULL	},
	{	"Random M Items",	NULL,	NULL	},
	{	"Random Decals",	NULL,	NULL	},
	{	"Random Inner Shirts",	NULL,	NULL	},
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexClothing)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexClothing].text, lines[activeLineIndexClothing].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexClothing * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			// common variables
			BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
			Player player = PLAYER::PLAYER_ID();
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			menu_beep_select();
			switch (activeLineIndexClothing)
			{
			case 0:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 0, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 0, rand() % 10, rand() % 10, 0);
				}
				break;
			case 1:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 1, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 1, rand() % 10, rand() % 10, 0);
				}
				break;
			case 2:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 2, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 2, rand() % 10, rand() % 10, 0);
				}
				break;
			case 3:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 3, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 3, rand() % 10, rand() % 10, 0);
				}
				break;
			case 4:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 4, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 4, rand() % 10, rand() % 10, 0);
				}
				break;
			case 5:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 5, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 5, rand() % 10, rand() % 10, 0);
				}
				break;
			case 6:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 6, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 6, rand() % 10, rand() % 10, 0);
				}
				break;
			case 7:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 7, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 7, rand() % 10, rand() % 10, 0);
				}
				break;
			case 8:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 8, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 8, rand() % 10, rand() % 10, 0);
				}
				break;
			case 9:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 9, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 9, rand() % 10, rand() % 10, 0);
				}
				break;
			case 10:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 10, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 10, rand() % 10, rand() % 10, 0);
				}
				break;
			case 11:
				if (PED::IS_PED_COMPONENT_VARIATION_VALID(playerPed, 11, rand() % 10, rand() % 10))
				{
					PED::SET_PED_COMPONENT_VARIATION(playerPed, 11, rand() % 10, rand() % 10, 0);
				}
				break;
				// switchable features
			default:
				if (lines[activeLineIndexClothing].pState)
					*lines[activeLineIndexClothing].pState = !(*lines[activeLineIndexClothing].pState);
				if (lines[activeLineIndexClothing].pUpdated)
					*lines[activeLineIndexClothing].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexClothing == 0)
						activeLineIndexClothing = lineCount;
					activeLineIndexClothing--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexClothing++;
						if (activeLineIndexClothing == lineCount)
							activeLineIndexClothing = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexPlayer = 0;
void process_player_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 30;

	std::string caption = "~r~Player ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "ESP", &featurePlayerESP, NULL },
		{ "ESP Boxes", &featurePlayerESPBoxes, NULL },
		{ "ESP Trace", &featurePlayerESPTrace, NULL },
		{ "Skin Changer 1", NULL, NULL },
		{ "Skin Changer 2", NULL, NULL },
		{ "Fix Player", NULL, NULL },
		{ "Reset Skin", NULL, NULL },
		{ "Attire Options", NULL, NULL },
		{ "Add Cash", &cashAdd, NULL },
		{ "Wanted Up", NULL, NULL },
		{ "Wanted Down", NULL, NULL },
		{ "Never Wanted", &featurePlayerNeverWanted, NULL },
		{ "Invincible", &featurePlayerInvincible, &featurePlayerInvincibleUpdated },
		{ "Police Ignored", &featurePlayerIgnored, &featurePlayerIgnoredUpdated },
		{ "Unlim Ability", &featurePlayerUnlimitedAbility, NULL },
		{ "Noiseless", &featurePlayerNoNoise, &featurePlayerNoNoiseUpdated },
		{ "Fast Swim", &featurePlayerFastSwim, &featurePlayerFastSwimUpdated },
		{ "Fast Run", &featurePlayerFastRun, &featurePlayerFastRunUpdated },
		{ "Ultra Run", &featurePlayerSuperRun, NULL },
		{ "Ultra Jump", &featurePlayerUltraJump, NULL},
		{ "Super Jump", &featurePlayerSuperJump, NULL },
		{ "Invisible", &playerInvisible, &playerInvisibleUpdated },
		{ "Night Vision", &featureNightVision, &featureNightVisionUpdated },
		{ "Thermal Vision", &featureThermalVision, &featureThermalVisionUpdated },
		{ "Blood Removal",&featurePlayerBloodDamage, NULL },
		{ "Auto Heal",&featurePlayerAutoHeal, NULL },
		{ "Chauffeur",	NULL,	NULL },
		{ "Parachute",	NULL,	NULL },
		{ "Hancock",&featurePlayerHancock, NULL },
		{ "Air Break",&featureAirBrake, NULL },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexPlayer)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, -0.5f, 60.0f + i * 20.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexPlayer].text, lines[activeLineIndexPlayer].pState),
				lineWidth + 0.0f, -0.5f, 60.0f + activeLineIndexPlayer * 20.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			// common variables
			BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
			Player player = PLAYER::PLAYER_ID();
			Ped playerPed = PLAYER::PLAYER_PED_ID();

			menu_beep_select();
			switch (activeLineIndexPlayer)
			{
				// skin changer
			case 3:
				if (process_skinchanger_menu())	return;
				break;
				// skin changer
			case 4:
				if (process_skinchanger_menu2()) return;
				break;
				// fix player
			case 5:
			{
				ENTITY::SET_ENTITY_HEALTH(playerPed, ENTITY::GET_ENTITY_MAX_HEALTH(playerPed));
				PED::ADD_ARMOUR_TO_PED(playerPed, PLAYER::GET_PLAYER_MAX_ARMOUR(player) - PED::GET_PED_ARMOUR(playerPed));
				if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
				{
					Vehicle playerVeh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
					if (ENTITY::DOES_ENTITY_EXIST(playerVeh) && !ENTITY::IS_ENTITY_DEAD(playerVeh))
						VEHICLE::SET_VEHICLE_FIXED(playerVeh);
				}
				std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
				std::string text = " ~g~your player was fixed.";
				set_status_text(playerName + text);
			}
			break;
			// reset model skin
			case 6:
			{
				PED::SET_PED_DEFAULT_COMPONENT_VARIATION(playerPed);
				std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
				std::string text = " ~g~you are using default model skin.";
				set_status_text(playerName + text);
			}
			break;
			case 7:
				process_clothing_menu();
				break;
			case 8:
				set_status_text("~o~Hotkey~w~: numpad minus - ");
				cashAdd = !cashAdd;
				break;
				// wanted up
			case 9:
				if (bPlayerExists && PLAYER::GET_PLAYER_WANTED_LEVEL(player) < 5)
				{
					PLAYER::SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 1, 0);
					PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, 0);
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~your wanted level is up.";
					set_status_text(playerName + text);
				}
				break;
				// wanted down
			case 10:
				if (bPlayerExists && PLAYER::GET_PLAYER_WANTED_LEVEL(player) > 0)
				{
					PLAYER::SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) - 1, 0);
					PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, 0);
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~g~your wanted level is down.";
					set_status_text(playerName + text);
				}
				break;
			case 26:
				if (bPlayerExists)
				{
					int blipInfo = UI::GET_FIRST_BLIP_INFO_ID(8);
					if (UI::DOES_BLIP_EXIST(blipInfo))
					{
						STREAMING::REQUEST_MODEL(GAMEPLAY::GET_HASH_KEY("f620"));
						while (!STREAMING::HAS_MODEL_LOADED(GAMEPLAY::GET_HASH_KEY("f620"))) WAIT(0);
						Vehicle createVehicle = VEHICLE::CREATE_VEHICLE(GAMEPLAY::GET_HASH_KEY("f620"), ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 5.0, 0).x, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 5.0, 0).y, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 5.0, 0).z, ENTITY::GET_ENTITY_HEADING(playerPed), TRUE, TRUE);
						if (createVehicle != 0)
						{
							Ped driver = PED::CREATE_RANDOM_PED_AS_DRIVER(createVehicle, FALSE);
							PED::SET_PED_INTO_VEHICLE(driver, createVehicle, -1);
							PED::SET_PED_INTO_VEHICLE(playerPed, createVehicle, 0);
							AI::TASK_VEHICLE_DRIVE_TO_COORD(driver, createVehicle, UI::GET_BLIP_COORDS(blipInfo).x, UI::GET_BLIP_COORDS(blipInfo).y, UI::GET_BLIP_COORDS(blipInfo).z, 40.0, 1, ENTITY::GET_ENTITY_MODEL(createVehicle), 5, 6.0f, -1.0f);
						}
					}
					if (!UI::DOES_BLIP_EXIST(blipInfo))
					{
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~r~you must set a wavepoint.";
						set_status_text(playerName + text);
					}
				}
				break;
			case 27:
				if (bPlayerExists)
				{
					Vector3 coords = ENTITY::GET_ENTITY_COORDS(playerPed, 0);
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, coords.x, coords.y, coords.z + 2000.0f, 0, 0, 1);
					AI::TASK_PARACHUTE_TO_TARGET(playerPed, coords.x, coords.y, coords.z);
				}
				break;
			case 28:
				set_status_text("~o~Hotkey~s~: [{ and }]");
				featurePlayerHancock = !featurePlayerHancock;
				break;
			case 29:
				set_status_text("~o~Hotkey~s~: Q/Z - Up/Down~n~A/D - Rotate~n~W/S - Forward/Back~n~Shift - Speed");
				featureAirBrake = !featureAirBrake;
				break;
				// switchable features
			default:
				if (lines[activeLineIndexPlayer].pState)
					*lines[activeLineIndexPlayer].pState = !(*lines[activeLineIndexPlayer].pState);
				if (lines[activeLineIndexPlayer].pUpdated)
					*lines[activeLineIndexPlayer].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexPlayer == 0)
						activeLineIndexPlayer = lineCount;
					activeLineIndexPlayer--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexPlayer++;
						if (activeLineIndexPlayer == lineCount)
							activeLineIndexPlayer = 0;
						waitTime = 200;
					}
	}
}
#pragma region teleport extras

struct tele_location {
	std::string text;
	float x;
	float y;
	float z;
	void(*scenery_loader)(void);
	void(*scenery_unloader)(void);
	bool isLoaded;
};

std::vector<tele_location> LOCATIONS = {
	{ "Waypoint Market" },
	{ "Michael's House", -852.4f, 160.0f, 65.6f },
	{ "Franklin's House", 7.9f, 548.1f, 175.5f },
	{ "Trevor's Trailer", 1985.7f, 3812.2f, 32.2f },
	{ "Airport Enterance", -1034.6f, -2733.6f, 13.8f },
	{ "Airport Field", -1336.0f, -3044.0f, 13.9f },
	{ "Ekysuan Island", 338.2f, -2715.9f, 38.5f },
	{ "Jetsam", 760.4f, -2943.2f, 5.8f },
	{ "Stripclub", 127.4f, -1307.7f, 29.2f },
	{ "Elburro Heights", 1384.0f, -2057.1f, 52.0f },
	{ "Ferris Wheel", -1670.7f, -1125.0f, 13.0f },
	{ "Chumash", -3192.6f, 1100.0f, 20.2f },
	{ "Windfarm", 2354.0f, 1830.3f, 101.1f },
	{ "Military Base", -2047.4f, 3132.1f, 32.8f },
	{ "McKenzie Airfield", 2121.7f, 4796.3f, 41.1f },
	{ "Desert Airfield", 1747.0f, 3273.7f, 41.1f },
	{ "Chilliad", 425.4f, 5614.3f, 766.5f },
	{ "North Yankton", 3360.19f, -4849.67f, 111.8f, load_north_yankton, unload_north_yankton, false },

	// Extra locations coordinates source: "PulseR_HD" @ http://gtaforums.com/topic/789786-vrelwip-simple-trainer-enhancements-skin-detail-chooser-menu-architecture/?p=1067398379
	{ "10 Car Garage Back Room", 223.193f, -967.322f, 99.000f },
	{ "Airplane Graveyard Airplane Tail ", 2395.096f, 3049.616f, 60.053f },
	{ "Altruist Cult Camp", -1170.841f, 4926.646f, 224.295f },
	{ "Ammunation Gun Range", 22.153f, -1072.854f, 29.797f },
	{ "Ammunation Office", 12.494f, -1110.130f, 29.797f },
	{ "Blaine County Savings Bank", -109.299f, 6464.035f, 31.627f },
	{ "Burnt FIB Building", 160.868f, -745.831f, 250.063f },
	{ "Calafia Train Bridge", -517.869f, 4425.284f, 89.795f },
	{ "Cargo Ship", 899.678f, -2882.191f, 19.013f },
	{ "Carrier (MP only)", 3069.330f, -4704.220f, 15.043f },
	{ "Chumash Historic Family Pier", -3426.683f, 967.738f, 8.347f },
	{ "Dead Sea Monster", -3373.726f, 504.714f, -24.656f },
	{ "Del Perro Pier", -1850.127f, -1231.751f, 13.017f },
	{ "Devin Weston's House", -2639.872f, 1866.812f, 160.135f },
	{ "Director Mod Trailer (might not work)", -20.004f, -10.889f, 500.602f },
	{ "FIB Roof", 150.126f, -754.591f, 262.865f },
	{ "FIB Top Floor", 135.733f, -749.216f, 258.152f },
	{ "Floyd's Apartment", -1150.703f, -1520.713f, 10.633f },
	{ "Fort Zancudo ATC entrance", -2344.373f, 3267.498f, 32.811f },
	{ "Fort Zancudo ATC top floor", -2358.132f, 3249.754f, 101.451f },
	{ "Fort Zankudo UFO", 2052.000f, 3237.000f, 1456.973f },
	{ "Galileo Observatory Roof", -438.804f, 1076.097f, 352.411f },
	{ "God's thumb", -1006.402f, 6272.383f, 1.503f },
	{ "Hippy Camp", 2476.712f, 3789.645f, 41.226f },
	{ "Humane Labs Entrance", 3619.749f, 2742.740f, 28.690f },
	{ "Humane Labs Tunnel", 3525.495f, 3705.301f, 20.992f },
	{ "IAA Office", 117.220f, -620.938f, 206.047f },
	{ "IAA Roof", 134.085f, -637.859f, 262.851f },
	{ "Jolene Cranley-Evans Ghost", 3059.620f, 5564.246f, 197.091f },
	{ "Kortz Center", -2243.810f, 264.048f, 174.615f },
	{ "Lester's House", 1273.898f, -1719.304f, 54.771f },
	{ "Main LS Customs", -365.425f, -131.809f, 37.873f },
	{ "Marlowe Vineyards", -1868.971f, 2095.674f, 139.115f },
	{ "Maze Bank Arena Roof", -324.300f, -1968.545f, 67.002f },
	{ "Maze Bank Roof", -75.015f, -818.215f, 326.176f },
	{ "Merryweather Dock", 486.417f, -3339.692f, 6.070f },
	{ "Mineshaft", -595.342f, 2086.008f, 131.412f },
	{ "Most Northerly Point", 24.775f, 7644.102f, 19.055f },
	{ "NOOSE Headquarters", 2535.243f, -383.799f, 92.993f },
	{ "North Yankton Bank", 5309.519f, -5212.375f, 83.522f },
	{ "Pacific Standard Bank Vault", 255.851f, 217.030f, 101.683f },
	{ "Pacific Standard Bank", 235.046f, 216.434f, 106.287f },
	{ "Paleto Bay Pier", -275.522f, 6635.835f, 7.425f },
	{ "Palmer-Taylor Power Station Chimney", 2732.931f, 1577.540f, 83.671f },
	{ "Playboy Mansion", -1475.234f, 167.088f, 55.841f },
	{ "Police Station", 436.491f, -982.172f, 30.699f },
	{ "Quarry", 2954.196f, 2783.410f, 41.004f },
	{ "Rebel Radio", 736.153f, 2583.143f, 79.634f },
	{ "Sandy Shores Building Site Crane", 1051.209f, 2280.452f, 89.727f },
	{ "Satellite Dish Antenna", 2034.988f, 2953.105f, 74.602f },
	{ "Satellite Dishes", 2062.123f, 2942.055f, 47.431f },
	{ "Sisyphus Theater Stage", 205.316f, 1167.378f, 227.005f },
	{ "Stab City", 126.975f, 3714.419f, 46.827f },
	{ "Strip Club DJ Booth", 126.135f, -1278.583f, 29.270f },
	{ "Sunken Body", -3161.078f, 3001.998f, -37.974f },
	{ "Sunken Cargo Ship", 3199.748f, -379.018f, -22.500f },
	{ "Sunken Plane", -942.350f, 6608.752f, -20.912f },
	{ "Top of the Mt Chilad", 450.718f, 5566.614f, 806.183f },
	{ "Trevor's Meth Lab", 1391.773f, 3608.716f, 38.942f },
	{ "Underwater Hatch", 4273.950f, 2975.714f, -170.746f },
	{ "Underwater UFO", 762.426f, 7380.371f, -111.377f },
	{ "Underwater WW2 Tank", 4201.633f, 3643.821f, -39.016f },
	{ "Very High Up", -129.964f, 8130.873f, 6705.307f },
	{ "Vinewood Bowl Stage", 686.245f, 577.950f, 130.461f },
	{ "Weed Farm", 2208.777f, 5578.235f, 53.735f },
	{ "Windmill Top", 2026.677f, 1842.684f, 133.313f },
	{ "Yacht", -2023.661f, -1038.038f, 5.577f } };

int teleportMenuIndex = 0;

void teleport_to_coords(Entity e, Vector3 coords)
{
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, coords.z, 0, 0, 1);
	WAIT(0);
	set_status_text("Teleported");
}

void teleport_to_marker(Entity e)
{
	Vector3 coords;
	bool success = false;

	bool blipFound = false;
	// search for marker blip
	int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
	for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
	{
		if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4)
		{
			coords = UI::GET_BLIP_INFO_ID_COORD(i);
			blipFound = true;
			break;
		}
	}
	if (blipFound)
	{
		// load needed map region and check height levels for ground existence
		bool groundFound = false;
		static float groundCheckHeight[] = {
			100.0, 150.0, 50.0, 0.0, 200.0, 250.0, 300.0, 350.0, 400.0,
			450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
		};
		for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++)
		{
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, groundCheckHeight[i], 0, 0, 1);
			WAIT(100);
			if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, groundCheckHeight[i], &coords.z, 1))
			{
				groundFound = true;
				coords.z += 3.0;
				break;
			}
		}
		// if ground not found then set Z in air and give player a parachute
		if (!groundFound)
		{
			coords.z = 1000.0;
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, 1, 0);
		}

		//do it
		teleport_to_coords(e, coords);
	}
	else
	{
		set_status_text("Map marker isn't set");
	}
}

bool onconfirm_teleport_location(int selection, std::string caption, int locationIndex)
{
	teleportMenuIndex = selection;

	tele_location* value = &LOCATIONS[locationIndex];

	// get entity to teleport
	Entity e = PLAYER::PLAYER_PED_ID();
	if (PED::IS_PED_IN_ANY_VEHICLE(e, 0))
	{
		e = PED::GET_VEHICLE_PED_IS_USING(e);
	}

	if (selection == 0)
	{
		teleport_to_marker(e);
	}
	else
	{
		Vector3 coords;

		if (value->scenery_loader != NULL && !value->isLoaded)
		{
			set_status_text("Loading new scenery...");
			value->scenery_loader();
			value->isLoaded = true;

			DWORD time = GetTickCount() + 1000;
			while (GetTickCount() < time)
			{
				make_periodic_feature_call();
				WAIT(0);
			}

			set_status_text("New scenery loaded");

			time = GetTickCount() + 1000;
			while (GetTickCount() < time)
			{
				make_periodic_feature_call();
				WAIT(0);
			}
		}

		coords.x = value->x;
		coords.y = value->y;
		coords.z = value->z;
		teleport_to_coords(e, coords);

		bool unloadedAnything = false;
		DWORD time = GetTickCount() + 1000;

		for (int i = 0; i < LOCATIONS.size(); i++)
		{
			//don't unload our newly loaded scenery
			if (i == locationIndex)
			{
				continue;
			}

			tele_location* loc = &LOCATIONS[i];

			if (loc->isLoaded && loc->scenery_unloader != NULL)
			{
				if (!unloadedAnything)
				{
					set_status_text("Unloading old scenery...");
					time = GetTickCount() + 1000;
					while (GetTickCount() < time)
					{
						make_periodic_feature_call();
						WAIT(0);
					}
				}

				loc->scenery_unloader();
				unloadedAnything = true;
				loc->isLoaded = false;
			}
		}

		if (unloadedAnything)
		{
			set_status_text("Old scenery unloaded");

			time = GetTickCount() + 1000;
			while (GetTickCount() < time)
			{
				make_periodic_feature_call();
				WAIT(0);
			}
		}
	}
	return true;
}

bool process_teleport_menu2()
{
	std::string caption = "Teleport";

	std::vector<std::string> menuCaptions;
	std::vector<int> menuIndexes;

	for (int i = 0; i < LOCATIONS.size(); i++)
	{
		menuCaptions.push_back(LOCATIONS[i].text);
		menuIndexes.push_back(i);
	}

	return draw_generic_menu<int>(menuCaptions, menuIndexes, teleportMenuIndex, "Teleport Locations", onconfirm_teleport_location, NULL, NULL);
}

void reset_teleporter_globals()
{
	teleportMenuIndex = 0;
}

void load_north_yankton()
{
	if (ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()))// && STREAMING::IS_IPL_ACTIVE("plg_01") == 0)
	{
		STREAMING::REQUEST_IPL("plg_01");
		STREAMING::REQUEST_IPL("prologue01");
		STREAMING::REQUEST_IPL("prologue01_lod");
		STREAMING::REQUEST_IPL("prologue01c");
		STREAMING::REQUEST_IPL("prologue01c_lod");
		STREAMING::REQUEST_IPL("prologue01d");
		STREAMING::REQUEST_IPL("prologue01d_lod");
		STREAMING::REQUEST_IPL("prologue01e");
		STREAMING::REQUEST_IPL("prologue01e_lod");
		STREAMING::REQUEST_IPL("prologue01f");
		STREAMING::REQUEST_IPL("prologue01f_lod");
		STREAMING::REQUEST_IPL("prologue01g");
		STREAMING::REQUEST_IPL("prologue01h");
		STREAMING::REQUEST_IPL("prologue01h_lod");
		STREAMING::REQUEST_IPL("prologue01i");
		STREAMING::REQUEST_IPL("prologue01i_lod");
		STREAMING::REQUEST_IPL("prologue01j");
		STREAMING::REQUEST_IPL("prologue01j_lod");
		STREAMING::REQUEST_IPL("prologue01k");
		STREAMING::REQUEST_IPL("prologue01k_lod");
		STREAMING::REQUEST_IPL("prologue01z");
		STREAMING::REQUEST_IPL("prologue01z_lod");
		STREAMING::REQUEST_IPL("plg_02");
		STREAMING::REQUEST_IPL("prologue02");
		STREAMING::REQUEST_IPL("prologue02_lod");
		STREAMING::REQUEST_IPL("plg_03");
		STREAMING::REQUEST_IPL("prologue03");
		STREAMING::REQUEST_IPL("prologue03_lod");
		STREAMING::REQUEST_IPL("prologue03b");
		STREAMING::REQUEST_IPL("prologue03b_lod");
		//the commented code disables the 'Prologue' grave and
		//enables the 'Bury the Hatchet' grave
		//STREAMING::REQUEST_IPL("prologue03_grv_cov");
		//STREAMING::REQUEST_IPL("prologue03_grv_cov_lod");
		STREAMING::REQUEST_IPL("prologue03_grv_dug");
		STREAMING::REQUEST_IPL("prologue03_grv_dug_lod");
		//STREAMING::REQUEST_IPL("prologue03_grv_fun");
		STREAMING::REQUEST_IPL("prologue_grv_torch");
		STREAMING::REQUEST_IPL("plg_04");
		STREAMING::REQUEST_IPL("prologue04");
		STREAMING::REQUEST_IPL("prologue04_lod");
		STREAMING::REQUEST_IPL("prologue04b");
		STREAMING::REQUEST_IPL("prologue04b_lod");
		STREAMING::REQUEST_IPL("prologue04_cover");
		STREAMING::REQUEST_IPL("des_protree_end");
		STREAMING::REQUEST_IPL("des_protree_start");
		STREAMING::REQUEST_IPL("des_protree_start_lod");
		STREAMING::REQUEST_IPL("plg_05");
		STREAMING::REQUEST_IPL("prologue05");
		STREAMING::REQUEST_IPL("prologue05_lod");
		STREAMING::REQUEST_IPL("prologue05b");
		STREAMING::REQUEST_IPL("prologue05b_lod");
		STREAMING::REQUEST_IPL("plg_06");
		STREAMING::REQUEST_IPL("prologue06");
		STREAMING::REQUEST_IPL("prologue06_lod");
		STREAMING::REQUEST_IPL("prologue06b");
		STREAMING::REQUEST_IPL("prologue06b_lod");
		STREAMING::REQUEST_IPL("prologue06_int");
		STREAMING::REQUEST_IPL("prologue06_int_lod");
		STREAMING::REQUEST_IPL("prologue06_pannel");
		STREAMING::REQUEST_IPL("prologue06_pannel_lod");
		//STREAMING::REQUEST_IPL("prologue_m2_door");
		//STREAMING::REQUEST_IPL("prologue_m2_door_lod");
		STREAMING::REQUEST_IPL("plg_occl_00");
		STREAMING::REQUEST_IPL("prologue_occl");
		STREAMING::REQUEST_IPL("plg_rd");
		STREAMING::REQUEST_IPL("prologuerd");
		STREAMING::REQUEST_IPL("prologuerdb");
		STREAMING::REQUEST_IPL("prologuerd_lod");
	}
}

void unload_north_yankton()
{
	if (ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()))// && STREAMING::IS_IPL_ACTIVE("plg_01") == 1)
	{
		STREAMING::REMOVE_IPL("plg_01");
		STREAMING::REMOVE_IPL("prologue01");
		STREAMING::REMOVE_IPL("prologue01_lod");
		STREAMING::REMOVE_IPL("prologue01c");
		STREAMING::REMOVE_IPL("prologue01c_lod");
		STREAMING::REMOVE_IPL("prologue01d");
		STREAMING::REMOVE_IPL("prologue01d_lod");
		STREAMING::REMOVE_IPL("prologue01e");
		STREAMING::REMOVE_IPL("prologue01e_lod");
		STREAMING::REMOVE_IPL("prologue01f");
		STREAMING::REMOVE_IPL("prologue01f_lod");
		STREAMING::REMOVE_IPL("prologue01g");
		STREAMING::REMOVE_IPL("prologue01h");
		STREAMING::REMOVE_IPL("prologue01h_lod");
		STREAMING::REMOVE_IPL("prologue01i");
		STREAMING::REMOVE_IPL("prologue01i_lod");
		STREAMING::REMOVE_IPL("prologue01j");
		STREAMING::REMOVE_IPL("prologue01j_lod");
		STREAMING::REMOVE_IPL("prologue01k");
		STREAMING::REMOVE_IPL("prologue01k_lod");
		STREAMING::REMOVE_IPL("prologue01z");
		STREAMING::REMOVE_IPL("prologue01z_lod");
		STREAMING::REMOVE_IPL("plg_02");
		STREAMING::REMOVE_IPL("prologue02");
		STREAMING::REMOVE_IPL("prologue02_lod");
		STREAMING::REMOVE_IPL("plg_03");
		STREAMING::REMOVE_IPL("prologue03");
		STREAMING::REMOVE_IPL("prologue03_lod");
		STREAMING::REMOVE_IPL("prologue03b");
		STREAMING::REMOVE_IPL("prologue03b_lod");
		//the commented code disables the 'Prologue' grave and
		//enables the 'Bury the Hatchet' grave
		//STREAMING::REMOVE_IPL("prologue03_grv_cov");
		//STREAMING::REMOVE_IPL("prologue03_grv_cov_lod");
		STREAMING::REMOVE_IPL("prologue03_grv_dug");
		STREAMING::REMOVE_IPL("prologue03_grv_dug_lod");
		//STREAMING::REMOVE_IPL("prologue03_grv_fun");
		STREAMING::REMOVE_IPL("prologue_grv_torch");
		STREAMING::REMOVE_IPL("plg_04");
		STREAMING::REMOVE_IPL("prologue04");
		STREAMING::REMOVE_IPL("prologue04_lod");
		STREAMING::REMOVE_IPL("prologue04b");
		STREAMING::REMOVE_IPL("prologue04b_lod");
		STREAMING::REMOVE_IPL("prologue04_cover");
		STREAMING::REMOVE_IPL("des_protree_end");
		STREAMING::REMOVE_IPL("des_protree_start");
		STREAMING::REMOVE_IPL("des_protree_start_lod");
		STREAMING::REMOVE_IPL("plg_05");
		STREAMING::REMOVE_IPL("prologue05");
		STREAMING::REMOVE_IPL("prologue05_lod");
		STREAMING::REMOVE_IPL("prologue05b");
		STREAMING::REMOVE_IPL("prologue05b_lod");
		STREAMING::REMOVE_IPL("plg_06");
		STREAMING::REMOVE_IPL("prologue06");
		STREAMING::REMOVE_IPL("prologue06_lod");
		STREAMING::REMOVE_IPL("prologue06b");
		STREAMING::REMOVE_IPL("prologue06b_lod");
		STREAMING::REMOVE_IPL("prologue06_int");
		STREAMING::REMOVE_IPL("prologue06_int_lod");
		STREAMING::REMOVE_IPL("prologue06_pannel");
		STREAMING::REMOVE_IPL("prologue06_pannel_lod");
		//STREAMING::REMOVE_IPL("prologue_m2_door");
		//STREAMING::REMOVE_IPL("prologue_m2_door_lod");
		STREAMING::REMOVE_IPL("plg_occl_00");
		STREAMING::REMOVE_IPL("prologue_occl");
		STREAMING::REMOVE_IPL("plg_rd");
		STREAMING::REMOVE_IPL("prologuerd");
		STREAMING::REMOVE_IPL("prologuerdb");
		STREAMING::REMOVE_IPL("prologuerd_lod");
	}
}

int teleportActiveLineIndex2 = 0;
bool process_teleport_menu1()
{
	player_coords();
	const float lineWidth = 200.0;
	const int lineCount = 24;

	std::string caption = "~r~Teleport ~w~Options";

	static struct {
		LPCSTR  text;
		float x;
		float y;
		float z;
	} lines[lineCount] = {
		{ "Marker" },
		{ "Ls Customs", -375.89f, -119.05f, 38.69f },
		{ "Mors Insurance", -221.86f, -1155.84f, 23.02f },
		{ "Ammu-nationnum", 237.79f, -42.70f, 69.74f },
		{ "Barber Shop", -28.32f, -137.87f, 56.95f },
		{ "Tattoo Parlor", 319.96f, 174.10f, 103.75f },
		{ "Clothing Store", 87.82f, -1391.84f, 29.18f },
		{ "Convenience Store", 37.67f, -1354.44f, 29.29f },
		{ "Michael's House", -852.4f, 160.0f, 65.6f },
		{ "Franklin's House", 7.9f, 548.1f, 175.5f },
		{ "Trevor's Trailer", 1985.7f, 3812.2f, 32.2f },
		{ "Airport Entrance", -1034.6f, -2733.6f, 13.8f },
		{ "Airport Field", -1336.0f, -3044.0f, 13.9f },
		{ "Elysian Island", 338.2f, -2715.9f, 38.5f },
		{ "Jetsam", 760.4f, -2943.2f, 5.8f },
		{ "Stripclub", 127.4f, -1307.7f, 29.2f },
		{ "Elburro Heights", 1384.0f, -2057.1f, 52.0f },
		{ "Ferris Wheel", -1670.7f, -1125.0f, 13.0f },
		{ "Chumash", -3192.6f, 1100.0f, 20.2f },
		{ "Windfarm", 2354.0f, 1830.3f, 101.1f },
		{ "Military Base", -2047.4f, 3132.1f, 32.8f },
		{ "Mckenzie Airfield", 2121.7f, 4796.3f, 41.1f },
		{ "Desert Airfield", 1747.0f, 3273.7f, 41.1f },
		{ "Chilliad", 425.4f, 5614.3f, 766.5f }
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != teleportActiveLineIndex2)
					draw_menu_line(lines[i].text,
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(lines[teleportActiveLineIndex2].text,
				lineWidth + 0.0f, 2.0f, 60.0f + teleportActiveLineIndex2 * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();

			// get entity to teleport
			Entity e = PLAYER::PLAYER_PED_ID();
			if (PED::IS_PED_IN_ANY_VEHICLE(e, 0))
				e = PED::GET_VEHICLE_PED_IS_USING(e);

			// get coords
			Vector3 coords;
			bool success = FALSE;
			if (teleportActiveLineIndex2 == 0) // marker
			{
				bool blipFound = FALSE;
				// search for marker blip
				int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
				for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
				{
					if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4)
					{
						coords = UI::GET_BLIP_INFO_ID_COORD(i);
						blipFound = TRUE;
						break;
					}
				}
				if (blipFound)
				{
					// load needed map region and check height levels for ground existence
					bool groundFound = FALSE;
					static float groundCheckHeight[] = {
						100.0, 300.0, 50.0, 0.0, 300.0, 250.0, 300.0, 350.0, 400.0,
						450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
					};
					for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++)
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, groundCheckHeight[i], 0, 0, 1);
						WAIT(100);
						if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, groundCheckHeight[i], &coords.z, 1))
						{
							groundFound = TRUE;
							coords.z += 3.0;
							break;
						}
					}
					// if ground not found then set Z in air and give player a parachute
					if (!groundFound)
					{
						coords.z = 1000.0;
						WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, 1, 0);
					}
					success = TRUE;
				}
				else
				{
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~your map marker isn't set.";
					set_status_text(playerName + text);
				}

			}
			else // predefined coords
			{
				coords.x = lines[teleportActiveLineIndex2].x;
				coords.y = lines[teleportActiveLineIndex2].y;
				coords.z = lines[teleportActiveLineIndex2].z;
				success = TRUE;
			}

			// set player pos
			if (success)
			{
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, coords.x, coords.y, coords.z, 0, 0, 1);
				WAIT(0);
				std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
				std::string text = " ~g~your teleport was complete.";
				set_status_text(playerName + text);
				return TRUE;
			}

			for (int i = 0; i <= 1000; i++)
			{
				//			Blip_t* list;
			//				Blip_t* blip = list;//pBlipList->m_Blips[i].m_pBlip;
						//	if (blip)
						//	{
						//		if ((blip->dwColor == BLIPCOLOR_MISSION && blip->iIcon == BLIP_CIRCLE) ||
						//			(blip->dwColor == BLIPCOLOR_YELLOWMISSION && blip->iIcon == BLIP_CIRCLE) ||
						//			///..//	(blip->dwColor == BLIPCOLOR_YELLOWMISSION2 && (blip->iIcon == BLIP_CIRCLE || blip->iIcon == BLIP_DOLLARSIGN)) ||
						//			(blip->dwColor == BLIPCOLOR_NONE && blip->iIcon == BLIP_RACEFLAG) ||
						//			(blip->dwColor == BLIPCOLOR_GREEN && blip->iIcon == BLIP_CIRCLE) ||
						//			(blip->iIcon == BLIP_SPECIALCRATE))
						//		{
						//			Entity e = PLAYER::PLAYER_PED_ID();
						//			Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
						//			if (veh)
						//				e = veh;
						//			requestControl(veh);
						//			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, blip->x, blip->y, blip->z, FALSE, FALSE, FALSE);
						//			break;
						//		}
						//	}
						//}
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (teleportActiveLineIndex2 == 0)
						teleportActiveLineIndex2 = lineCount;
					teleportActiveLineIndex2--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						teleportActiveLineIndex2++;
						if (teleportActiveLineIndex2 == lineCount)
							teleportActiveLineIndex2 = 0;
						waitTime = 200;
					}
	}
	return FALSE;
}
#pragma endregion
int teleportActiveLineIndex = 0;
void process_teleport_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 2;

	std::string caption = "~r~Teleport ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
	{ "Teleport Options 1", NULL, NULL},
	{ "Teleport Options 2", NULL, NULL}
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != teleportActiveLineIndex)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[teleportActiveLineIndex].text, lines[teleportActiveLineIndex].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + teleportActiveLineIndex * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			switch (teleportActiveLineIndex)
			{
			case 0:
				process_teleport_menu1();
				break;
			case 1:
				process_teleport_menu2();
				break;
				// switchable features
			default:
				if (lines[teleportActiveLineIndex].pState)
					*lines[teleportActiveLineIndex].pState = !(*lines[teleportActiveLineIndex].pState);
				if (lines[teleportActiveLineIndex].pUpdated)
					*lines[teleportActiveLineIndex].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (teleportActiveLineIndex == 0)
						teleportActiveLineIndex = lineCount;
					teleportActiveLineIndex--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						teleportActiveLineIndex++;
						if (teleportActiveLineIndex == lineCount)
							teleportActiveLineIndex = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexSuperRounds = 0;
void process_superrounds_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 18;

	std::string caption = "~r~Super ~w~Rounds";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "Firework", &featureWeaponFirework,	  NULL },
		{ "Vehicl Rocket", &featureWeaponVehicleRocket,	  NULL },
		{ "Tank", &featureWeaponTank,	  NULL },
		{ "Railgun", &featureWeaponRailGun, NULL },
		{ "Heavy Sniper", &featureWeaponHeavySniper, NULL },
		{ "Minigun", &featureWeaponMinigun, NULL },
		{ "Plane Rocket", &featureWeaponPlaneRocket, NULL },
		{ "Grenade Launcher", &featureWeaponGrenadeLauncher, NULL },
		{ "Airstrike Rocket", &featureWeaponAirstrikeRocket, NULL },
		{ "Savage", &featureWeaponSavage, NULL },
		{ "Space Rocket", &featureWeaponSpaceRocket, NULL },
		{ "Money Bag", &featureWeaponPropMoneyBag, NULL },
		{ "Coke Block", &featureWeaponPropCokeBlock, NULL },
		{ "Weed Plant", &featureWeaponPropWeed, NULL },
		{ "Weed Pallet", &featureWeaponPropWeedPallet, NULL },
		{ "Xmas Tree", &featureWeaponPropXmasTree, NULL },
		{ "Xmas Tree", &featureWeaponPropXmas, NULL },
		{ "Dildo", &featureWeaponPropDildo, NULL },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexSuperRounds)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexSuperRounds].text, lines[activeLineIndexSuperRounds].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexSuperRounds * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();

			// common variables
			BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
			Player player = PLAYER::PLAYER_ID();
			Ped playerPed = PLAYER::PLAYER_PED_ID();

			switch (activeLineIndexSuperRounds)
			{
				// switchable features
			default:
				if (lines[activeLineIndexSuperRounds].pState)
					*lines[activeLineIndexSuperRounds].pState = !(*lines[activeLineIndexSuperRounds].pState);
				if (lines[activeLineIndexSuperRounds].pUpdated)
					*lines[activeLineIndexSuperRounds].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexSuperRounds == 0)
						activeLineIndexSuperRounds = lineCount;
					activeLineIndexSuperRounds--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexSuperRounds++;
						if (activeLineIndexSuperRounds == lineCount)
							activeLineIndexSuperRounds = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexWeapon = 0;
void process_weapon_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 15;

	std::string caption = "~r~Weapon ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "Get All Weapon",	NULL,						  NULL },
		{ "Get Weapon Hash",	NULL,						  NULL },
		{ "Triggerbot",		&featureWeaponTriggerbot,		  NULL },
		{ "No Reload",		&featureWeaponNoReload,		  &featureWeaponNoReloadUpdated },
		{ "Fire Ammo",		&featureWeaponFireAmmo,		  NULL },
		{ "Explosive Ammo",  &featureWeaponExplosiveAmmo,  NULL },
		{ "Explosive Melee", &featureWeaponExplosiveMelee, NULL },
		{ "One Punch Kill",	&weaponOnePunchKill,	&weaponOnePunchKillUpdated },
		{ "One Shot Kill",	&featureWeaponDamageModifier,	&featureWeaponDamageModifierUpdate },
		{ "Force Gun",	&weaponForceGun, NULL },
		{ "Gravity Gun",	&weaponGravityGun, NULL },
		{ "Spaghettios",	&weaponSpaghettios, NULL },
		{ "Pink Guns",	NULL, NULL },
		{ "Super Rounds",	NULL, NULL },
		{ "Delete Objects",	&weaponDeleteObjects, NULL },

	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexWeapon)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexWeapon].text, lines[activeLineIndexWeapon].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexWeapon * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();

			// common variables
			BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
			Player player = PLAYER::PLAYER_ID();
			Ped playerPed = PLAYER::PLAYER_PED_ID();

			switch (activeLineIndexWeapon)
			{
			case 0:
				for (int a = 0; a < sizeof(weaponNames) / sizeof(weaponNames[0]); a++)
					for (int b = 0; b < sizeof(weaponComponents) / sizeof(weaponComponents[0]); b++)
					{
						WEAPON::GIVE_DELAYED_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char*)weaponNames[a]), 1500, FALSE);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char*)weaponNames[a]), GAMEPLAY::GET_HASH_KEY((char*)weaponComponents[b]));
						WEAPON::GIVE_DELAYED_WEAPON_TO_PED(playerPed, 0xFBAB5776, 1500, FALSE);
						WEAPON::SET_PED_AMMO(playerPed, GAMEPLAY::GET_HASH_KEY((char*)weaponNames[a]), 9999);
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~g~all weapons were added.";
						set_status_text(playerName + text);
					}
				break;
			case 1:
				if (bPlayerExists)
				{
					GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(TRUE, "FMMC_KEY_TIP8", "", "weapon_railgun", "", "", "", 25);
					while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(0);
					if (!WEAPON::IS_WEAPON_VALID(GAMEPLAY::GET_HASH_KEY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())))
					{
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~r~that was a invalid weapon hash.";
						set_status_text(playerName + text);
						return;
					}
					else
					{
						char* weapon = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
						char* playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						Hash pedWeapon = GAMEPLAY::GET_HASH_KEY(weapon);
						WEAPON::GIVE_DELAYED_WEAPON_TO_PED(playerPed, pedWeapon, 0, TRUE);
						WEAPON::SET_PED_AMMO(playerPed, pedWeapon, 1000);
						char text[250];
						sprintf_s(text, "%s ~g~you received a %s.", playerName, weapon);
						set_status_text(text);
					}
				}
				break;
			case 11:
				if (bPlayerExists)
				{
					weaponSpaghettios = !weaponSpaghettios;
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = "~o~Hotkey~s~: / divide";
					set_status_text(playerName + text);
				}
				break;
			case 12:
				for (int i = 0; i < sizeof(weaponNames) / sizeof(weaponNames[0]); i++)
				{
					if (WEAPON::HAS_PED_GOT_WEAPON(playerPed, GAMEPLAY::GET_HASH_KEY((char*)weaponNames[i]), 0))
					{
						WEAPON::SET_PED_WEAPON_TINT_INDEX(playerPed, GAMEPLAY::GET_HASH_KEY((char*)weaponNames[i]), 3);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~g~your weapon skins are pink.";
					set_status_text(playerName + text);
				}
				break;
			case 13:
				process_superrounds_menu();
				break;
				// switchable features
			default:
				if (lines[activeLineIndexWeapon].pState)
					*lines[activeLineIndexWeapon].pState = !(*lines[activeLineIndexWeapon].pState);
				if (lines[activeLineIndexWeapon].pUpdated)
					*lines[activeLineIndexWeapon].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexWeapon == 0)
						activeLineIndexWeapon = lineCount;
					activeLineIndexWeapon--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexWeapon++;
						if (activeLineIndexWeapon == lineCount)
							activeLineIndexWeapon = 0;
						waitTime = 200;
					}
	}
}
#pragma region lpcstr
LPCSTR vehicleModelsCase[37][10] = { { "ninef", "ninef2", "blista", "asea", "asea2", "boattrailer", "bus", "armytanker", "armytrailer", "armytrailer2" },{ "suntrap", "coach", "airbus", "asterope", "airtug", "ambulance", "barracks", "barracks2", "baller", "baller2" },{ "bjxl", "banshee", "benson", "bfinjection", "biff", "blazer", "blazer2", "blazer3", "bison", "bison2" },{ "bison3", "boxville", "boxville2", "boxville3", "bobcatxl", "bodhi2", "buccaneer", "buffalo", "buffalo2", "bulldozer" },{ "bullet", "blimp", "burrito", "burrito2", "burrito3", "burrito4", "burrito5", "cavalcade", "cavalcade2", "policet" },{ "gburrito", "cablecar", "caddy", "caddy2", "camper", "carbonizzare", "cheetah", "comet2", "cogcabrio", "coquette" },{ "cutter", "gresley", "dilettante", "dilettante2", "dune", "dune2", "hotknife", "dloader", "dubsta", "dubsta2" },{ "dump", "rubble", "docktug", "dominator", "emperor", "emperor2", "emperor3", "entityxf", "exemplar", "elegy2" },{ "f620", "fbi", "fbi2", "felon", "felon2", "feltzer2", "firetruk", "flatbed", "forklift", "fq2" },{ "fusilade", "fugitive", "futo", "granger", "gauntlet", "habanero", "hauler", "handler", "infernus", "ingot" },{ "intruder", "issi2", "jackal", "journey", "jb700", "khamelion", "landstalker", "lguard", "manana", "mesa" },{ "mesa2", "mesa3", "crusader", "minivan", "mixer", "mixer2", "monroe", "mower", "mule", "mule2" },{ "oracle", "oracle2", "packer", "patriot", "pbus", "penumbra", "peyote", "phantom", "phoenix", "picador" },{ "pounder", "police", "police4", "police2", "police3", "policeold1", "policeold2", "pony", "pony2", "prairie" },{ "pranger", "premier", "primo", "proptrailer", "rancherxl", "rancherxl2", "rapidgt", "rapidgt2", "radi", "ratloader" },{ "rebel", "regina", "rebel2", "rentalbus", "ruiner", "rumpo", "rumpo2", "rhino", "riot", "ripley" },{ "rocoto", "romero", "sabregt", "sadler", "sadler2", "sandking", "sandking2", "schafter2", "schwarzer", "scrap" },{ "seminole", "sentinel", "sentinel2", "zion", "zion2", "serrano", "sheriff", "sheriff2", "speedo", "speedo2" },{ "stanier", "stinger", "stingergt", "stockade", "stockade3", "stratum", "sultan", "superd", "surano", "surfer" },{ "surfer2", "surge", "taco", "tailgater", "taxi", "trash", "tractor", "tractor2", "tractor3", "graintrailer" },{ "baletrailer", "tiptruck", "tiptruck2", "tornado", "tornado2", "tornado3", "tornado4", "tourbus", "towtruck", "towtruck2" },{ "utillitruck", "utillitruck2", "utillitruck3", "voodoo2", "washington", "stretch", "youga", "ztype", "sanchez", "sanchez2" },{ "scorcher", "tribike", "tribike2", "tribike3", "fixter", "cruiser", "bmx", "policeb", "akuma", "carbonrs" },{ "bagger", "bati", "bati2", "ruffian", "daemon", "double", "pcj", "vader", "vigero", "faggio2" },{ "hexer", "annihilator", "buzzard", "buzzard2", "cargobob", "cargobob2", "cargobob3", "skylift", "polmav", "maverick" },{ "nemesis", "frogger", "frogger2", "cuban800", "duster", "stunt", "mammatus", "jet", "shamal", "luxor" },{ "titan", "lazer", "cargoplane", "squalo", "marquis", "dinghy", "dinghy2", "jetmax", "predator", "tropic" },{ "seashark", "seashark2", "submersible", "trailers", "trailers2", "trailers3", "tvtrailer", "raketrailer", "tanker", "trailerlogs" },{ "tr2", "tr3", "tr4", "trflat", "trailersmall", "velum", "adder", "voltic", "vacca", "bifta" },{ "speeder", "paradise", "kalahari", "jester", "turismor", "vestra", "alpha", "huntley", "thrust", "massacro" },{ "massacro2", "zentorno", "blade", "glendale", "panto", "pigalle", "warrener", "rhapsody", "dubsta3", "monster" },{ "sovereign", "innovation", "hakuchou", "furoregt", "miljet", "coquette2", "btype", "buffalo3", "dominator2", "gauntlet2" },{ "marshall", "dukes", "dukes2", "stalion", "stalion2", "blista2", "blista3", "dodo", "submersible2", "hydra" },{ "insurgent", "insurgent2", "technical", "savage", "valkyrie", "kuruma", "kuruma2", "jester2", "casco", "velum2" },{ "guardian", "enduro", "lectro", "slamvan", "slamvan2", "ratloader2", "swift2", "luxor2", "feltzer3", "osiris" },{ "virgo", "windsor", "besra", "swift", "blimp2", "vindicator", "toro", "t20", "coquette3", "chino" },{ "brawler", "", "", "", "", "", "", "", "", "" } };
#pragma endregion veh models
#pragma region vehicle extras
void BoostBaseVehicleStats(Vehicle vehicle)
{
	if (vehicle != NULL)
	{
		requestControl(vehicle);

		VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0.0f);
		//VEHICLE::_SET_VEHICLE_PAINT_FADE(vehicle, 0.0f);
		VEHICLE::SET_VEHICLE_IS_STOLEN(vehicle, FALSE);
		VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, FALSE);

		DECORATOR::DECOR_REGISTER("Player_Vehicle", 3);
		DECORATOR::DECOR_REMOVE(vehicle, "Player_Vehicle");

		Hash vehicleModel = ENTITY::GET_ENTITY_MODEL(vehicle);
		if (VEHICLE::IS_THIS_MODEL_A_CAR(vehicleModel) || VEHICLE::IS_THIS_MODEL_A_BIKE(vehicleModel))
		{
			VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 0, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 0) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 1, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 1) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 2, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 2) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 3, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 3) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 4, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 4) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 5, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 5) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 6, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 6) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 7, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 7) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 8, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 8) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 9, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 9) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 10, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 10) - 1, 10);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 11, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 11) - 1, 11);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 12, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 12) - 1, 12);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 13, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 13) - 1, 13);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 14, rand() % 32, 0);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 15, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 15) - 1, 1);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 16, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, 16) - 1, 1);
			VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 18, TRUE);
			VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 20, TRUE);
			VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, 20, 20, 20);
			VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 22, TRUE);
			VEHICLE::SET_VEHICLE_WHEEL_TYPE(vehicle, rand() % 7);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 23, rand() % 23, 0);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 24, rand() % 23, 0);
			VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, NULL, rand() % 157);
			VEHICLE::SET_VEHICLE_WINDOW_TINT(vehicle, 1);
			VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), 0, 1);
			VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), 1, 1);
			VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), 2, 1);
			VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), 3, 1);
			VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(vehicle, 32, 32, 32);
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle, rand() % 5);
			int modIndex = VEHICLE::GET_VEHICLE_MOD(vehicle, 23);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 23, modIndex, TRUE);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 24, modIndex, TRUE);
			VEHICLE::SET_VEHICLE_COLOURS(vehicle, 0, 0);
		}

		RepairVehicle(vehicle);
		SetVehicleModded(vehicle);
	}
}

void RepairVehicle(Vehicle vehicle)
{
	VEHICLE::SET_VEHICLE_FIXED(vehicle);
	VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(vehicle);
	VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(vehicle, 1000.0f);
	VEHICLE::SET_VEHICLE_BODY_HEALTH(vehicle, 1000.0f);
	VEHICLE::SET_VEHICLE_ENGINE_HEALTH(vehicle, 1000.0f);
	VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0.0f);
	//VEHICLE::_SET_VEHICLE_PAINT_FADE(vehicle, 0.0f);
}

void DumpPlayerFromVehicle(Ped ped)
{
	if (PED::IS_PED_IN_ANY_VEHICLE(ped, FALSE))
	{
		AI::CLEAR_PED_TASKS_IMMEDIATELY(ped);
		WEAPON::REMOVE_WEAPON_FROM_PED(ped, 0xFBAB5776);
	}
}

void SetVehicleModded(Vehicle vehicle)
{
	DECORATOR::DECOR_REGISTER("Veh_Modded_By_Player", 3);
	DECORATOR::DECOR_SET_INT(vehicle, "Veh_Modded_By_Player", GAMEPLAY::GET_HASH_KEY(PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID())));
}

void SpawnPlayerVehicle(Ped playerPed, Hash vehicleHash, bool spawnInVehicle, bool baseUpgrade, bool maxUpgrade)
{
	requestControl(playerPed);

	float currentSpeed = ENTITY::GET_ENTITY_SPEED(playerPed);
	Vector3 currentVelocity = ENTITY::GET_ENTITY_VELOCITY(playerPed);

	Vector3 spawnPos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0, 6, 0);

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, FALSE) && spawnInVehicle)
	{
		DumpPlayerFromVehicle(playerPed);
	}

	Vehicle vehicle = VEHICLE::CREATE_VEHICLE(vehicleHash, spawnPos.x, spawnPos.y, spawnPos.z, ENTITY::GET_ENTITY_HEADING(playerPed), TRUE, TRUE);

	requestControl(vehicle);

	VEHICLE::SET_VEHICLE_ENGINE_ON(vehicle, TRUE, TRUE, 1);

	if (baseUpgrade || maxUpgrade)
	{
		BoostBaseVehicleStats(vehicle);
		VEHICLE::SET_VEHICLE_COLOURS(vehicle, 0, 0);
		VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, 0, 0);
	}
	if (maxUpgrade)
	{
		MaxUpgrade(vehicle);
	}

	if (featureVehWrapInSpawned)
	{
		ENTITY::SET_ENTITY_VELOCITY(vehicle, currentVelocity.x, currentVelocity.y, max(-5, currentVelocity.z));
		PED::SET_PED_INTO_VEHICLE(playerPed, vehicle, -1);
		VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, currentSpeed);
	}

	ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&vehicle);
}

void SetPlateType(Vehicle vehicle, int plateindex)
{
	if (vehicle != NULL)
	{
		VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle, plateindex);
		SetVehicleModded(vehicle);
	}
}

void SetWindowTint(Vehicle vehicle, int tint)
{
	VEHICLE::SET_VEHICLE_WINDOW_TINT(vehicle, tint);
	SetVehicleModded(vehicle);
}

void ResprayVehicle(Vehicle vehicle, int primary, int secondary, int pearl, int wheel)
{
	VEHICLE::SET_VEHICLE_COLOURS(vehicle, primary, secondary);
	VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, pearl, wheel);
	SetVehicleModded(vehicle);
	DECORATOR::DECOR_SET_BOOL(vehicle, "Sprayed_Veicle_Decorator", TRUE);
	DECORATOR::DECOR_SET_TIME(vehicle, "Sprayed_Vehicle_Timer_Dec", NETWORK::GET_NETWORK_TIME());
}

void ToggleExtra(Vehicle vehicle, int extraID)
{
	if (VEHICLE::DOES_EXTRA_EXIST(vehicle, extraID))
	{
		requestControl(vehicle);
		VEHICLE::SET_VEHICLE_EXTRA(vehicle, extraID, VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(vehicle, extraID) ? -1 : 0);
		SetVehicleModded(vehicle);
	}
}

void ClaimVehicle(Vehicle vehicle)
{
	requestControl(vehicle);
	DECORATOR::DECOR_SET_INT(vehicle, "Player_Vehicle", NETWORK::_NETWORK_HASH_FROM_PLAYER_HANDLE(PLAYER::PLAYER_ID()));
	DECORATOR::DECOR_SET_INT(vehicle, "PV_Slot", 0);
	DECORATOR::DECOR_SET_INT(vehicle, "Not_Allow_As_Saved_Veh", GAMEPLAY::GET_HASH_KEY(PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID())));
}

void SetNumberPlate(Vehicle vehicle, char* text)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(vehicle, text);
	SetVehicleModded(vehicle);
}

bool DoesVehicleExist(char* name)
{
	return !GAMEPLAY::ARE_STRINGS_EQUAL("CARNOTFOUND", VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY(name)));
}

void SetVehicleMod(Vehicle vehicle, int modType, int modIndex)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::SET_VEHICLE_MOD(vehicle, modType, modIndex, TRUE);
}

void ToggleVehicleMod(Vehicle vehicle, int modType)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, modType, VEHICLE::IS_TOGGLE_MOD_ON(vehicle, modType) == 0 ? -1 : 0);
}

void SetVehicleLivery(Vehicle vehicle, int livery)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::SET_VEHICLE_LIVERY(vehicle, livery);
}

void SetTireSmoke(Vehicle vehicle, int r, int g, int b)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, r, g, b);
}

void ToggleNeonLight(Vehicle vehicle, int index)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(vehicle, index, !VEHICLE::_IS_VEHICLE_NEON_LIGHT_ENABLED(vehicle, index));
}

void MaxUpgrade(Vehicle vehicle)
{
	requestControl(vehicle);
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);

	for (int i = 0; i < 50; i++)
	{
		SetVehicleMod(vehicle, i, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, i) - 1);
	}
}

void(*periodic_feature_call)(void) = NULL;

void set_periodic_feature_call(void method(void))
{
	periodic_feature_call = method;
}

void make_periodic_feature_call()
{
	periodic_feature_call();
}

bool featureVehicleDoorInstant = false;
//Door Options list + struct
struct struct_door_options {
	std::string text;
	bool* pState;
};

std::vector<struct_door_options> DOOR_OPTIONS = {
	{ "Open Instantly", &featureVehicleDoorInstant },
	{ "Front Right", NULL }, //INDEX 0
	{ "Front Left", NULL }, //INDEX 1
	{ "Back Right", NULL }, //INDEX 2
	{ "Bacl Left", NULL }, //INDEX 3 (This opens the ramp on the An-225|CARGOPLANE)
	{ "Hood", NULL }, //INDEX 4
	{ "Trunk", NULL }, //INDEX 5 (Opens ramp on C-130|TITAN)
	{ "Trunk 2", NULL } //INDEX 6 (What uses this?)
};

int doorOptionsMenuIndex = 0;
const std::vector<std::string> MENU_VEHICLE_CATEGORIES{ "Cars", "Industrial", "Emergency and Military", "Motorcycles", "Planes", "Helicopters", "Boats", "Bicycles", "Enter Name Manually" };

//Cars

const std::vector<std::string> MENU_CAR_CATEGORIES{ "Supercars", "Sports", "Sport Classics", "Coupes", "Muscle", "Offroad", "SUVs", "Sedans", "Compacts" };

const std::vector<std::string> CAPTIONS_SUPERCARS{ "Coil Voltic", "Grotti Cheetah", "Grotti Turismo R", "Overflod Entity XF", "Pegassi Infernus", "Pegassi Osiris", "Pegassi Vacca", "Pegassi Zentorno", "Progen T20", "Truffade Adder", "Vapid Bullet" };

const std::vector<std::string> CAPTIONS_SPORTS{ "Albany Alpha", "Annis Elegy RH8", "Benefactor Feltzer", "Benefactor Schwartzer", "Benefactor Surano", "Bravado Banshee", "Bravado Buffalo", "Bravado Buffalo S", "Bravado Buffalo S (Race)", "Dewbauchee Massacro", "Dewbauchee Massacro (Race)", "Dewbauchee Rapid GT", "Dewbauchee Rapid GT Cabrio", "Dinka Blista Compact", "Dinka Blista Compact (Go Go Monkey Race)", "Dinka Jester", "Dinka Jester (Race)", "Grotti Carbonizzare", "Hijak Khamelion", "Invetero Coquette", "Karin Futo", "Karin Kuruma", "Karin Kuruma (Armoured)", "Karin Sultan", "Lampadati Furore GT", "Maibatsu Penumbra", "Obey 9F", "Obey 9F Cabrio", "Phister Comet", "Schyster Fusilade" };

const std::vector<std::string> CAPTIONS_SPORTCLASSICS{ "Albany Manana", "Albany Roosevelt", "Benefactor Stirling GT", "Declasse Tornado", "Declasse Tornado (Rusty)", "Declasse Tornado Cabrio", "Declasse Tornado Cabrio (Rusty)", "Dewbauchee JB 700", "Grotti Stinger", "Grotti Stinger GT", "Invetero Coquette BlackFin", "Invetero Coquette Classic", "Lampadati Casco", "Lampadati Pigalle", "Pegassi Monroe", "Truffade Z-Type", "Vapid Peyote" };

const std::vector<std::string> CAPTIONS_COUPES{ "Dewbauchee Exemplar", "Enus Cognoscenti Cabrio", "Enus Windsor", "Lampadati Felon", "Lampadati Felon GT", "Ocelot F620", "Ocelot Jackal", "Ubermacht Sentinel", "Ubermacht Sentinel XS", "Ubermacht Zion", "Ubermacht Zion Cabrio" };

const std::vector<std::string> CAPTIONS_MUSCLE{ "Albany Buccaneer", "Albany Virgo", "Bravado Gauntlet", "Bravado Gauntlet (Race)", "Cheval Picador", "Declasse Sabre Turbo", "Declasse Stallion", "Declasse Stallion (Race)", "Declasse Vigero", "Declasse Voodoo", "Imponte Duke O' Death", "Imponte Dukes", "Imponte Phoenix", "Imponte Ruiner", "Vapid Blade", "Vapid Chino", "Vapid Dominator", "Vapid Dominator (Race)", "Vapid Hotknife", "Vapid Slamvan", "Vapid Slamvan (Lost MC)" };

const std::vector<std::string> CAPTIONS_OFFROAD{ "Benefactor Dubsta 6x6", "BF Bifta", "BF Injection", "Bravado Dune", "Bravado Duneloader", "Bravado Space Docker", "Canis Bodhi", "Canis Kalahari", "Canis Mesa (Off-Road)", "Cheval Marshall", "Coil Brawler", "Declasse Rancher XL", "Declasse Rancher XL (Snow)", "Insurgent", "Insurgent (Gun Mount)", "Karin Rebel", "Karin Rebel (Rusty)", "Karin Technical", "Nagasaki Blazer", "Nagasaki Blazer (Hot Rod)", "Nagasaki Blazer (Lifeguard)", "Vapid Guardian", "Vapid Sandking", "Vapid Sandking XL", "Vapid The Liberator" };

const std::vector<std::string> CAPTIONS_SUVS{ "Albany Cavalcade", "Albany Cavalcade Mk2", "Benefactor Dubsta", "Benefactor Dubsta (Flat Black)", "Benefactor Serrano", "Bravado Gresley", "Canis Mesa", "Canis Mesa (Snow)", "Canis Seminole", "Declasse Granger", "Dundreary Landstalker", "Emperor Habanero", "Enus Huntley S", "Fathom FQ 2", "Gallivanter Baller (Large)", "Gallivanter Baller (Small)", "Karin BeeJay XL", "Mammoth Patriot", "Obey Rocoto", "Vapid Radius" };

const std::vector<std::string> CAPTIONS_SEDANS{ "Albany Emperor", "Albany Emperor (Rusty)", "Albany Emperor (Snow)", "Albany Primo", "Albany Washington", "Benefactor Glendale", "Benefactor Schafter", "Chariot Romero Hearse", "Cheval Fugitive", "Cheval Surge", "Declasse Asea", "Declasse Asea (Snow)", "Declasse Premier", "Dundreary Regina", "Dundreary Stretch", "Enus Super Diamond", "Karin Asterope", "Karin Intruder", "Obey Tailgater", "Ubermacht Oracle", "Ubermacht Oracle Mk2", "Vapid Stanier", "Vapid Stanier (Taxi)", "Vulcan Ingot", "Vulcar Warrener", "Zirconium Stratum" };

const std::vector<std::string> CAPTIONS_COMPACTS{ "Benefactor Panto", "Bollokan Prairie", "Declasse Rhapsody", "Dinka Blista", "Karin Dilettante", "Karin Dilettante (Liveried)", "Weeny Issi" };

const std::vector<std::string> VALUES_SUPERCARS{ "VOLTIC", "CHEETAH", "TURISMOR", "ENTITYXF", "INFERNUS", "OSIRIS", "VACCA", "ZENTORNO", "T20", "ADDER", "BULLET" };

const std::vector<std::string> VALUES_SPORTS{ "ALPHA", "ELEGY2", "FELTZER2", "SCHWARZER", "SURANO", "BANSHEE", "BUFFALO", "BUFFALO2", "BUFFALO3", "MASSACRO", "MASSACRO2", "RAPIDGT", "RAPIDGT2", "BLISTA2", "BLISTA3", "JESTER", "JESTER2", "CARBONIZZARE", "KHAMELION", "COQUETTE", "FUTO", "KURUMA", "KURUMA2", "SULTAN", "FUROREGT", "PENUMBRA", "NINEF", "NINEF2", "COMET2", "FUSILADE" };

const std::vector<std::string> VALUES_SPORTCLASSICS{ "MANANA", "BTYPE", "FELTZER3", "TORNADO", "TORNADO3", "TORNADO2", "TORNADO4", "JB700", "STINGER", "STINGERGT", "COQUETTE3", "COQUETTE2", "CASCO", "PIGALLE", "MONROE", "ZTYPE", "PEYOTE" };

const std::vector<std::string> VALUES_COUPES{ "EXEMPLAR", "COGCABRIO", "WINDSOR", "FELON", "FELON2", "F620", "JACKAL", "SENTINEL2", "SENTINEL", "ZION", "ZION2" };

const std::vector<std::string> VALUES_MUSCLE{ "BUCCANEER", "VIRGO", "GAUNTLET", "GAUNTLET2", "PICADOR", "SABREGT", "STALION", "STALION2", "VIGERO", "VOODOO2", "DUKES2", "DUKES", "PHOENIX", "RUINER", "BLADE", "CHINO", "DOMINATOR", "DOMINATOR2", "HOTKNIFE", "SLAMVAN", "SLAMVAN2" };

const std::vector<std::string> VALUES_OFFROAD{ "DUBSTA3", "BIFTA", "BFINJECTION", "DUNE", "DLOADER", "DUNE2", "BODHI2", "KALAHARI", "MESA3", "MARSHALL", "BRAWLER", "RANCHERXL", "RANCHERXL2", "INSURGENT2", "INSURGENT", "REBEL2", "REBEL", "TECHNICAL", "BLAZER", "BLAZER3", "BLAZER2", "GUARDIAN", "SANDKING2", "SANDKING", "MONSTER" };

const std::vector<std::string> VALUES_SUVS{ "CAVALCADE", "CAVALCADE2", "DUBSTA", "DUBSTA2", "SERRANO", "GRESLEY", "MESA", "MESA2", "SEMINOLE", "GRANGER", "LANDSTALKER", "HABANERO", "HUNTLEY", "FQ2", "BALLER", "BALLER2", "BJXL", "PATRIOT", "ROCOTO", "RADI" };

const std::vector<std::string> VALUES_SEDANS{ "EMPEROR", "EMPEROR2", "EMPEROR3", "PRIMO", "WASHINGTON", "GLENDALE", "SCHAFTER2", "ROMERO", "FUGITIVE", "SURGE", "ASEA", "ASEA2", "PREMIER", "REGINA", "STRETCH", "SUPERD", "ASTEROPE", "INTRUDER", "TAILGATER", "ORACLE", "ORACLE2", "STANIER", "TAXI", "INGOT", "WARRENER", "STRATUM" };

const std::vector<std::string> VALUES_COMPACTS{ "PANTO", "PRAIRIE", "RHAPSODY", "BLISTA", "DILETTANTE", "DILETTANTE2", "ISSI2" };

const std::vector<std::string> VOV_CAR_CAPTIONS[] = { CAPTIONS_SUPERCARS, CAPTIONS_SPORTS, CAPTIONS_SPORTCLASSICS, CAPTIONS_COUPES, CAPTIONS_MUSCLE, CAPTIONS_OFFROAD, CAPTIONS_SUVS, CAPTIONS_SEDANS, CAPTIONS_COMPACTS };

const std::vector<std::string> VOV_CAR_VALUES[] = { VALUES_SUPERCARS, VALUES_SPORTS, VALUES_SPORTCLASSICS, VALUES_COUPES, VALUES_MUSCLE, VALUES_OFFROAD, VALUES_SUVS, VALUES_SEDANS, VALUES_COMPACTS };

//Industrial

const std::vector<std::string> MENU_INDUS_CATEGORIES{ "Pickups", "Vans", "Trucks", "Service/Utility", "Trailers", "Trains (Undriveable)" };

const std::vector<std::string> CAPTIONS_PICKUPS{ "Bravado Bison", "Bravado Bison (Backrack)", "Bravado Bison (Construction)", "Bravado Ratloader", "Bravado Ratloader (Rusty)", "Vapid Bobcat", "Vapid Sadler", "Vapid Sadler (Snow)" };

const std::vector<std::string> CAPTIONS_VANS{ "BF Surfer", "BF Surfer (Rusty)", "Bravado Paradise", "Bravado Rumpo (Deludamol)", "Bravado Rumpo (Plain/Weazel)", "Bravado Youga", "Brute Camper", "Brute Pony (Business)", "Brute Pony (Cannibus Shop)", "Brute Taco Van", "Declasse Burrito", "Declasse Burrito (Bug Stars)", "Declasse Burrito (Construction)", "Declasse Burrito (Gang)", "Declasse Burrito (Graphics)", "Declasse Burrito (Snow)", "Declasse Burrito (The Lost)", "Vapid Minivan", "Vapid Speedo", "Vapid Speedo (Clown)", "Zirconium Journey" };

const std::vector<std::string> CAPTIONS_TRUCKS{ "Brute Boxville (Go Postal)", "Brute Boxville (Humane Labs)", "Brute Boxville (Post OP)", "Brute Boxville (Water & Power)", "Brute Stockade", "Brute Stockade (Snow)", "Brute Tipper (2 Axle)", "Brute Tipper (3 Axle)", "Cutter", "Dock Handler", "Dock Tug", "Dump Truck", "HVY Biff", "Jobuilt Hauler", "Jobuilt Phantom", "Jobuilt Rubble", "Maibatsu Mule (Graphics 1)", "Maibatsu Mule (Graphics 2)", "Maibatsu Mule (Plain)", "Mixer", "Mixer (Support Wheel)", "MTL Flatbed Truck", "MTL Packer", "MTL Pounder", "Vapid Benson", "Vapid Scrap Truck", "Vapid Tow Truck", "Vapid Tow Truck (Old)" };

const std::vector<std::string> CAPTIONS_SERVICES{ "Airtug", "Brute Airport Bus", "Brute Bus (City Bus)", "Brute Rental Shuttle Bus", "Brute Tourbus", "Cable Car (Mt. Chilliad)", "Dashound Coach", "Dozer", "Forklift", "Jobuilt Trashmaster", "Jobuilt Trashmaster (Rusty)", "Nagasaki Caddy (New)", "Nagasaki Caddy (Old)", "Ripley (Airport Tug)", "Stanley Fieldmaster Tractor", "Stanley Fieldmaster Tractor (Snow)", "Stanley Lawn Mower", "Stanley Tractor (Rusty)", "Vapid Pickup Utility", "Vapid Plumbing Utility", "Vapid Telephone Utility" };

const std::vector<std::string> CAPTIONS_TRAILERS{ "Army Flatbed Trailer (Empty)", "Army Flatbed Trailer (With Drill)", "Army Fuel Tanker", "Boat Trailer", "Boat Trailer (With Boat)", "Car Transport Trailer", "Car Transport Trailer (Empty)", "Commercial Trailer (Graphics 1)", "Commercial Trailer (Graphics 2)", "Container Trailer", "Fame or Shame Trailer", "Flatbed Trailer 1", "Flatbed Trailer 2", "Grain Trailer", "Hay Bale Trailer", "Logging Trailer", "Meth Lab Trailer", "Petrol Tanker Trailer (Plain)", "Petrol Tanker Trailer (RON)", "Plain Trailer", "Rake Trailer", "Small Trailer" };

const std::vector<std::string> CAPTIONS_TRAINS{ "Container Car 1", "Container Car 2", "Flatbed Car", "Freight Train Cab", "Grain Car", "Metro Train (Half)", "Oil Tanker Car" };

const std::vector<std::string> VALUES_PICKUPS{ "BISON", "BISON3", "BISON2", "RATLOADER2", "RATLOADER", "BOBCATXL", "SADLER", "SADLER2" };

const std::vector<std::string> VALUES_VANS{ "SURFER", "SURFER2", "PARADISE", "RUMPO2", "RUMPO", "YOUGA", "CAMPER", "PONY", "PONY2", "TACO", "BURRITO3", "BURRITO2", "BURRITO4", "GBURRITO2", "BURRITO", "BURRITO5", "GBURRITO", "MINIVAN", "SPEEDO", "SPEEDO2", "JOURNEY" };

const std::vector<std::string> VALUES_TRUCKS{ "BOXVILLE2", "BOXVILLE3", "BOXVILLE4", "BOXVILLE", "STOCKADE", "STOCKADE3", "TIPTRUCK", "TIPTRUCK2", "CUTTER", "HANDLER", "DOCKTUG", "DUMP", "BIFF", "HAULER", "PHANTOM", "RUBBLE", "MULE", "MULE2", "MULE3", "MIXER", "MIXER2", "FLATBED", "PACKER", "POUNDER", "BENSON", "SCRAP", "TOWTRUCK", "TOWTRUCK2" };

const std::vector<std::string> VALUES_SERVICES{ "AIRTUG", "AIRBUS", "BUS", "RENTALBUS", "TOURBUS", "CABLECAR", "COACH", "BULLDOZER", "FORKLIFT", "TRASH", "TRASH2", "CADDY", "CADDY2", "RIPLEY", "TRACTOR2", "TRACTOR3", "MOWER", "TRACTOR", "UTILLITRUCK3", "UTILLITRUCK2", "UTILLITRUCK" };

const std::vector<std::string> VALUES_TRAILERS{ "ARMYTRAILER", "ARMYTRAILER2", "ARMYTANKER", "BOATTRAILER", "TR3", "TR4", "TR2", "TRAILERS2", "TRAILERS3", "DOCKTRAILER", "TVTRAILER", "FREIGHTTRAILER", "TRFLAT", "GRAINTRAILER", "BALETRAILER", "TRAILERLOGS", "PROPTRAILER", "TANKER2", "TANKER", "TRAILERS", "RAKETRAILER", "TRAILERSMALL" };

const std::vector<std::string> VALUES_TRAINS{ "FREIGHTCONT1", "FREIGHTCONT2", "FREIGHTCAR", "FREIGHT", "FREIGHTGRAIN", "METROTRAIN", "TANKERCAR" };

const std::vector<std::string> VOV_INDUS_CAPTIONS[] = { CAPTIONS_PICKUPS, CAPTIONS_VANS, CAPTIONS_TRUCKS, CAPTIONS_SERVICES, CAPTIONS_TRAILERS, CAPTIONS_TRAINS };

const std::vector<std::string> VOV_INDUS_VALUES[] = { VALUES_PICKUPS, VALUES_VANS, VALUES_TRUCKS, VALUES_SERVICES, VALUES_TRAILERS, VALUES_TRAINS };

//Everything else

const std::vector<std::string> CAPTIONS_EMERGENCY{ "Albany Police Roadcruiser (Snow)", "Ambulance", "Army Barracks Truck", "Army Truck Cab", "Bravado Buffalo (FIB)", "Brute Police Riot Van", "Canis Crusader (Army Mesa)", "Declasse Granger (FIB)", "Declasse Lifeguard", "Declasse Park Ranger", "Declasse Police Rancher (Snow)", "Declasse Police Transporter", "Declasse Sheriff SUV", "Firetruck", "Prison Bus", "Rhino Tank", "Vapid Police Buffalo", "Vapid Police Cruiser", "Vapid Police Interceptor", "Vapid Sheriff Cruiser", "Vapid Unmarked Police Cruiser", "Western Police Bike" };

const std::vector<std::string> CAPTIONS_MOTORCYCLES{ "Dinka Akuma", "Dinka Double-T", "Dinka Enduro", "Dinka Thrust", "Dinka Vindicator", "LCC Hexer", "LCC Innovation", "Maibatsu Sanchez", "Maibatsu Sanchez (Graphics)", "Nagasaki Carbon RS", "Pegassi Bati", "Pegassi Bati (Race)", "Pegassi Ruffian", "Principe Faggio", "Principe Lectro", "Principe Nemesis", "Shitzu Hakuchou", "Shitzu PCJ 600", "Shitzu Vader", "Western Bagger", "Western Daemon", "Western Sovereign" };

const std::vector<std::string> CAPTIONS_PLANES{ "Buckingham Cargo Plane (An-225)", "Buckingham Jet (B747)", "Buckingham Luxor", "Buckingham Luxor Deluxe", "Buckingham Miljet", "Buckingham Shamal", "Buckingham Vestra", "Jobuilt Mammatus", "Jobuilt P-996 Lazer", "Jobuilt Velum (4 Seater)", "Jobuilt Velum (5 Seater)", "Mammoth Dodo", "Mammoth Hydra", "Mammoth Titan", "Western Besra", "Western Cuban 800", "Western Duster", "Western Mallard Stunt Plane" };

const std::vector<std::string> CAPTIONS_HELOS{ "Blimp (Atomic)", "Blimp (Xero Gas)", "Buckingham Savage", "Buckingham Swift", "Buckingham Swift Deluxe", "Buckingham Valkyrie", "HVY Skylift", "Maibatsu Frogger", "Maibatsu Frogger (TPE/FIB)", "Nagasaki Buzzard (Unarmed)", "Nagasaki Buzzard Attack Chopper", "Western Annihilator", "Western Cargobob (Desert Camo)", "Western Cargobob (Jetsam)", "Western Cargobob (TPE)", "Western Maverick", "Western Maverick (Emergency)" };

const std::vector<std::string> CAPTIONS_BOATS{ "Dinka Marquis", "Kraken Sub", "Lampadati Toro", "Nagasaki Dinghy (2 Seater)", "Nagasaki Dinghy (4 Seater, Black)", "Nagasaki Dinghy (4 Seater, Red)", "Pegassi Speeder", "Shitzu Jetmax", "Shitzu Predator (Police)", "Shitzu Squalo", "Shitzu Suntrap", "Shitzu Tropic", "Speedophile Seashark", "Speedophile Seashark (Lifeguard)", "Submersible" };

const std::vector<std::string> CAPTIONS_BICYCLES{ "BMX", "Cruiser", "Endurex Race", "Fixter", "Scorcher", "Tri-Cycles Race", "Whippet Race" };

const std::vector<std::string> VALUES_EMERGENCY{ "POLICEOLD2", "AMBULANCE", "BARRACKS", "BARRACKS2", "FBI", "RIOT", "CRUSADER", "FBI2", "LGUARD", "PRANGER", "POLICEOLD1", "POLICET", "SHERIFF2", "FIRETRUK", "PBUS", "RHINO", "POLICE2", "POLICE", "POLICE3", "SHERIFF", "POLICE4", "POLICEB" };

const std::vector<std::string> VALUES_MOTORCYCLES{ "AKUMA", "DOUBLE", "ENDURO", "THRUST", "VINDICATOR", "HEXER", "INNOVATION", "SANCHEZ2", "SANCHEZ", "CARBONRS", "BATI", "BATI2", "RUFFIAN", "FAGGIO2", "LECTRO", "NEMESIS", "HAKUCHOU", "PCJ", "VADER", "BAGGER", "DAEMON", "SOVEREIGN" };

const std::vector<std::string> VALUES_PLANES{ "CARGOPLANE", "JET", "LUXOR", "LUXOR2", "MILJET", "SHAMAL", "VESTRA", "MAMMATUS", "LAZER", "VELUM", "VELUM2", "DODO", "HYDRA", "TITAN", "BESRA", "CUBAN800", "DUSTER", "STUNT" };

const std::vector<std::string> VALUES_HELOS{ "BLIMP", "BLIMP2", "SAVAGE", "SWIFT", "SWIFT2", "VALKYRIE", "SKYLIFT", "FROGGER", "FROGGER2", "BUZZARD2", "BUZZARD", "ANNIHILATOR", "CARGOBOB", "CARGOBOB2", "CARGOBOB3", "MAVERICK", "POLMAV" };

const std::vector<std::string> VALUES_BOATS{ "MARQUIS", "SUBMERSIBLE2", "TORO", "DINGHY2", "DINGHY3", "DINGHY", "SPEEDER", "JETMAX", "PREDATOR", "SQUALO", "SUNTRAP", "TROPIC", "SEASHARK", "SEASHARK2", "SUBMERSIBLE" };

const std::vector<std::string> VALUES_BICYCLES{ "BMX", "CRUISER", "TRIBIKE2", "FIXTER", "SCORCHER", "TRIBIKE3", "TRIBIKE" };

const std::vector<std::string> VOV_SHALLOW_CAPTIONS[] = { CAPTIONS_EMERGENCY, CAPTIONS_MOTORCYCLES, CAPTIONS_PLANES, CAPTIONS_HELOS, CAPTIONS_BOATS, CAPTIONS_BICYCLES };

const std::vector<std::string> VOV_SHALLOW_VALUES[] = { VALUES_EMERGENCY, VALUES_MOTORCYCLES, VALUES_PLANES, VALUES_HELOS, VALUES_BOATS, VALUES_BICYCLES };


bool onconfirm_vehdoor_menu(int selection, std::string caption, int value) {
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (selection > 0) {
		if (bPlayerExists && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);

			float doorAngle = VEHICLE::GET_VEHICLE_DOOR_ANGLE_RATIO(veh, selection - 1); //Best way I could figure out to detect if the part is animated.
			if (doorAngle < 0.01) {
				VEHICLE::SET_VEHICLE_DOOR_OPEN(veh, selection - 1, false, featureVehicleDoorInstant);
			}
			else {
				VEHICLE::SET_VEHICLE_DOOR_SHUT(veh, selection - 1, featureVehicleDoorInstant);
			}
		}
	}
	else {
		featureVehicleDoorInstant = !featureVehicleDoorInstant;
		set_status_text((featureVehicleDoorInstant) ? "Open Instantly Enabled" : "Open Instantly Disabled");
	}
	return false;
}

bool process_veh_door_menu() {
	std::string caption = "DOOR OPTIONS";

	std::vector<std::string> menuCaptions;
	std::vector<int>menuIndexes;

	for (int i = 0; i < DOOR_OPTIONS.size(); i++) {
		menuCaptions.push_back(DOOR_OPTIONS[i].text);
		menuIndexes.push_back(i);
	}

	return draw_generic_menu<int>(menuCaptions, menuIndexes, doorOptionsMenuIndex, caption, onconfirm_vehdoor_menu, NULL, NULL);
}

bool onconfirm_carspawn_menu(int selection, std::string caption, int value)
{
	switch (selection)
	{
	case 0:
		process_spawn_menu_cars();
		break;
	case 1:
		process_spawn_menu_indus();
		break;
	default:
		process_spawn_menu_generic(selection);
	}
	return false;
}

bool process_carspawn_menu2()
{
	std::vector<int> menuIDs;
	for (int i = 0; i < MENU_VEHICLE_CATEGORIES.size(); i++)
	{
		menuIDs.push_back(i);
	}

	return draw_generic_menu<int>(MENU_VEHICLE_CATEGORIES, menuIDs, 0, "Vehicle Categories", onconfirm_carspawn_menu, NULL, NULL);
}

bool onconfirm_spawn_menu_cars(int selection, std::string caption, int value)
{
	std::string category = MENU_CAR_CATEGORIES[selection];

	std::vector<std::string> vov_captions = VOV_CAR_CAPTIONS[selection];
	std::vector<std::string> vov_values = VOV_CAR_VALUES[selection];

	return draw_generic_menu<std::string>(vov_captions, vov_values, 0, category, onconfirm_spawn_menu_vehicle_selection, NULL, NULL);
}

bool process_spawn_menu_cars()
{
	std::vector<int> menuIDs;
	for (int i = 0; i < MENU_CAR_CATEGORIES.size(); i++)
	{
		menuIDs.push_back(i);
	}

	return draw_generic_menu<int>(MENU_CAR_CATEGORIES, menuIDs, 0, "Car Categories", onconfirm_spawn_menu_cars, NULL, NULL);
}

bool onconfirm_spawn_menu_indus(int selection, std::string caption, int value)
{
	std::string category = MENU_INDUS_CATEGORIES[selection];

	std::vector<std::string> vov_captions = VOV_INDUS_CAPTIONS[selection];
	std::vector<std::string> vov_values = VOV_INDUS_VALUES[selection];

	return draw_generic_menu<std::string>(vov_captions, vov_values, 0, category, onconfirm_spawn_menu_vehicle_selection, NULL, NULL);
}

bool process_spawn_menu_indus()
{
	std::vector<int> menuIDs;
	for (int i = 0; i < MENU_INDUS_CATEGORIES.size(); i++)
	{
		menuIDs.push_back(i);
	}

	return draw_generic_menu<int>(MENU_INDUS_CATEGORIES, menuIDs, 0, "Industrial Categories", onconfirm_spawn_menu_indus, NULL, NULL);
}

bool onconfirm_spawn_menu_vehicle_selection(int selection, std::string caption, std::string value)
{
	do_spawn_vehicle(value, caption);
	return false;
}

bool process_spawn_menu_generic(int topMenuSelection)
{
	std::string category = MENU_VEHICLE_CATEGORIES[topMenuSelection];

	std::vector<std::string> vov_captions = VOV_SHALLOW_CAPTIONS[topMenuSelection - 2];
	std::vector<std::string> vov_values = VOV_SHALLOW_VALUES[topMenuSelection - 2];

	return draw_generic_menu<std::string>(vov_captions, vov_values, 0, category, onconfirm_spawn_menu_vehicle_selection, NULL, NULL);
}

bool do_spawn_vehicle(std::string modelName, std::string modelTitle)
{
	DWORD model = GAMEPLAY::GET_HASH_KEY((char*)modelName.c_str());
	if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_A_VEHICLE(model))
	{
		STREAMING::REQUEST_MODEL(model);
		while (!STREAMING::HAS_MODEL_LOADED(model)) WAIT(0);
		Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 5.0, 0.0);
		Vehicle veh = VEHICLE::CREATE_VEHICLE(model, coords.x, coords.y, coords.z, 0.0, 1, 1);
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh);

		if (featureVehWrapInSpawned)
		{
			ENTITY::SET_ENTITY_HEADING(veh, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()));
			PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), veh, -1);
		}

		WAIT(0);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
		ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);

		std::ostringstream ss;
		ss << modelTitle << " spawned";
		set_status_text(ss.str());

		return true;
	}
	return false;
}

#pragma endregion
int carspawnActiveLineIndex = 0;
int carspawnActiveItemIndex = 0;
bool process_carspawn_menu()
{
	DWORD waitTime = 200;
	const int lineCount = 37;
	const int itemCount = 10;
	const int itemCountLastLine = 1;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			char caption[32];
			sprintf_s(caption, "~r~Car ~w~Spawner  %d / %d", carspawnActiveLineIndex + 1, lineCount);
			draw_menu_line(caption, 350.0, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < itemCount; i++)
				if (strlen(vehicleModelsCase[carspawnActiveLineIndex][i]))
					draw_menu_line(vehicleModelsCase[carspawnActiveLineIndex][i], 100.0f, 5.0f, 200.0f, 100.0f + i * 110.0f, 5.0f, i == carspawnActiveItemIndex, FALSE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		bool bSelect, bBack, bUp, bDown, bLeft, bRight;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, &bLeft, &bRight);

		if (bSelect)
		{
			menu_beep_select();
			LPCSTR modelName = vehicleModelsCase[carspawnActiveLineIndex][carspawnActiveItemIndex];
			DWORD model = GAMEPLAY::GET_HASH_KEY((char*)modelName);
			if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && STREAMING::IS_MODEL_A_VEHICLE(model))
			{
				STREAMING::REQUEST_MODEL(model);
				while (!STREAMING::HAS_MODEL_LOADED(model)) WAIT(0);
				Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
				SpawnPlayerVehicle(PLAYER::PLAYER_PED_ID(), model, TRUE, TRUE, TRUE);
				WAIT(0);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
				ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
				std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
				char statusText[32];
				sprintf_s(statusText, " ~g~a %s was spawned.", modelName);
				set_status_text(playerName + statusText);
				return TRUE;
			}
		}
		else
			if (bBack)
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bRight)
				{
					menu_beep_up_down();
					carspawnActiveItemIndex++;
					int itemsMax = (carspawnActiveLineIndex == (lineCount - 1)) ? itemCountLastLine : itemCount;
					if (carspawnActiveItemIndex == itemsMax)
						carspawnActiveItemIndex = 0;
					waitTime = 100;
				}
				else
					if (bLeft)
					{
						menu_beep_up_down();
						if (carspawnActiveItemIndex == 0)
							carspawnActiveItemIndex = (carspawnActiveLineIndex == (lineCount - 1)) ? itemCountLastLine : itemCount;
						carspawnActiveItemIndex--;
						waitTime = 100;
					}
					else
						if (bUp)
						{
							menu_beep_up_down();
							if (carspawnActiveLineIndex == 0)
								carspawnActiveLineIndex = lineCount;
							carspawnActiveLineIndex--;
							waitTime = 200;
						}
						else
							if (bDown)
							{
								menu_beep_up_down();
								carspawnActiveLineIndex++;
								if (carspawnActiveLineIndex == lineCount)
									carspawnActiveLineIndex = 0;
								waitTime = 200;
							}
		if (carspawnActiveLineIndex == (lineCount - 1))
			if (carspawnActiveItemIndex >= itemCountLastLine)
				carspawnActiveItemIndex = 0;
	}
	return FALSE;
}

int activeLineIndexVehicle = 0;
void process_vehicle_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 21;

	std::string caption = "~r~Vehicle ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "To Last Vehicle",		NULL, NULL },
		{ "Car Spawner 1",		NULL, NULL },
		{ "Car Spawner 2", NULL, NULL },
		{ "Door Control", NULL, NULL },
		{ "Hash Spawner",		NULL, NULL },
		{ "Upgrade",		NULL, NULL },
		{ "Paint Random",	NULL, NULL },
		{ "Fix",				NULL, NULL },
		{ "Boost Stats",				NULL, NULL },
		{ "Seat Belt",		&featureVehSeatbelt, &featureVehSeatbeltUpdated },
		{ "Wrap In Spawned",	&featureVehWrapInSpawned, NULL },
		{ "Invincible",		&featureVehInvincible, &featureVehInvincibleUpdated },
		{ "Invisible",		&featureVehInvisible, &featureVehInvisibleUpdated },
		{ "Strong Wheels",	&featureVehInvincibleWheels, &featureVehInvincibleWheelsUpdated },
		{ "Speed Boost",		&featureVehSpeedBoost, NULL },
		{ "Rainbow Paint",		&featureVehRainbow, NULL },
		{ "Drift",		&featureVehDrift, NULL },
		{ "Grip",		&featureVehGrip, NULL },
		{ "Slammed",		&featureVehSlammed, &featureVehSlammedUpdated },
		{ "Auto Fix",		&featureVehAutoFix, NULL },
		{ "Damage Impact",		&featureVehDamageModifier, &featureVehDamageModifierUpdate },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexVehicle)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexVehicle].text, lines[activeLineIndexVehicle].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexVehicle * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();

			// common variables
			BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
			Player player = PLAYER::PLAYER_ID();
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			switch (activeLineIndexVehicle)
			{
			case 0:
				if (bPlayerExists)
				{
					ENTITY::SET_ENTITY_HEADING(PLAYER::GET_PLAYERS_LAST_VEHICLE(), ENTITY::GET_ENTITY_HEADING(playerPed));
					PED::SET_PED_INTO_VEHICLE(playerPed, PLAYER::GET_PLAYERS_LAST_VEHICLE(), -1);
				}
				else
				{
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~your vehicle isn't found.";
					set_status_text(playerName + text);
				}
				break;
			case 1:
				if (process_carspawn_menu()) return;
				break;
			case 2:
				if (process_carspawn_menu2()) return;
				break;
			case 3:
				if (process_veh_door_menu()) return;
				break;
			case 4:
				if (bPlayerExists)
				{
					GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(TRUE, "FMMC_KEY_TIP8", "", "f620", "", "", "", 12);
					while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(0);
					char* keyboardResult = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
					Hash hash = GAMEPLAY::GET_HASH_KEY(keyboardResult);
					if (STREAMING::IS_MODEL_IN_CDIMAGE(hash) && STREAMING::IS_MODEL_A_VEHICLE(hash))
					{
						STREAMING::REQUEST_MODEL(hash);
						while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(0);
						Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
						SpawnPlayerVehicle(PLAYER::PLAYER_PED_ID(), hash, TRUE, TRUE, TRUE);
						WAIT(0);
						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
						ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);

						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						char statusText[32];
						sprintf_s(statusText, " ~g~a %s was spawned.", keyboardResult);
						set_status_text(playerName + statusText);
					}
				}
				break;
			case 5:
				if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
				{
					Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
					upgrade(veh, playerPed);
					if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, FALSE)) return;
					GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(TRUE, "FMMC_KEY_TIP8", "", VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(PED::GET_VEHICLE_PED_IS_IN(playerPed, FALSE)), "", "", "", 12);
					while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(0);
					if (!GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT() ||
						!PED::IS_PED_IN_ANY_VEHICLE(playerPed, FALSE)) return;
					VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(PED::GET_VEHICLE_PED_IS_IN(playerPed, FALSE), GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT());
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~g~your vehicle was upgraded.";
					set_status_text(playerName + text);
				}
				else
				{
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~you aren't in a vehicle.";
					set_status_text(playerName + text);
				}
				break;
			case 6:
				if (bPlayerExists)
				{
					if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
					{
						Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
						int colorPrimary = 0, ColorSecondary = 0;
						requestControl(veh);
						VEHICLE::GET_VEHICLE_COLOURS(veh, &colorPrimary, &ColorSecondary);
						if (rand() % 2)
							VEHICLE::SET_VEHICLE_COLOURS(veh, rand() % (159 + 1), ColorSecondary);
						else
							VEHICLE::SET_VEHICLE_COLOURS(veh, colorPrimary, rand() % (159 + 1));
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~g~your vehicle color was changed.";
						set_status_text(playerName + text);
					}
					else
					{
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~r~you aren't in a vehicle.";
						set_status_text(playerName + text);
					}
				}
				break;
			case 7:
				if (bPlayerExists)
					if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
					{
						Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
						requestControl(veh);
						VEHICLE::SET_VEHICLE_FIXED(veh);
						VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh);
						VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(veh);
						VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0);
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~g~nice vehicle.";
						set_status_text(playerName + text);
					}
					else
					{
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~r~you aren't in a vehicle.";
						set_status_text(playerName + text);
					}
				break;
			case 8:
				if (bPlayerExists)
					if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
					{
						Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
						requestControl(veh);
						boostVehicleStats(veh);
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~g~your vehicle was upgraded.";
						set_status_text(playerName + text);
					}
					else
					{
						std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
						std::string text = " ~r~you aren't in a vehicle.";
						set_status_text(playerName + text);
					}
				break;
				// switchable features
			default:
				if (lines[activeLineIndexVehicle].pState)
					*lines[activeLineIndexVehicle].pState = !(*lines[activeLineIndexVehicle].pState);
				if (lines[activeLineIndexVehicle].pUpdated)
					*lines[activeLineIndexVehicle].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexVehicle == 0)
						activeLineIndexVehicle = lineCount;
					activeLineIndexVehicle--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexVehicle++;
						if (activeLineIndexVehicle == lineCount)
							activeLineIndexVehicle = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexNearbyPeds = 0;
void process_nearbypeds_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 10;

	std::string caption = "~r~Nearby ~w~Peds";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
	{ "Nearby Peds",	&featureNearbyPeds,	NULL },
	{ "Set Calm 1",	&featureSetPedCalm0,	NULL },
	{ "Set Calm 2",	&featureSetPedCalm1, NULL },
	{ "Kill",	&featureKillPed, NULL },
	{ "Rich",	&featureRichPed, NULL },
	{ "Revive Loop",	&featureRevivePed, NULL },
	{ "Revive",	NULL, NULL },
	{ "Upgrade Veh",	NULL, NULL },
	{ "Attach To Me",	NULL, NULL },
	{ "ESP",	&featureEspPed, NULL },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexNearbyPeds)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexNearbyPeds].text, lines[activeLineIndexNearbyPeds].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexNearbyPeds * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			switch (activeLineIndexNearbyPeds)
			{
			case 0:
				featureNearbyPeds = !featureNearbyPeds;
				break;
			case 5:
				featureRevivePed = !featureRevivePed;
				break;
			case 6:
			{
				const int amount = 20;
				const int arrSize = amount * 2 + 2;
				int peds[arrSize];
				peds[0] = amount;
				int count = PED::GET_PED_NEARBY_PEDS(PLAYER::PLAYER_PED_ID(), peds, -1);
				for (int i = 0; i < count; ++i)
				{
					int offsettedID = i * 2 + 2;
					if (ENTITY::DOES_ENTITY_EXIST(peds[offsettedID]))
					{
						if ((peds[offsettedID]) == PLAYER::PLAYER_PED_ID()) continue;
						{
							requestControl(peds[offsettedID]);
							PED::REVIVE_INJURED_PED(peds[offsettedID]);
							PED::RESURRECT_PED(peds[offsettedID]);
						}
					}
				}
			}
			break;
			case 7:
				nearbyVehicles(PLAYER::PLAYER_PED_ID());
				break;
			case 8:
			{
				const int amount = 20;
				const int arrSize = amount * 2 + 2;
				int peds[arrSize];
				peds[0] = amount;
				int count = PED::GET_PED_NEARBY_PEDS(PLAYER::PLAYER_PED_ID(), peds, -1);
				for (int i = 0; i < count; ++i)
				{
					int offsettedID = i * 2 + 2;
					if (ENTITY::DOES_ENTITY_EXIST(peds[offsettedID]))
					{
						if ((peds[offsettedID]) == PLAYER::PLAYER_PED_ID()) continue;
						{
							requestControl(peds[offsettedID]);
							ENTITY::ATTACH_ENTITY_TO_ENTITY(peds[offsettedID], PLAYER::PLAYER_PED_ID(), PED::GET_PED_BONE_INDEX(PLAYER::PLAYER_PED_ID(), 0xe0fd), 0.0f, 0.0f, 0.0f, 0.0f, 90.0f, 0.0f, FALSE, FALSE, FALSE, FALSE, 2, TRUE);
						}
					}
				}
			}
			break;
			// switchable features
			default:
				if (lines[activeLineIndexNearbyPeds].pState)
					*lines[activeLineIndexNearbyPeds].pState = !(*lines[activeLineIndexNearbyPeds].pState);
				if (lines[activeLineIndexNearbyPeds].pUpdated)
					*lines[activeLineIndexNearbyPeds].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexNearbyPeds == 0)
						activeLineIndexNearbyPeds = lineCount;
					activeLineIndexNearbyPeds--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexNearbyPeds++;
						if (activeLineIndexNearbyPeds == lineCount)
							activeLineIndexNearbyPeds = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexWorld = 0;
void process_world_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 9;

	std::string caption = "~r~World ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "Moon Gravity",	&featureWorldMoonGravity,	NULL },
		{ "Clear Vehicles",	&worldClearVehicles,	NULL },
		{ "Clear Objects",	&worldClearObjects,	NULL },
		{ "Clear Peds",	&worldClearPeds,	NULL },
		{ "Clear Cops",	&worldClearCops,	NULL },
		{ "Clear Projectiles",	&worldClearProjectiles,	NULL },
		{ "Nearby Peds",	NULL,	NULL },
		{ "Massacre Mode",	&worldMassacre,	NULL },
		{ "Map Mod",	NULL,	NULL },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexWorld)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexWorld].text, lines[activeLineIndexWorld].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexWorld * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			switch (activeLineIndexWorld)
			{
			case 0:
				featureWorldMoonGravity = !featureWorldMoonGravity;
				GAMEPLAY::SET_GRAVITY_LEVEL(featureWorldMoonGravity ? 2 : 0);
				break;
			case 1:
				worldClearVehicles = !worldClearVehicles;
				break;
			case 2:
				worldClearObjects = !worldClearObjects;
				break;
			case 3:
				worldClearPeds = !worldClearPeds;
				break;
			case 4:
				worldClearCops = !worldClearCops;
				break;
			case 5:
				worldClearProjectiles = !worldClearProjectiles;
				break;
			case 6:
			{
				std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
				std::string text = " ~y~you must be far away from other players.";
				set_status_text(playerName + text);
				process_nearbypeds_menu();
			}
			break;
			case 7:
				worldMassacre = !worldMassacre;
				break;
			case 8:
			{
				Hash tempHash = GAMEPLAY::GET_HASH_KEY("prop_container_01mb");
				PlaceObject(tempHash, 509.8423f, 5589.2422f, 791.0656f, 0.1410f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 520.5002f, 5584.3774f, 790.5033f, 5.4410f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 531.0571f, 5579.5405f, 788.6912f, 12.4410f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 541.3253f, 5574.8403f, 785.4896f, 19.4409f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 551.0662f, 5570.3701f, 780.7990f, 27.5407f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 560.1738f, 5566.2046f, 774.6979f, 35.0403f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 568.6718f, 5562.3198f, 767.4281f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 576.9716f, 5558.5269f, 759.5663f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 585.2493f, 5554.7471f, 751.7451f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 593.5072f, 5550.9722f, 743.9170f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 601.7770f, 5547.1912f, 736.0764f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 610.0651f, 5543.3994f, 728.2167f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 618.3337f, 5539.6226f, 720.3861f, 40.7936f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 626.6017f, 5535.8477f, 712.5473f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 634.8616f, 5532.0669f, 704.7252f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 643.1213f, 5528.2861f, 696.8940f, 40.7936f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 651.3914f, 5524.5059f, 689.0526f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 659.6512f, 5520.7275f, 681.2211f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 667.9110f, 5516.9424f, 673.3893f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 676.1708f, 5513.1670f, 665.5580f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 684.4307f, 5509.3789f, 657.7266f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 692.6906f, 5505.6079f, 649.9052f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 700.9504f, 5501.8271f, 642.0737f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 709.2201f, 5498.0464f, 634.2426f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 717.4602f, 5494.2759f, 626.4309f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 725.7202f, 5490.4980f, 618.5996f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 733.9800f, 5486.7226f, 610.7783f, 40.7396f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 742.5997f, 5482.7764f, 603.1669f, 36.9395f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 751.8304f, 5478.5518f, 596.3347f, 31.0392f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 761.7103f, 5474.0220f, 590.6132f, 24.5989f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 772.0702f, 5469.2827f, 586.0803f, 18.9288f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 782.8400f, 5464.3433f, 582.8604f, 11.5788f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 793.8899f, 5459.2847f, 581.1174f, 5.0787f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 805.1001f, 5454.1479f, 580.8762f, -2.5212f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 816.1702f, 5449.0796f, 581.9746f, -7.6213f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 827.1907f, 5444.0405f, 584.5823f, -16.6212f, 0.0f, 65.3998f);
				PlaceObject(tempHash, 837.6807f, 5439.2407f, 588.8990f, -24.4210f, 0.0f, 65.3998f);
				set_status_text("Map mod by ~b~XBLToothpik");
				WAIT(0);
				if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0))
				{
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 502.967102f, 5592.634766f, 795.225403f, 0, 0, 0, 1);
				}
				else
				{
					Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
					requestControl(veh);
					ENTITY::SET_ENTITY_COORDS(veh, 502.967102f, 5592.634766f, 795.225403f, 0, 0, 0, 1);
				}
			}
			break;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexWorld == 0)
						activeLineIndexWorld = lineCount;
					activeLineIndexWorld--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexWorld++;
						if (activeLineIndexWorld == lineCount)
							activeLineIndexWorld = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexMiscellaneous = 0;
void process_miscellaneous_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 7;

	std::string caption = "~r~Misc ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "Globe Radio",	&featureMiscGlobeRadio,					&featureMiscGlobeRadioUpdated },
		{ "Next Radio Track",	NULL,					NULL },
		{ "Hide Hud",			&featureMiscHideHud,	NULL },
		{ "Object Hash",	NULL,					NULL },
		{ "Bodyguard Hash",	NULL,					NULL },
		{ "Bodyguard Invincible",	&featurePedestrianInvincibleBodyguardUpdate,					&featurePedestrianInvincibleBodyguardUpdate },
		{ "Restricted Zones",			&featureMiscRestrictedZones,					NULL }
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexMiscellaneous)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexMiscellaneous].text, lines[activeLineIndexMiscellaneous].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexMiscellaneous * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			switch (activeLineIndexMiscellaneous)
			{
				// next radio track
			case 1:
				if (ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()))
				{
					AUDIO::SKIP_RADIO_FORWARD();
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~g~great channel section.";
					set_status_text(playerName + text);
				}
				break;
			case 3:
				// create object
				if (ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()))
				{
					GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(TRUE, "FMMC_KEY_TIP8", "", "p_spinning_anus_s", "", "", "", 20);
					while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(0);
					if (!STREAMING::IS_MODEL_IN_CDIMAGE(GAMEPLAY::GET_HASH_KEY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())))
					{
						return;
					}
					else
					{
						Vector3 get_entity_coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 0);
						char* get_onscreen_keyboard_result = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
						Hash get_hash_key = GAMEPLAY::GET_HASH_KEY(get_onscreen_keyboard_result);
						STREAMING::REQUEST_MODEL(get_hash_key);
						while (!STREAMING::HAS_MODEL_LOADED(get_hash_key))
							WAIT(0);
						Object create_object = OBJECT::CREATE_OBJECT(get_hash_key, get_entity_coords.x, get_entity_coords.y + 10.0f, get_entity_coords.z + 10.0f, TRUE, TRUE, FALSE);
						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(get_hash_key);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~g~you should turn on force gun.";
					set_status_text(playerName + text);
				}
				break;
			case 4:
				// create bodyguard
				bodyguard(PLAYER::PLAYER_PED_ID(), PLAYER::PLAYER_ID());
				break;
				// switchable features
			default:
				if (lines[activeLineIndexMiscellaneous].pState)
					*lines[activeLineIndexMiscellaneous].pState = !(*lines[activeLineIndexMiscellaneous].pState);
				if (lines[activeLineIndexMiscellaneous].pUpdated)
					*lines[activeLineIndexMiscellaneous].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexMiscellaneous == 0)
						activeLineIndexMiscellaneous = lineCount;
					activeLineIndexMiscellaneous--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexMiscellaneous++;
						if (activeLineIndexMiscellaneous == lineCount)
							activeLineIndexMiscellaneous = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexAnimation = 0;
void process_animation_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 39;

	std::string caption = "~r~Animation ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "The Bird", NULL, NULL },
		{ "The Bird 2", NULL, NULL },
		{ "Up Yours", NULL, NULL },
		{ "Dock", NULL, NULL },
		{ "Jerk", NULL, NULL },
		{ "Knuckle Crunch", NULL, NULL },
		{ "Shush", NULL, NULL },
		{ "Dj", NULL, NULL },
		{ "Rock", NULL, NULL },
		{ "Air Guitar", NULL, NULL },
		{ "Wave", NULL, NULL },
		{ "Salute", NULL, NULL },
		{ "Thanks", NULL, NULL },
		{ "Thumbs Up", NULL, NULL },
		{ "Face Palm", NULL, NULL },
		{ "Nose Pick", NULL, NULL },
		{ "Slow Clap", NULL, NULL },
		{ "Photography", NULL, NULL },
		{ "Blow Kiss", NULL, NULL },
		{ "Jazz Hands", NULL, NULL },
		{ "Surrender", NULL, NULL },
		{ "Air Synth", NULL, NULL },
		{ "Air Shagging", NULL, NULL },
		{ "Pole Dance 1", NULL, NULL },
		{ "Pole Dance 2", NULL, NULL },
		{ "Pole Dance 3", NULL, NULL },
		{ "Stunned", NULL, NULL },
		{ "Situps", NULL, NULL },
		{ "Pushups", NULL, NULL },
		{ "Wave Arms", NULL, NULL },
		{ "Suicide", NULL, NULL },
		{ "On The Can", NULL, NULL },
		{ "On Fire", NULL, NULL },
		{ "Cower", NULL, NULL },
		{ "Private Dance", NULL, NULL },
		{ "Blowjob", NULL, NULL },
		{ "Stungun", NULL, NULL },
		{ "Sex 1", NULL, NULL },
		{ "Sex 2", NULL, NULL },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexAnimation)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, -0.8f, 60.0f + i * 17.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexAnimation].text, lines[activeLineIndexAnimation].pState),
				lineWidth + 0.0f, -0.8f, 60.0f + activeLineIndexAnimation * 17.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		// common variables
		BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
		Player player = PLAYER::PLAYER_ID();
		Ped playerPed = PLAYER::PLAYER_PED_ID();

		if (bSelect)
		{
			menu_beep_select();
			switch (activeLineIndexAnimation)
			{
			case 0:
				animation(playerPed, "mp_player_intfinger", "mp_player_int_finger");
				break;
			case 1:
				animation(playerPed, "mp_player_int_upperfinger", "mp_player_int_finger_01");
				break;
			case 2:
				animation(playerPed, "mp_player_int_upperfinger", "mp_player_int_finger_02");
				break;
			case 3:
				animation(playerPed, "anim@mp_player_intcelebrationmale@dock", "dock");
				break;
			case 4:
				animation(playerPed, "mp_player_intwank", "mp_player_int_wank");
				break;
			case 5:
				animation(playerPed, "anim@mp_player_intcelebrationmale@knuckle_crunch", "knuckle_crunch");
				break;
			case 6:
				animation(playerPed, "anim@mp_player_intcelebrationmale@shush", "shush");
				break;
			case 7:
				animation(playerPed, "anim@mp_player_intcelebrationmale@dj", "dj");
				break;
			case 8:
				animation(playerPed, "mp_player_introck", "mp_player_int_rock");
				break;
			case 9:
				animation(playerPed, "anim@mp_player_intcelebrationmale@air_guitar", "air_guitar");
				break;
			case 10:
				animation(playerPed, "anim@mp_player_intcelebrationmale@wave", "wave");
				break;
			case 11:
				animation(playerPed, "mp_player_intsalute", "mp_player_int_salute");
				break;
			case 12:
				animation(playerPed, "mp_action", "thanks_male_06");
				break;
			case 13:
				animation(playerPed, "anim@mp_player_intcelebrationmale@thumbs_up", "thumbs_up");
				break;
			case 14:
				animation(playerPed, "anim@mp_player_intcelebrationmale@face_palm", "face_palm");
				break;
			case 15:
				animation(playerPed, "anim@mp_player_intcelebrationmale@nose_pick", "nose_pick");
				break;
			case 16:
				animation(playerPed, "anim@mp_player_intcelebrationfemale@slow_clap", "slow_clap");
				break;
			case 17:
				animation(playerPed, "anim@mp_player_intcelebrationmale@photography", "photography");
				break;
			case 18:
				animation(playerPed, "anim@mp_player_intcelebrationmale@blow_kiss", "blow_kiss");
				break;
			case 19:
				animation(playerPed, "anim@mp_player_intcelebrationmale@jazz_hands", "jazz_hands");
				break;
			case 20:
				animation(playerPed, "anim@mp_player_intcelebrationmale@surrender", "surrender");
				break;
			case 21:
				animation(playerPed, "anim@mp_player_intcelebrationmale@air_synth", "air_synth");
				break;
			case 22:
				animation(playerPed, "anim@mp_player_intcelebrationmale@air_shagging", "air_shagging");
				break;
			case 23:
				animation(playerPed, "mini@strip_club@pole_dance@pole_dance1", "pd_dance_01");
				break;
			case 24:
				animation(playerPed, "mini@strip_club@pole_dance@pole_dance2", "pd_dance_02");
				break;
			case 25:
				animation(playerPed, "mini@strip_club@pole_dance@pole_dance3", "pd_dance_03");
				break;
			case 26:
				animation(playerPed, "ragdoll@human", "electrocute");
				break;
			case 27:
				animation(playerPed, "amb@world_human_sit_ups@male@base", "base");
				break;
			case 28:
				animation(playerPed, "amb@world_human_push_ups@male@base", "base");
				break;
			case 29:
				animation(playerPed, "random@car_thief@waving_ig_1", "waving");
				break;
			case 30:
				animation(playerPed, "mp_suicide", "pistol_fp");
				break;
			case 31:
				animation(playerPed, "timetable@trevor@on_the_toilet", "trevonlav_struggleloop");
				break;
			case 32:
				animation(playerPed, "ragdoll@human", "on_fire");
				break;
			case 33:
				animation(playerPed, "random@arrests", "kneeling_arrest_idle");
				break;
			case 34:
				animation(playerPed, "mini@strip_club@private_dance@part1", "priv_dance_p1");
				break;
			case 35:
				animation(playerPed, "mini@prostitutes@sexnorm_veh", "bj_loop_prostitute");
				break;
			case 36:
				animation(playerPed, "stungun@standing", "damage");
				break;
			case 37:
				animation(playerPed, "rcmpaparazzo_2", "shag_loop_a");
				break;
			case 38:
				animation(playerPed, "rcmpaparazzo_2", "shag_loop_poppy");
				break;
				// switchable features
			default:
				if (lines[activeLineIndexAnimation].pState)
					*lines[activeLineIndexAnimation].pState = !(*lines[activeLineIndexAnimation].pState);
				if (lines[activeLineIndexAnimation].pUpdated)
					*lines[activeLineIndexAnimation].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexAnimation == 0)
						activeLineIndexAnimation = lineCount;
					activeLineIndexAnimation--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexAnimation++;
						if (activeLineIndexAnimation == lineCount)
							activeLineIndexAnimation = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexOnline2 = 0;
void process_online_menu2()
{
	const float lineWidth = 200.0;
	const int lineCount = 11;

	std::string caption = "~r~Lobby ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "Show Voice Chat Speaker", &featureShowVoiceChatSpeaker, NULL, },
		{ "Player Blips", &featurePlayerBlips, NULL, },
		{ "Player Head Display", &featurePlayerHeadDisplay, NULL, },
		{ "Player Blip Cone (Police FOV)", &featurePlayerBlipCone, NULL, },
		{ "Player Notifications", &featurePlayerNotifications, NULL, },
		{ "Remove Weapons", NULL, NULL },
		{ "Explode", NULL, NULL },
		{ "Vehicle Fire", NULL, NULL },
		{ "Plane Fire", NULL, NULL },
		{ "Lock In Veh", NULL, NULL },
		{ "Kick From Veh", NULL, NULL },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexOnline2)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexOnline2].text, lines[activeLineIndexOnline2].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexOnline2 * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			switch (activeLineIndexOnline2)
			{
			case 5:
				for (int index = 0; index < 32; index++)
				{
					Player playerOnline = PLAYER::GET_PLAYER_PED(index);
					if (PLAYER::GET_PLAYER_PED(index) == PLAYER::PLAYER_PED_ID()) continue;
					{
						WEAPON::REMOVE_ALL_PED_WEAPONS(PLAYER::GET_PLAYER_PED(index), TRUE);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~warning everyone may leave this server.";
					set_status_text(playerName + text);
				}
				break;
			case 6:
				for (int index = 0; index < 32; index++)
				{
					Player playerOnline = PLAYER::GET_PLAYER_PED(index);
					if (PLAYER::GET_PLAYER_PED(index) == PLAYER::PLAYER_PED_ID()) continue;
					{
						Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
						if (ENTITY::DOES_ENTITY_EXIST(playerOnline))
							FIRE::ADD_OWNED_EXPLOSION(playerOnline, playerOnlineCoords.x, playerOnlineCoords.y, playerOnlineCoords.z, 37, 5, FALSE, TRUE, 0);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~warning everyone may leave this server.";
					set_status_text(playerName + text);
				}
				break;
			case 7:
				for (int index = 0; index < 32; index++)
				{
					Player playerOnline = PLAYER::GET_PLAYER_PED(index);
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					if (ENTITY::DOES_ENTITY_EXIST(playerOnline) && PED::IS_PED_IN_ANY_VEHICLE(playerOnline, 0))
					{
						requestControl(currentOnlineVeh);
						VEHICLE::SET_VEHICLE_ENGINE_HEALTH(currentOnlineVeh, -1);
						VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(currentOnlineVeh, -1);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~warning everyone may leave this server.";
					set_status_text(playerName + text);
				}
				break;
			case 8:
				for (int index = 0; index < 32; index++)
				{
					Player playerOnline = PLAYER::GET_PLAYER_PED(index);
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					if (ENTITY::DOES_ENTITY_EXIST(playerOnline) && PED::IS_PED_IN_FLYING_VEHICLE(playerOnline))
					{
						requestControl(currentOnlineVeh);
						VEHICLE::SET_VEHICLE_DOORS_LOCKED(currentOnlineVeh, 4);
						VEHICLE::SET_VEHICLE_ENGINE_HEALTH(currentOnlineVeh, -1);
						VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(currentOnlineVeh, -1);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~warning everyone may leave this server.";
					set_status_text(playerName + text);
				}
				break;
			case 9:
				for (int index = 0; index < 32; index++)
				{
					Player playerOnline = PLAYER::GET_PLAYER_PED(index);
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					if (ENTITY::DOES_ENTITY_EXIST(playerOnline) && PED::IS_PED_IN_ANY_VEHICLE(playerOnline, 0))
					{
						requestControl(currentOnlineVeh);
						VEHICLE::SET_VEHICLE_DOORS_LOCKED(currentOnlineVeh, 4);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~warning everyone may leave this server.";
					set_status_text(playerName + text);
				}
				break;
			case 10:
				for (int index = 0; index < 32; index++)
				{
					Player playerOnline = PLAYER::GET_PLAYER_PED(index);
					if (ENTITY::DOES_ENTITY_EXIST(playerOnline))
					{
						freezePlayer(playerOnline);
					}
					std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
					std::string text = " ~r~warning everyone may leave this server.";
					set_status_text(playerName + text);
				}
				break;
				// switchable features
			default:
				if (lines[activeLineIndexOnline2].pState)
					*lines[activeLineIndexOnline2].pState = !(*lines[activeLineIndexOnline2].pState);
				if (lines[activeLineIndexOnline2].pUpdated)
					*lines[activeLineIndexOnline2].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexOnline2 == 0)
						activeLineIndexOnline2 = lineCount;
					activeLineIndexOnline2--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexOnline2++;
						if (activeLineIndexOnline2 == lineCount)
							activeLineIndexOnline2 = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexOnline1 = 0;
void process_online_menu1(int index)
{
	const float lineWidth = 200.0;
	const int lineCount = 24;

	std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));

	char* me = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
	char* name = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
	int health = ENTITY::GET_ENTITY_HEALTH(PLAYER::GET_PLAYER_PED(index));
	int armor = PED::GET_PED_ARMOUR(PLAYER::GET_PLAYER_PED(index));
	float distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), FALSE).x, ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), FALSE).y, ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), FALSE).z, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(index), FALSE).x, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(index), FALSE).y, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(index), FALSE).z, TRUE);
	map_player_status("~r~Username: ~s~%s\n~g~Health: ~s~%d\n~b~Armor: ~s~%d\n~y~Distance: ~s~%.02f from ~g~%s", name, health, armor, distance, me);

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "Clone", NULL, NULL },
		{ "Attach Ufo", NULL, NULL },
		{ "Cash", NULL, NULL },
		{ "Kill", NULL, NULL },
		{ "Upgrade", NULL, NULL },
		{ "Burst Tires", NULL, NULL },
		{ "Teleport To", NULL, NULL },
		{ "Teleport To Me", NULL, NULL },
		{ "Teleport To Veh", NULL, NULL },
		{ "Smash Windows", NULL, NULL },
		{ "Frame", NULL, NULL },
		{ "Clone Veh", NULL, NULL },
		{ "Launch Veh Up", NULL, NULL },
		{ "Launch Veh Forward", NULL, NULL },
		{ "Launch Veh Back", NULL, NULL },
		{ "Freeze", NULL, NULL },
		{ "Face Fuck", NULL, NULL },
		{ "Crash", NULL, NULL },
		{ "Three Stars", NULL, NULL },
		{ "Attach Veh", NULL, NULL },
		{ "Bodyguard", NULL, NULL },
		{ "Status", NULL, NULL },
		{ "AI Car Jack", NULL, NULL },
		{ "Air Strike", NULL, NULL },
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexOnline1)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, -0.5f, 60.0f + i * 20.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexOnline1].text, lines[activeLineIndexOnline1].pState),
				lineWidth + 0.0f, -0.5f, 60.0f + activeLineIndexOnline1 * 20.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;
		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			// common variables
			Player player = PLAYER::PLAYER_ID();
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			Ped playerGroup = PED::GET_PED_GROUP_INDEX(playerPed);
			BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(playerPed);
			Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(playerPed, FALSE);
			Vehicle currentVeh = PED::GET_VEHICLE_PED_IS_USING(playerPed);

			// common online variables
			Player playerOnline = PLAYER::GET_PLAYER_PED(index);
			BOOL bPlayerOnlineExists = ENTITY::DOES_ENTITY_EXIST(playerOnline);
			//Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
			//Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);

			menu_beep_select();
			switch (activeLineIndexOnline1)
			{
			case 0:
				if (bPlayerOnlineExists)
				{
					Ped clone = PED::CLONE_PED(playerOnline, ENTITY::GET_ENTITY_HEADING(playerPed), 1, 1);
					WEAPON::GIVE_DELAYED_WEAPON_TO_PED(clone, GAMEPLAY::GET_HASH_KEY("WEAPON_STUNGUN"), 1000, 1);
					AI::TASK_SHOOT_AT_ENTITY(clone, playerOnline, 999999, GAMEPLAY::GET_HASH_KEY("FIRING_PATTERN_FULL_AUTO"));
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~g~was cloned.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 1:
				if (bPlayerOnlineExists)
				{
					Hash model = GAMEPLAY::GET_HASH_KEY("p_spinning_anus_s");
					STREAMING::REQUEST_MODEL(model);
					while (!STREAMING::HAS_MODEL_LOADED(model))
						WAIT(0);
					Object object = OBJECT::CREATE_OBJECT(model, 0, 0, 0, TRUE, TRUE, FALSE);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(object, playerOnline, PED::GET_PED_BONE_INDEX(playerOnline, 0x796e), 0, 0, 0, 0, 90, 0, FALSE, FALSE, FALSE, FALSE, 2, TRUE);
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
					std::string message = "~y~Make sure  to type the hash properly.";
					set_status_text(message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 2:
				if (bPlayerOnlineExists)
				{
					DWORD hash = GAMEPLAY::GET_HASH_KEY((char*)"a_c_rat");
					if (STREAMING::IS_MODEL_IN_CDIMAGE(hash) && STREAMING::IS_MODEL_VALID(hash))
					{
						STREAMING::REQUEST_MODEL(hash);
						while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(0);
						Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
						Ped rat = PED::CREATE_PED(1, hash, playerOnlineCoords.x, playerOnlineCoords.y, playerOnlineCoords.z, ENTITY::GET_ENTITY_HEADING(playerOnline), TRUE, TRUE);
						PED::SET_PED_MONEY(rat, rand() % 2000);
						WAIT(0);
						ENTITY::SET_ENTITY_HEALTH(rat, 0);
						WAIT(0);
						PED::DELETE_PED(&rat);
						WAIT(0);
						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
						std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
						std::string message = " ~g~received cash.";
						set_status_text(caption + message);
					}
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 3:
				if (bPlayerOnlineExists)
				{
					Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
					FIRE::ADD_OWNED_EXPLOSION(playerOnline, playerOnlineCoords.x, playerOnlineCoords.y, playerOnlineCoords.z, 37, 5, TRUE, FALSE, 1);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~g~was exploded.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 4:
				if (PED::IS_PED_IN_ANY_VEHICLE(playerOnline, 0))
				{
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					upgrade(currentOnlineVeh, playerOnline);
					VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(currentOnlineVeh, 5);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~g~vehicle was upgraded.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 5:
				if (PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					requestControl(currentOnlineVeh);
					if (VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(currentOnlineVeh))
					{
						VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(currentOnlineVeh, TRUE);
					}
					static int tireID = 0;
					for (tireID = 0; tireID < 8; tireID++)
					{
						requestControl(currentOnlineVeh);
						VEHICLE::SET_VEHICLE_TYRE_BURST(currentOnlineVeh, tireID, TRUE, 1000);
					}
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~y~might have bulletproof tires.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 6:
				if (bPlayerOnlineExists)
				{
					Entity e = playerPed;
					if (currentVeh != NULL)
						e = currentVeh;
					Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, playerOnlineCoords.x, playerOnlineCoords.y, playerOnlineCoords.z + 1, FALSE, FALSE, TRUE);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 7:
				if (ENTITY::DOES_ENTITY_EXIST(playerOnline))
				{
					if (PED::IS_PED_IN_ANY_VEHICLE(playerOnline, 0))
					{
						Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
						requestControl(currentOnlineVeh);
						Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
						ENTITY::SET_ENTITY_COORDS(currentOnlineVeh, playerOnlineCoords.x, playerOnlineCoords.y, playerOnlineCoords.z, 0, 0, 0, 1);
					}
					else
					{
						set_status_text("~y~This function only works if the player is in a vehicle.");
					}
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 8:
				if (PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					int numSeats = VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(currentOnlineVeh);
					for (int i = numSeats; i >= -1; i--)
					{
						PED::SET_PED_INTO_VEHICLE(playerPed, currentOnlineVeh, i);
					}
				}
				else if (PED::IS_PED_ON_FOOT(playerOnline))
				{
					Vector3 playerOnlineCoords = ENTITY::GET_ENTITY_COORDS(playerOnline, FALSE);
					if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
					{
						requestControl(PED::GET_VEHICLE_PED_IS_USING(playerPed));
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(PED::GET_VEHICLE_PED_IS_IN(playerPed, 0), playerOnlineCoords.x, playerOnlineCoords.y + 5.0f, playerOnlineCoords.z, 0, 0, 1);
					}
					else
					{
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, playerOnlineCoords.x, playerOnlineCoords.y + 5.0f, playerOnlineCoords.z, 0, 0, 1);
					}
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 9:
				if (bPlayerOnlineExists && PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					requestControl(currentOnlineVeh);
					VEHICLE::SMASH_VEHICLE_WINDOW(currentOnlineVeh, 1);
					VEHICLE::SMASH_VEHICLE_WINDOW(currentOnlineVeh, 0);
					VEHICLE::SMASH_VEHICLE_WINDOW(currentOnlineVeh, 3);
					VEHICLE::SMASH_VEHICLE_WINDOW(currentOnlineVeh, 2);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~g~windows were smashed out.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 10:
				if (bPlayerOnlineExists)
				{
					char* name = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					framePlayer(playerOnline, index, name);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 11:
				if (bPlayerOnlineExists && PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle clonedVeh = clonePedVehicle(playerOnline);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~g~vehcile was cloned for you.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 12:
				if (bPlayerOnlineExists && PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					requestControl(currentOnlineVeh);
					applyForceToEntity(currentOnlineVeh, 0.0f, 0.0f, 200.0f);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~g~vehicle was launched.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 13:
				if (bPlayerOnlineExists && PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					requestControl(currentOnlineVeh);
					applyForceToEntity(currentOnlineVeh, 0.0f, 50.0f, 0.0f);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~g~vehicle was launched forwards.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 14:
				if (bPlayerOnlineExists && PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle currentOnlineVeh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					requestControl(currentOnlineVeh);
					applyForceToEntity(currentOnlineVeh, 0.0f, -50.0f, 0.0f);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~g~vehicle was launched backwards.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 15:
				if (bPlayerOnlineExists)
				{
					freezePlayer(playerOnline);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "~g~Continuously press freeze to really annoy ~s~";
					set_status_text(message + caption);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 16:
				if (bPlayerOnlineExists)
				{
					ENTITY::ATTACH_ENTITY_TO_ENTITY(playerPed, playerOnline, PED::GET_PED_BONE_INDEX(playerOnline, 0x796e), 0.0f, 0.0f, 0.0f, 90.0f, 90.0f, 90.0f, FALSE, FALSE, FALSE, FALSE, 2, TRUE);
					STREAMING::REQUEST_ANIM_SET((char*)"shag_loop_a");
					STREAMING::REQUEST_ANIM_DICT((char*)"rcmpaparazzo_2");
					while (!STREAMING::HAS_ANIM_DICT_LOADED((char*)"rcmpaparazzo_2") && !STREAMING::HAS_ANIM_DICT_LOADED((char*)"rcmpaparazzo_2")) WAIT(0);
					{
						AI::TASK_PLAY_ANIM(playerPed, (char*)"rcmpaparazzo_2", (char*)"shag_loop_a", 1.0f, 1.0, -1, 47, 0.0f, FALSE, FALSE, FALSE);
					}
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "~g~How does that taste ~s~";
					std::string text = "?";
					set_status_text(message + caption + text);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 17:
				if (bPlayerOnlineExists)
				{
					crashPlayer(playerOnline, playerPed);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 18:
				if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED(index)))
				{
					const int ped_amount = 50;
					Ped ped_clone[ped_amount];
					for (int i = 0; i < ped_amount; i++)
					{
						ped_clone[i] = PED::CLONE_PED(PLAYER::GET_PLAYER_PED(index), ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()), 1, 1);
						WAIT(0);
						ENTITY::SET_ENTITY_COORDS(ped_clone[i], ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(index), 0).x, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(index), 0).y, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(index), 0).z + 25.0f, 0, 0, 0, 1);
						WAIT(0);
						PED::SET_PED_AS_COP(ped_clone[i], TRUE);
						WAIT(0);
						FIRE::ADD_OWNED_EXPLOSION(PLAYER::GET_PLAYER_PED(index), ENTITY::GET_ENTITY_COORDS(ped_clone[i], 0).x, ENTITY::GET_ENTITY_COORDS(ped_clone[i], 0).y, ENTITY::GET_ENTITY_COORDS(ped_clone[i], 0).z, 37, 10.0f, 0, 1, 0);
						WAIT(0);
						PED::DELETE_PED(&ped_clone[i]);
					}
				}
				break;
			case 19:
				if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED(index)))
				{
					const int _element = 10;
					const int _array = _element * 2 + 2;
					int get_vehicle_ped_is_using[_array];
					get_vehicle_ped_is_using[0] = _element;
					int _count = PED::GET_PED_NEARBY_VEHICLES(PLAYER::GET_PLAYER_PED(index), get_vehicle_ped_is_using);
					for (int i = 0; i < _count; ++i)
					{
						int offsettedID = i * 2 + 2;
						if (ENTITY::DOES_ENTITY_EXIST(get_vehicle_ped_is_using[offsettedID]))
						{
							if ((get_vehicle_ped_is_using[offsettedID]) == PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED(index), 0)) continue;
							{
								WAIT(0);
								ENTITY::ATTACH_ENTITY_TO_ENTITY(get_vehicle_ped_is_using[offsettedID], PLAYER::GET_PLAYER_PED(index), PED::GET_PED_BONE_INDEX(PLAYER::GET_PLAYER_PED(index), 0xe0fd), 0.0, 0.0, 0.0, 0.0, 90.0, 0, FALSE, FALSE, FALSE, FALSE, 2, TRUE);
							}
						}
					}
				}
				break;
			case 20:
				bodyguard(playerOnline, PLAYER::INT_TO_PLAYERINDEX(index));
				break;
			case 21:
				map_player_status("~r~Username: ~s~%s\n~g~Health: ~s~%d\n~b~Armor: ~s~%d\n~y~Distance: ~s~%.02f from ~g~%s", name, health, armor, distance, me);
				break;
			case 22:
				if (PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerOnline);
					AI::CLEAR_PED_TASKS_IMMEDIATELY(playerOnline);
					WAIT(0);
					Ped Driver = PED::CLONE_PED(playerOnline, 0.0f, TRUE, TRUE);
					WAIT(0);
					PED::SET_PED_INTO_VEHICLE(Driver, veh, -1);
					PED::SET_PED_DIES_WHEN_INJURED(Driver, FALSE);
					AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(Driver, TRUE);
					WAIT(0);
					AI::TASK_VEHICLE_DRIVE_TO_COORD(Driver, veh, -2000.0f, -1000.0f, 0.0f, 100.0f, 1, ENTITY::GET_ENTITY_MODEL(veh), 0, 0xC00AB, -1);
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~g~ride was jacked.";
					set_status_text(caption + message);
				}
				else if (!PED::IS_PED_IN_ANY_VEHICLE(playerOnline, FALSE))
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = "'s ~r~vehicle does not exist.";
					set_status_text(caption + message);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
			case 23:
				if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED(index)))
				{
					int weaponHash = GAMEPLAY::GET_HASH_KEY("WEAPON_VEHICLE_ROCKET");
					Vector3 getcoords1 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerOnline, 0.0f, 0.0f, 26.6f);
					Vector3 getcoords2 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerOnline, 0.0f, 0.0f, 1.5f);
					GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(getcoords1.x, getcoords1.y, getcoords1.z, getcoords2.x, getcoords2.y, getcoords2.z, 250, 1, weaponHash, player, 1, 0, -1.0);
				}
				else
				{
					std::string caption = PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(index));
					std::string message = " ~r~does not exist.";
					set_status_text(caption + message);
				}
				break;
				// switchable features
			default:
				if (lines[activeLineIndexOnline1].pState)
					*lines[activeLineIndexOnline1].pState = !(*lines[activeLineIndexOnline1].pState);
				if (lines[activeLineIndexOnline1].pUpdated)
					*lines[activeLineIndexOnline1].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexOnline1 == 0)
						activeLineIndexOnline1 = lineCount;
					activeLineIndexOnline1--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexOnline1++;
						if (activeLineIndexOnline1 == lineCount)
							activeLineIndexOnline1 = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexOnline0 = 0;
void process_online_menu0()
{
	const float lineWidth = 200.0;
	const int lineCount = 32;

	std::string caption = "~r~Player ~w~Selection";

	typedef struct {
		std::string		name;
		Ped				ped;
		int			playerInt;
	} lines;

	lines onlinePlayers[32];

	for (int i = 0; i < lineCount; i++)
		onlinePlayers[i] = { PLAYER::GET_PLAYER_NAME(PLAYER::INT_TO_PLAYERINDEX(i)), PLAYER::GET_PLAYER_PED(i), i };

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexOnline0)
					draw_menu_line(onlinePlayers[i].name,
						lineWidth, -0.5f, 60.0f + i * 20.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(onlinePlayers[activeLineIndexOnline0].name,
				lineWidth + 0.0f, -0.5f, 60.0f + activeLineIndexOnline0 * 20.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			if (onlinePlayers[activeLineIndexOnline0].name.compare("**Invalid**") != 0)
				process_online_menu1(onlinePlayers[activeLineIndexOnline0].playerInt);
			else
			{
				std::string playerName = PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
				std::string text = " ~r~that was invalid entry.";
				set_status_text(playerName + text);
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexOnline0 == 0)
						activeLineIndexOnline0 = lineCount;
					activeLineIndexOnline0--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexOnline0++;
						if (activeLineIndexOnline0 == lineCount)
							activeLineIndexOnline0 = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexOnline = 0;
void process_online_menu()
{
	const float lineWidth = 200.0;
	const int lineCount = 2;

	std::string caption = "~r~Online ~w~Options";

	static struct {
		LPCSTR		text;
		bool* pState;
		bool* pUpdated;
	} lines[lineCount] = {
		{ "Select Player",	NULL,	NULL },
		{ "Lobby",	NULL,	NULL }
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexOnline)
					draw_menu_line(line_as_str(lines[i].text, lines[i].pState),
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(line_as_str(lines[activeLineIndexOnline].text, lines[activeLineIndexOnline].pState),
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexOnline * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			switch (activeLineIndexOnline)
			{
			case 0:
				process_online_menu0();
				break;
			case 1:
				process_online_menu2();
				break;
				// switchable features
			default:
				if (lines[activeLineIndexOnline].pState)
					*lines[activeLineIndexOnline].pState = !(*lines[activeLineIndexOnline].pState);
				if (lines[activeLineIndexOnline].pUpdated)
					*lines[activeLineIndexOnline].pUpdated = TRUE;
			}
			waitTime = 200;
		}
		else
			if (bBack || trainer_switch_pressed())
			{
				menu_beep_cancel();
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexOnline == 0)
						activeLineIndexOnline = lineCount;
					activeLineIndexOnline--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexOnline++;
						if (activeLineIndexOnline == lineCount)
							activeLineIndexOnline = 0;
						waitTime = 200;
					}
	}
}

int activeLineIndexMain = 0;
void process_main_menu()
{
	set_creator_text("~h~~g~iTzNeutron initialize successfully.~n~~b~https://www.[explicit].menu/");
	menuVisible = TRUE;

	const float lineWidth = 200.0;
	const int lineCount = 10;

	std::string caption = "~r~iTzNeutron ~c~Version ~g~420";

	static LPCSTR lineCaption[lineCount] = {
		"Player Options",
		"Teleport Options",
		"Weapon Options",
		"Vehicle Options",
		"World Options",
		"Misc Options",
		"Animation Options",
		"Online Options",
		"Reset Menu",
		"Credits"
	};

	DWORD waitTime = 200;
	while (TRUE)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 6.0, 18.0, 850.0, 5.0, FALSE, TRUE);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexMain)
					draw_menu_line(lineCaption[i],
						lineWidth, 2.0f, 60.0f + i * 25.0f, 875.0f, 9.0f, FALSE, FALSE);
			draw_menu_line(lineCaption[activeLineIndexMain],
				lineWidth + 0.0f, 2.0f, 60.0f + activeLineIndexMain * 25.0f, 875.0f, 7.0f, TRUE, FALSE);

			update_features();
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep_select();
			switch (activeLineIndexMain)
			{
			case 0:
				process_player_menu();
				break;
			case 1:
				process_teleport_menu();
				break;
			case 2:
				process_weapon_menu();
				break;
			case 3:
				process_vehicle_menu();
				break;
			case 4:
				process_world_menu();
				break;
			case 5:
				process_miscellaneous_menu();
				break;
			case 6:
				process_animation_menu();
				break;
			case 7:
				process_online_menu();
				break;
			case 8:
				featurePlayerESP =
					featurePlayerESPBoxes =
					featurePlayerESPTrace =
					featurePlayerInvincible =
					featurePlayerInvincibleUpdated =
					featurePlayerNeverWanted =
					featurePlayerIgnored =
					featurePlayerIgnoredUpdated =
					featurePlayerUnlimitedAbility =
					featurePlayerNoNoise =
					featurePlayerNoNoiseUpdated =
					featurePlayerFastSwim =
					featurePlayerFastSwimUpdated =
					featurePlayerFastRun =
					featurePlayerFastRunUpdated =
					featurePlayerSuperJump =
					playerInvisible =
					playerInvisibleUpdated =
					cashAdd =
					featureNightVision =
					featureNightVisionUpdated =
					featureThermalVision =
					featureThermalVisionUpdated =
					featurePlayerUltraJump =
					featurePlayerSuperRun =
					featurePlayerBloodDamage =
					featurePlayerAutoHeal =
					featurePlayerHancock =
					featureAirBrake =
					featureWeaponTriggerbot =
					featureWeaponNoReload =
					featureWeaponNoReloadUpdated =
					featureWeaponFireAmmo =
					featureWeaponExplosiveAmmo =
					featureWeaponExplosiveMelee =
					featureWeaponFirework =
					featureWeaponVehicleRocket =
					featureWeaponTank =
					featureWeaponRailGun =
					featureWeaponHeavySniper =
					featureWeaponMinigun =
					featureWeaponPlaneRocket =
					featureWeaponGrenadeLauncher =
					featureWeaponAirstrikeRocket =
					featureWeaponSavage =
					featureWeaponSpaceRocket =
					featureWeaponPropMoneyBag =
					featureWeaponPropCokeBlock =
					featureWeaponPropWeed =
					featureWeaponPropWeedPallet =
					featureWeaponPropXmasTree =
					featureWeaponPropXmas =
					featureWeaponPropDildo =
					weaponOnePunchKill =
					weaponOnePunchKillUpdated =
					featureWeaponDamageModifier =
					featureWeaponDamageModifierUpdate =
					weaponForceGun =
					weaponGravityGun =
					gravityLock =
					weaponSpaghettios =
					weaponDeleteObjects =
					featureVehInvincible =
					featureVehInvincibleUpdated =
					featureVehInvisible =
					featureVehInvisibleUpdated =
					featureVehInvincibleWheels =
					featureVehInvincibleWheelsUpdated =
					featureVehSeatbelt =
					featureVehSeatbeltUpdated =
					featureVehSpeedBoost =
					featureVehWrapInSpawned =
					featureVehRainbow =
					featureVehDrift =
					featureVehGrip =
					featureVehSlammed =
					featureVehSlammedUpdated =
					featureVehAutoFix =
					featureVehDamageModifier =
					featureVehDamageModifierUpdate =
					featureWorldMoonGravity =
					worldClearVehicles =
					worldClearObjects =
					worldClearPeds =
					worldClearCops =
					worldClearProjectiles =
					featureMiscGlobeRadio =
					featureMiscGlobeRadioUpdated =
					featureMiscLockRadio =
					featureMiscHideHud =
					featureMiscRestrictedZones =
					featurePedestrianInvincibleBodyguard =
					featurePedestrianInvincibleBodyguardUpdate =
					featureNearbyPeds =
					featureSetPedCalm0 =
					featureSetPedCalm1 =
					featureKillPed =
					featureRichPed =
					featureRevivePed =
					featureEspPed = 0;
				playerWasDisconnected = 1;
				featurePlayerBlips =
					featurePlayerHeadDisplay =
					featurePlayerBlipCone =
					featurePlayerNotifications =
					featureShowVoiceChatSpeaker =
					featureSkinChanger = 0;
				set_creator_text("~h~~g~iTzNeutron reset successfully. ~n~~y~Menu reset hotkey F12.");
				break;
			case 9:
				WAIT(50);
				set_status_text("~h~iTzNeutron, Gaz'lam, Alexander Blade, gir489, NTAuthority / citizenMP, s0biet, FreeOffset, Mafins, CryptXor");
				WAIT(100);
				set_status_text("Some Functions are from ~h~~r~FiveM, Enhanced Native Trainer, m0d s0beit v Redux, and Menya Baes.");
				WAIT(150);
				set_status_text("Original project ~h~~r~Native Trainer (AB)");
				WAIT(200);
				set_status_text("Current project name ~h~~r~iTzNeutron");
				break;
			}
			waitTime = 200;
		}
		else
			if (bBack && menuVisible || trainer_switch_pressed() && menuVisible)
			{
				menu_beep_cancel();
				menuVisible = FALSE;
				break;
			}
			else
				if (bUp)
				{
					menu_beep_up_down();
					if (activeLineIndexMain == 0)
						activeLineIndexMain = lineCount;
					activeLineIndexMain--;
					waitTime = 200;
				}
				else
					if (bDown)
					{
						menu_beep_up_down();
						activeLineIndexMain++;
						if (activeLineIndexMain == lineCount)
							activeLineIndexMain = 0;
						waitTime = 200;
					}
	}
}

//#pragma region hooking
//eThreadState Trampoline(GtaThread* This)
//{
//	rage::scrThread* runningThread = GetActiveThread();
//	SetActiveThread(This);
//	#ifdef  __DEBUG
//	Run();
//	#else
//	Tick();
//	#endif
//	SetActiveThread(runningThread);
//	return gGtaThreadOriginal.Run(This);
//}

//void __stdcall ReliableScriptFunction(LPVOID lpParameter)
//{
//	try
//	{
//		while (1)
//		{
//			Run();
//			SwitchToFiber(mainFiber);
//		}
//	}
//	catch (...) {
//	}
//}
//
//void __stdcall HeavyWaitFunction(LPVOID lpParameter)
//{
//	try
//	{
//		while (1)
//		{
//			RunUnreliable();
//			SwitchToFiber(mainFiber);
//		}
//	}
//	catch (...) {
//	}
//}
//
//void Tick()
//{
//	if (mainFiber == nullptr)
//		mainFiber = ConvertThreadToFiber(nullptr);
//
//	static HANDLE reliableFiber;
//	if (reliableFiber)
//		SwitchToFiber(reliableFiber);
//	else
//		reliableFiber = CreateFiber(NULL, ReliableScriptFunction, nullptr);
//
//	if (timeGetTime() < wakeAt)
//		return;
//
//	static HANDLE scriptFiber;
//	if (scriptFiber)
//		SwitchToFiber(scriptFiber);
//	else
//		scriptFiber = CreateFiber(NULL, HeavyWaitFunction, nullptr);
//}

void RunUnreliable()
{
	while (true)
	{
		if (trainer_switch_pressed())
		{

			menu_beep_cancel();
			//set_menu_showing(true);
			process_main_menu();
			///set_menu_showing(false);
		}
		//else if (airbrake_switch_pressed())
		//{
		//	menu_beep();
		//	process_airbrake_menu();
		//}
		update_features();
		WAIT(0);
	}
}

void TickThis()
{
	if (IsKeyPressed(VK_F4))
	{
		process_main_menu();
	}
}

void Run()
{
	update_features();
}

//bool AttemptScriptHook()
//{
//	rage::pgPtrCollection<GtaThread>* threadCollection = GetGtaThreadCollection(pBlipList);
//
//	if (!threadCollection) {
//		return FALSE;
//	}
//
//	for (UINT16 i = 0; i < threadCollection->count(); i++) {
//		GtaThread* pThread = threadCollection->at(i);
//
//		if (!pThread)
//			continue;
//
//		if (pThread->GetContext()->ScriptHash != 0x5700179C) {
//			continue;
//		}
//
//		if (gGtaThreadOriginal.Deconstructor == NULL) {
//			memcpy(&gGtaThreadOriginal, (DWORD64*)((DWORD64*)pThread)[0], sizeof(gGtaThreadOriginal)); 
//			memcpy(&gGtaThreadNew, &gGtaThreadOriginal, sizeof(GtaThread_VTable));
//
//			gGtaThreadNew.Run = Trampoline;
//		}
//
//		if (((DWORD64*)pThread)[0] != (DWORD64)&gGtaThreadNew) {
//			((DWORD64*)pThread)[0] = (DWORD64)&gGtaThreadNew;
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//
//DWORD WINAPI lpHookScript(LPVOID lpParam) {
//	while (!AttemptScriptHook()) {
//		Sleep(100);
//	}
//
//	return 0;
//}

//void SpawnScriptHook() {
//	CreateThread(0, 0, lpHookScript, 0, 0, 0);
//}
//#pragma endregion