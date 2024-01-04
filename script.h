#pragma once

extern void(*periodic_feature_call)(void);

/**Set the method that is used to periodically update the entire UI and apply repeat settings. The script core does this once.*/
void set_periodic_feature_call(void method(void));

/**Make the periodic feature call. For example it updates the status text etc. - look at existing menus to see where this is done.*/
void make_periodic_feature_call();

#define IsKeyPressed(key) GetAsyncKeyState(key) & 0x8000

bool isKeyPressedOnce(bool& bIsPressed, DWORD vk);
void ReleaseKeys();

void BoostBaseVehicleStats(Vehicle vehicle);
void DumpPlayerFromVehicle(Ped PlayerID);
void SpawnPlayerVehicle(Ped playerPed, Hash vehicleHash, bool spawnInVehicle, bool baseUpgrade, bool maxUpgrade);
void SetPlateType(Vehicle vehicle, int plateindex);
void SetWindowTint(Vehicle vehicle, int tint);
void ResprayVehicle(Vehicle vehicle, int primary, int secondary, int pearl, int wheel);
void ToggleExtra(Vehicle vehicle, int extraID);
void ClaimVehicle(Vehicle vehicle);
void SetNumberPlate(Vehicle playerVeh, char* text);
void RepairVehicle(Vehicle vehicle);
void SetVehicleModded(Vehicle vehicle);
bool DoesVehicleExist(char* name);
void SetVehicleMod(Vehicle vehicle, int modType, int modIndex);
void ToggleVehicleMod(Vehicle vehicle, int modType);
void SetVehicleLivery(Vehicle vehicle, int livery);
void SetTireSmoke(Vehicle vehicle, int r, int g, int b);
void ToggleNeonLight(Vehicle vehicle, int index);
void MaxUpgrade(Vehicle vehicle);
void TickThis();
void RunUnreliable();
template<typename T>
bool draw_generic_menu(std::vector<std::string> captions, std::vector<T> values, int currentSelectionIndex, std::string headerText,
	bool(*onConfirmation)(int selectedIndex, std::string caption, T value),
	void(*onHighlight)(int selectedIndex, std::string caption, T value),
	void(*onExit)(bool returnValue))
{
	bool result = false;
	DWORD waitTime = 150;
	const int totalItems = (int)captions.size();
	const int itemsPerLine = 10;
	const int lineCount = (int)(ceil((double)totalItems / (double)itemsPerLine));

	if (onHighlight != NULL)
	{
		onHighlight(currentSelectionIndex, captions[currentSelectionIndex], values[currentSelectionIndex]);
	}

	while (true)
	{
		double currentLine = floor((int)currentSelectionIndex / (int)itemsPerLine);

		const int originalIndex = currentSelectionIndex;

		int positionOnThisLine = currentSelectionIndex % itemsPerLine;
		int lineStartPosition = currentSelectionIndex - positionOnThisLine;
		int itemsOnThisLine = (lineStartPosition + itemsPerLine > totalItems) ? (totalItems - lineStartPosition) : itemsPerLine;

		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			// draw menu
			std::ostringstream ss;
			ss << headerText;
			if (lineCount > 1)
			{
				ss << " " << (currentLine + 1) << " / " << lineCount;
			}

			draw_menu_header_line(ss.str(),/*350.0*/280.0f,//line W
				/*50*//*24*/70.0f,//line H
				59.0f,//	10.0f,//line T
				208.0f,//line L
				/*15*//*208.0f*/233.0f,//text X offset
				false, true, (currentLine + 1),
				lineCount
			);
			for (int i = 0; i < itemsOnThisLine; i++)
			{
				std::string thisItem = captions[lineStartPosition + i];

				/*
				//old horizontals
				float lineWidth = 100.0f;
				float lineHeight = 5.0f;
				float lineTop = 200.0;
				float lineLeft = 100.0f + i * 110.0f;
				*/
				float lineSpacingY = 0.0f;//8.0f;

				float lineWidth = 280.0f;
				float lineHeight = /*31*//*20.0f;*/24.0f;

				float lineTop = /*14*//*79.0f*/129.0f + (i * (lineHeight + lineSpacingY));// y-axis items
				float lineLeft = 208.000000f;
				float textOffset = /*10*/8.0f;
				draw_menu_line(thisItem, lineWidth, lineHeight, lineTop, lineLeft, textOffset, i == positionOnThisLine, false, false);
			}

			if (periodic_feature_call != NULL)
			{
				periodic_feature_call();
			}

			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		bool bSelect, bBack, bUp, bDown, bLeft, bRight;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, &bLeft, &bRight);

		if (bSelect)
		{
			//			menu_beep();
			if (onConfirmation != NULL)
			{
				result = onConfirmation(currentSelectionIndex, captions[currentSelectionIndex], values[currentSelectionIndex]);
			}
			waitTime = 200;
			if (result)
			{
				result = false; //to avoid cascading upwards
				break;
			}
		}
		else
		{
			if (bBack)
			{
				//				menu_beep();
				waitTime = 200;
				result = false;
				break;
			}
			else
			{
				if (bDown)
				{
					//					menu_beep();
					currentSelectionIndex++;
					if (currentSelectionIndex >= totalItems || (currentSelectionIndex >= lineStartPosition + itemsOnThisLine))
					{
						currentSelectionIndex = lineStartPosition;
					}
					waitTime = 100;
				}
				else
					if (bUp)
					{
						//						menu_beep();
						currentSelectionIndex--;
						if (currentSelectionIndex < 0 || (currentSelectionIndex < lineStartPosition))
						{
							currentSelectionIndex = lineStartPosition + itemsOnThisLine - 1;
						}
						waitTime = 100;
					}
					else
						if (bLeft)
						{
							//							menu_beep();
							currentSelectionIndex -= itemsPerLine;
							if (currentSelectionIndex < 0)
								currentSelectionIndex = totalItems - 1;
							waitTime = 200;
						}
						else
							if (bRight)
							{
								//								menu_beep();
								currentSelectionIndex += itemsPerLine;
								if (currentSelectionIndex >= totalItems)
									currentSelectionIndex = 0;
								waitTime = 200;
							}

				if (onHighlight != NULL && originalIndex != currentSelectionIndex)
				{
					onHighlight(currentSelectionIndex, captions[currentSelectionIndex], values[currentSelectionIndex]);
				}
			}
		}
	}

	if (onExit != NULL)
	{
		onExit(result);
	}

	// wait before exit
	if (waitTime > 0)
	{
		DWORD maxTickCount = GetTickCount() + waitTime;
		do
		{
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;
	}

	return result;
}

//void update_vehicle_features(BOOL playerExists, Ped playerPed);

bool process_spawn_menu_cars();

bool process_spawn_menu_indus();

bool process_spawn_menu_generic(int topMenuSelection);

bool onconfirm_spawn_menu_vehicle_selection(int selection, std::string caption, std::string value);

bool do_spawn_vehicle(std::string modelName, std::string modelTitle);

int findFirstValidPedTexture(int drawable);

int findFirstValidPedDrawable(int component);

bool applyChosenSkin(std::string skinName);

void reset_skin_globals();

bool is_custom_skin_applied();

void set_custom_skin_applied(bool applied);

bool process_skinchanger_menu();

void load_north_yankton();

void unload_north_yankton();

bool process_teleport_menu2();

void reset_teleporter_globals();