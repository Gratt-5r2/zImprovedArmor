// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	static string SlotPrefix = "ZS_WEAR_";
	static string SoftSkinWearSeparator = "  ";


	static string CreateMessage_CantEquipArmorPart( oCItem* equiped, oCItem* notEquiped ) {
		char* msg[Lang_Max] = {
			"Item '%z' is incompatible with '%z'!\n",
			"Элемент '%z' несовместим с '%z'!\n",
			"Item '%z' is incompatible with '%z'!\n",
			"Item '%z' ist nicht kompatibel mit '%z'!\n",
			"Element '%z' jest niezgodny z '%z'!\n",
			"Item '%z' is incompatible with '%z'!\n",
			"Item '%z' is incompatible with '%z'!\n",
			"Item '%z' is incompatible with '%z'!\n",
			"Item '%z' is incompatible with '%z'!\n"
		};

		return string::Combine( msg[Union.GetSystemLanguage()], notEquiped->name, equiped->name );
	}
}