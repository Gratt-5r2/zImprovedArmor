// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	static string SlotPrefix = "ZS_WEAR_";
	static string SoftSkinWearSeparator = "  ";


	static int pow( int a, int b ) {
		if( b == 0 )
			return 1;

		if( b % 2 == 0 )
			return pow( a * a, b / 2 );

		return a * pow( a, b - 1 );
	}



	inline bool Item_IsArmorPart( oCItem* item ) {
		if( !item || (item->wear & ITM_WEAR_TORSO) == ITM_WEAR_TORSO || !item->HasFlag( ITM_CAT_ARMOR ) )
			return false;

		return (uint&)item->wear > ITM_WEAR_LIGHT;
	}



	inline zSTRING SetWearSuffix( const zSTRING& str, const int& wear ) {
		return str.GetWord( SoftSkinWearSeparator ) + Z SoftSkinWearSeparator + Z wear;
	}



	inline zSTRING RemoveWearSuffix( const zSTRING& str ) {
		return str.GetWord( SoftSkinWearSeparator );
	}



	inline int GetFirstWearSlot( const int& wear ) {
		for( int i = 4; i < 32; i++ ) {
			int power = pow( 2, i );
			if( wear & power )
				return power;
		}

		return 0;
	}



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